//
// Created by madhurjain on 11/28/20.
//
#include <glog/logging.h>
#include <utility>
#include <evnsq/consumer.h>
#include <evpp/event_loop.h>
#include <evnsq/producer.h>
#include "app_server.h"
#include "ui.pb.h"
#include <google/protobuf/util/json_util.h>
#include <document.h>

namespace nupad::app {

void AppServer::initChangeConsumer(DocHandle& docHandle) {
    evpp::EventLoop loop;
    evnsq::Consumer client(&loop, docHandle.doc->getName(), docHandle.peerName, evnsq::Option());
    nupad::ui::Response res;
    // TODO: refactor into its own method
    client.SetMessageCallback([&](const evnsq::Message* msg){
        nupad::common::Change change;
        change.ParseFromString(msg->body.ToString());
        std::string modifiedContents;
        {
            std::scoped_lock<std::mutex> lk(*docHandle.docMutex);
            docHandle.doc->processChange(change);
            modifiedContents = docHandle.doc->getString();
        }
        res.set_message_type(nupad::ui::UIMessageType::RESPONSE);
        res.set_contents(modifiedContents);
        res.set_modified_by(change.change_id().peer_id());
        std::string jsonResponse;
        google::protobuf::util::MessageToJsonString(res, &jsonResponse);
        sendMessage(docHandle.connHdl, jsonResponse);
        res.clear_message_type();
        res.clear_contents();
        res.clear_modified_by();
        return 0;
    });
    client.ConnectToNSQDs(nsqdAddr_);
    loop.Run();
}

void AppServer::initChangeProducer(const std::string& docName) {
    // TODO: dont create producer objects every time
    // TODO: remove event loop variable if you can
    evpp::EventLoop loop;
    evnsq::Producer producer(&loop, evnsq::Option());
    producer.Publish(docName, "test message");
    producer.ConnectToNSQD(nsqdAddr_);
}

void AppServer::sendMessage(connection_hdl hdl, const std::string& message) {
    server_.send(hdl, message, websocketpp::frame::opcode::text);
}

AppServer::AppServer(std::string serverName, std::string nsqdAddr) :
    serverName_(std::move(serverName)), peerCounter_(0), nsqdAddr_(std::move(nsqdAddr)) {
    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;
    server_.init_asio();

    server_.set_open_handler(bind(&AppServer::onOpen, this,
                                  _1));
    server_.set_close_handler(bind(&AppServer::onClose, this,
                                   _1));
    server_.set_message_handler(bind(&AppServer::onMessage, this,
                                     _1, _2));
}

void AppServer::onOpen(connection_hdl hdl) {
    LOG(INFO) << "Got a new connection: ";
    peerCounter_++;
    docConn_[hdl] = {.initialized = false, .peerName = serverName_ + std::to_string(peerCounter_)};
    LOG(INFO) << "Size of m connections; " << docConn_.size();
}

void AppServer::onClose(connection_hdl hdl) {
    LOG(INFO) << "Removing the connection: ";
    if(docConn_.find(hdl) != docConn_.end()) {
        delete docConn_.at(hdl).doc;
        delete docConn_.at(hdl).docMutex;
        delete docConn_.at(hdl).changeConsumer;
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
    if(!docConn_.at(hdl).initialized) {
        initializeDocHandle(message_ptr->get_payload(), docConn_.at(hdl));
    } else {
        processUIInput(message_ptr->get_payload(), docConn_.at(hdl));
    }
}

void AppServer::processUIInput(const std::string& rawJson, DocHandle& docHandle) {
    nupad::ui::UIInput inputMessage;
    auto status = google::protobuf::util::JsonStringToMessage(rawJson, &inputMessage);
    CHECK(status.ok()) << "JSON string to Message conversion failed: " << rawJson;
    CHECK_NE(inputMessage.operation_type(), common::OperationType::NONE) << "Operation type cannot be NONE";
    // the nsqd client buffers the messages within its queue
    {
        std::scoped_lock<std::mutex> lk(*docHandle.docMutex);
        switch(inputMessage.operation_type()) {
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
    google::protobuf::util::MessageToJsonString(response, &jsonResponse);
    sendMessage(docHolder.connHdl, jsonResponse);

    docHolder.doc = new Document(docHolder.peerName, req.document_name());
    docHolder.docMutex = new std::mutex();
    docHolder.changeConsumer = new std::thread(&AppServer::initChangeConsumer, this, std::ref(docHolder));
    docHolder.initialized = true;
}
} // namespace nupad::app
