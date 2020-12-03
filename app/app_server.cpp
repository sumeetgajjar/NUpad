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
        std::string modifiedContents;
        {
            std::scoped_lock<std::mutex> lk(*docHandle.docMutex);
            docHandle.doc->processChange(change);
            modifiedContents = docHandle.doc->getString();
        }
        remoteChange.set_message_type(nupad::ui::UIMessageType::REMOTE_CHANGE);
        remoteChange.set_contents(modifiedContents);
        remoteChange.set_modified_by(change.change_id().peer_id());

        std::string jsonResponse;
        google::protobuf::util::MessageToJsonString(remoteChange, &jsonResponse, jsonPrintOptions_);
        sendMessage(docHandle.connHdl, jsonResponse);

        remoteChange.clear_message_type();
        remoteChange.clear_contents();
        remoteChange.clear_modified_by();
    }

    void AppServer::initChangeConsumer(DocHandle &docHandle) {
        evnsq::Consumer client(docHandle.changeConsumerLoop, docHandle.doc->getName(), docHandle.peerName,
                               evnsq::Option());

        // Reusing remoteChange object to save on memory
        nupad::ui::RemoteChange remoteChange;

        client.SetMessageCallback([&](const evnsq::Message *msg) {
            nupad::common::Change change;
            change.ParseFromString(msg->body.ToString());
            onChangeMessage(change, docHandle, remoteChange);
            return 0;
        });
        client.ConnectToNSQDs(nsqdAddr_);
        docHandle.changeConsumerLoop->Run();
        LOG(INFO) << "stopping change consumer for Doc: "
                  << docHandle.doc->getName() << ", Peer: " << docHandle.peerName;
    }

    void AppServer::publishBufferedChanges() {
        // iterate over all the peer connections
        std::vector<std::tuple<std::string, nupad::common::Change>> bufferedChanges;
        for(auto & con : docConn_) {
            auto docHandle = con.second;
            {
                std::scoped_lock<std::mutex> lk(*docHandle.docMutex);
                if (docHandle.doc->hasChange()) {
                    bufferedChanges.emplace_back(docHandle.doc->getName(), docHandle.doc->getChange());
                }
            }
        }
        for(auto& [docName, change]: bufferedChanges) {
            producer_.Publish(docName, change.SerializeAsString());
        }
        // if we are here, then we are ready
        nsqdReady_.store(true);
    }

    void AppServer::sendMessage(connection_hdl hdl, const std::string &message) {
        server_.send(hdl, message, websocketpp::frame::opcode::text);
    }

    AppServer::AppServer(std::string serverName, std::string nsqdAddr) :
            serverName_(std::move(serverName)), peerCounter_(0), nsqdAddr_(std::move(nsqdAddr)),
            producer_(&changeProducerLoop_, evnsq::Option()), nsqdReady_(false) {
        using websocketpp::lib::placeholders::_1;
        using websocketpp::lib::placeholders::_2;
        server_.init_asio();

        // TODO: change bind to lambda
        server_.set_open_handler(bind(&AppServer::onOpen, this,
                                      _1));
        server_.set_close_handler(bind(&AppServer::onClose, this,
                                       _1));
        server_.set_message_handler(bind(&AppServer::onMessage, this,
                                         _1, _2));

        jsonPrintOptions_.always_print_enums_as_ints = true;
        producer_.SetReadyCallback(bind(&AppServer::publishBufferedChanges, this));
        producer_.ConnectToNSQD(nsqdAddr_);
    }

    void AppServer::onOpen(connection_hdl hdl) {
        LOG(INFO) << "Got a new connection: ";
        DocHandle docHandle;
        docHandle.initialized = false;
        docHandle.peerName = serverName_ + "_" + std::to_string(++peerCounter_);
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
        if(nsqdReady_.load()) {
            // always gonna be a local change here; no need to processChange
            {
                std::scoped_lock<std::mutex> lk(*docHandle.docMutex);
                if(docHandle.doc->hasChange()) {
                    change.emplace(docHandle.doc->getChange());
                }
            }
            if(change.has_value()) {
                producer_.Publish(docHandle.doc->getName(), change->SerializeAsString());
            }
        }
    }

    void AppServer::run(uint32_t port) {
        server_.set_reuse_addr(true);
        server_.listen(port);
        server_.start_accept();
        server_.run();
    }

    void AppServer::initializeDocHandle(const std::string &rawJsonMessage, DocHandle &docHolder) {
        nupad::ui::InitRequest req;
        auto status = google::protobuf::util::JsonStringToMessage(rawJsonMessage, &req);
        CHECK(status.ok()) << "JSON string to Message conversion failed: " << rawJsonMessage;

        nupad::ui::InitResponse response;
        response.set_message_type(nupad::ui::UIMessageType::INIT_RESPONSE);
        response.set_peer_name(docHolder.peerName);
        std::string jsonResponse;
        google::protobuf::util::MessageToJsonString(response, &jsonResponse, jsonPrintOptions_);
        sendMessage(docHolder.connHdl, jsonResponse);

        docHolder.doc = new Document(docHolder.peerName, req.document_name());
        docHolder.docMutex = new std::mutex();
        docHolder.changeConsumerLoop = new evpp::EventLoop;
        docHolder.changeConsumerThread = new std::thread(&AppServer::initChangeConsumer, this, std::ref(docHolder));
        docHolder.initialized = true;
    }
} // namespace nupad::app
