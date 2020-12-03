//
// Created by madhurjain on 11/28/20.
//
#include <glog/logging.h>
#include <utility>
#include <evnsq/consumer.h>
#include <evnsq/producer.h>
#include "app_server.h"
#include "ui.pb.h"
#include <google/protobuf/util/json_util.h>
#include <document.h>

namespace nupad::app {

    void AppServer::onChangeMessage(common::Change &change, DocHandle &docHandle, ui::RemoteChange &remoteChange) {
        LOG(INFO) << "processing change, changeId: " << change.change_id();
        std::string modifiedContents;
        {
            std::scoped_lock<std::mutex> lk(*docHandle.docMutex);
            docHandle.doc->processChange(change);
            modifiedContents = docHandle.doc->getString();
        }
        remoteChange.set_message_type(nupad::ui::UIMessageType::REMOTE_CHANGE);
        remoteChange.set_content(modifiedContents);
        for (const auto &editor: docHandle.doc->getEditors()) {
            remoteChange.add_editors(editor);
        }

        std::string jsonResponse;
        google::protobuf::util::MessageToJsonString(remoteChange, &jsonResponse, jsonPrintOptions_);
        sendMessage(docHandle.connHdl, jsonResponse);

        remoteChange.clear_message_type();
        remoteChange.clear_content();
        remoteChange.clear_editors();
    }

    void AppServer::initChangeConsumer(DocHandle &docHandle) {
        evnsq::Consumer client(docHandle.changeConsumerLoop, docHandle.doc->getName(), docHandle.peerName,
                               evnsq::Option());

        // Reusing remoteChange object to save on memory
        nupad::ui::RemoteChange remoteChange;

        client.SetMessageCallback([&](const evnsq::Message *msg) {
            nupad::common::Change change;
            change.ParseFromString(msg->body.ToString());
            // Only handle changes from others authors
            if (change.change_id().peer_id() != docHandle.peerName) {
                onChangeMessage(change, docHandle, remoteChange);
            }
            return 0;
        });
        client.ConnectToNSQDs(nsqdAddr_);
        docHandle.changeConsumerLoop->Run();
        LOG(INFO) << "stopping change consumer for Doc: "
                  << docHandle.doc->getName() << ", Peer: " << docHandle.peerName;
    }

    void AppServer::publishBufferedChanges() {
        LOG(INFO) << "Inside NSQD Producer OnReady callback";
        // iterate over all the peer connections
        std::vector<std::tuple<std::string, nupad::common::Change>> bufferedChanges;
        for (auto &con : docConn_) {
            auto docHandle = con.second;
            {
                std::scoped_lock<std::mutex> lk(*docHandle.docMutex);
                if (docHandle.doc->hasChange()) {
                    bufferedChanges.emplace_back(docHandle.doc->getName(), docHandle.doc->getChange());
                }
            }
        }

        LOG(INFO) << "Publishing " << bufferedChanges.size() << " buffered changes";
        for (auto&[docName, change]: bufferedChanges) {
            publisher_.Publish(docName, change.SerializeAsString());
        }
        // if we are here, then we are ready
        nsqdReady_.store(true);
    }

    void AppServer::sendMessage(connection_hdl hdl, const std::string &message) {
        server_.send(hdl, message, websocketpp::frame::opcode::text);
    }

    AppServer::AppServer(std::string serverName, std::string nsqdAddr) :
            serverName_(std::move(serverName)), peerCounter_(0), nsqdAddr_(std::move(nsqdAddr)),
            changePublisherLoop_(),
            publisher_(&changePublisherLoop_, evnsq::Option()),
            publisherLoopThread_([this]() { changePublisherLoop_.Run(); }),
            nsqdReady_(false) {
        using websocketpp::lib::placeholders::_1;
        using websocketpp::lib::placeholders::_2;
        // clear all access channels log except connect and disconnect
        server_.clear_access_channels(websocketpp::log::alevel::all ^ websocketpp::log::alevel::connect ^ websocketpp::log::alevel::disconnect);
        server_.init_asio();

        server_.set_open_handler([this](auto && PH1) { onOpen(PH1); });
        server_.set_close_handler([this](auto && PH1) { onClose(PH1); });
        server_.set_message_handler([this](auto && PH1, auto && PH2) { onMessage(PH1, PH2); });

        jsonPrintOptions_.always_print_enums_as_ints = true;
        jsonPrintOptions_.always_print_primitive_fields = true;
        publisher_.SetReadyCallback([this] { publishBufferedChanges(); });
        publisher_.ConnectToNSQD(nsqdAddr_);
    }

    void AppServer::onOpen(connection_hdl hdl) {
        LOG(INFO) << "Got a new connection: ";
        DocHandle docHandle;
        docHandle.initialized = false;
        docHandle.peerName = getNewPeerName();
        docHandle.connHdl = hdl;
        docConn_[hdl] = docHandle;
        LOG(INFO) << "Size of m connections; " << docConn_.size();
    }

    void AppServer::onClose(connection_hdl hdl) {
        LOG(INFO) << "Removing the connection: ";
        if (docConn_.find(hdl) != docConn_.end()) {
            auto docHandle = docConn_.at(hdl);
            docHandle.changeConsumerLoop->Stop();
            docHandle.changeConsumerThread->join();

            delete docHandle.changeConsumerThread;
            delete docHandle.changeConsumerLoop;
            delete docHandle.docMutex;
            delete docHandle.doc;

            docConn_.erase(hdl);
        }
        LOG(INFO) << "Size of m connections; " << docConn_.size();
    }

    void AppServer::onMessage(connection_hdl hdl,
                              server::message_ptr message_ptr) {
        // call the init method for the document connection
        // assuming message_ptr contains the document name for now
        CHECK(docConn_.find(hdl) != docConn_.end());
        // for now the payload will only contain the doc name as a string
        // there could be multiple incoming messages, switch case for the type
        LOG(INFO) << "received message: " << message_ptr->get_payload();
        if (!docConn_.at(hdl).initialized) {
            initializeDocHandle(message_ptr->get_payload(), docConn_.at(hdl));
        } else {
            processUIInput(message_ptr->get_payload(), docConn_.at(hdl));
        }
    }

    void AppServer::processUIInput(const std::string &rawJson, DocHandle &docHandle) {
        LOG(INFO) << "processing UIInput from Peer: " << docHandle.peerName;
        nupad::ui::UIInput inputMessage;
        auto status = google::protobuf::util::JsonStringToMessage(rawJson, &inputMessage);
        CHECK(status.ok()) << "JSON string to Message conversion failed: " << rawJson;
        CHECK_NE(inputMessage.operation_type(), common::OperationType::NONE) << "Operation type cannot be NONE";
        // the nsqd client buffers the messages within its queue
        {
            std::scoped_lock<std::mutex> lk(*docHandle.docMutex);
            switch (inputMessage.operation_type()) {
                case common::INSERT:
                    docHandle.doc->insert(inputMessage.index(), inputMessage.value());
                    break;
                case common::DELETE:
                    docHandle.doc->remove(inputMessage.index());
                    break;
                default:
                    LOG(FATAL) << "Only INSERT and REMOVE operations are allowed.";
            }
        }
        std::optional<common::Change> change;
        if (nsqdReady_.load()) {
            // always gonna be a local change here; no need to processChange
            {
                std::scoped_lock<std::mutex> lk(*docHandle.docMutex);
                if (docHandle.doc->hasChange()) {
                    change.emplace(docHandle.doc->getChange());
                }
            }
            if (change.has_value()) {
                LOG(INFO) << "publishing changes, changeId: " << change->change_id();
                docChangeCache.at(docHandle.doc->getName()).push_back(change.value());
                publisher_.Publish(docHandle.doc->getName(), change->SerializeAsString());
            }
        } else {
            LOG(INFO) << "NSQD not ready, buffering changes";
        }
    }

    void AppServer::run(uint32_t port) {
        server_.set_reuse_addr(true);
        server_.listen(port);
        server_.start_accept();
        server_.run();
    }

    void AppServer::initializeDocHandle(const std::string &rawJsonMessage, DocHandle &docHandle) {
        LOG(INFO) << "initializing Doc Handle for Peer: " << docHandle.peerName;
        nupad::ui::InitRequest req;
        auto status = google::protobuf::util::JsonStringToMessage(rawJsonMessage, &req);
        CHECK(status.ok()) << "JSON string to Message conversion failed: " << rawJsonMessage;

        docHandle.doc = new Document(docHandle.peerName, req.document_name());
        syncPastChanges(*docHandle.doc);
        docHandle.docMutex = new std::mutex();
        docHandle.changeConsumerLoop = new evpp::EventLoop;
        docHandle.changeConsumerThread = new std::thread(&AppServer::initChangeConsumer, this, std::ref(docHandle));
        sendInitResponse(docHandle);
        docHandle.initialized = true;
        LOG(INFO) << "Doc Handle for Peer: " << docHandle.peerName << " initialized";
    }

    std::string AppServer::getNewPeerName() {
        return serverName_ + "_" + std::to_string(++peerCounter_);
    }


    void AppServer::sendInitResponse(const DocHandle &docHandle) {
        nupad::ui::InitResponse response;
        response.set_message_type(nupad::ui::UIMessageType::INIT_RESPONSE);
        response.set_peer_name(docHandle.peerName);
        response.set_initial_content(docHandle.doc->getString());
        for (const auto &editor: docHandle.doc->getEditors()) {
            // You are not an editor unless you make a change
            if (editor != docHandle.peerName) {
                response.add_editors(editor);
            }
        }

        std::string jsonResponse;
        google::protobuf::util::MessageToJsonString(response, &jsonResponse, jsonPrintOptions_);
        sendMessage(docHandle.connHdl, jsonResponse);
    }

    void AppServer::syncPastChanges(Document &presentDoc) {
        if (docChangeCache.find(presentDoc.getName()) == docChangeCache.end()) {
            docChangeCache[presentDoc.getName()] = std::vector<common::Change>();
            docChangeCache.at(presentDoc.getName()).reserve(1000);
        } else {
            for (auto &change: docChangeCache.at(presentDoc.getName())) {
                presentDoc.processChange(change);
            }
        }
    }
} // namespace nupad::app
