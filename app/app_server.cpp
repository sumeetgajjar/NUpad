//
// Created by madhurjain on 11/28/20.
//
#include <glog/logging.h>
#include <utility>
#include <evnsq/consumer.h>
#include <evpp/event_loop.h>
#include <evnsq/producer.h>
#include "app_server.h"

namespace nupad::app {

void AppServer::initConsumeFromNsqd(const std::string& docName, const std::string &channelName) {
    evpp::EventLoop loop;
    evnsq::Consumer client(&loop, docName, channelName, evnsq::Option());
    // the OnMessage callback will call the document.processChange() method
    // and then create a PROTO message for the UI with document.getContents()
    //client.SetMessageCallback(&OnMessage);
    client.ConnectToNSQDs(nsqdAddr_);
    loop.Run();
}

// TODO: need the UIinput proto message here as well
void AppServer::initPushToNsqd(const std::string& docName) {
    evpp::EventLoop loop;
    evnsq::Producer producer(&loop, evnsq::Option());
    producer.SetReadyCallback([&]{
        // call publish topic method here
        producer.Publish(docName, "test message");
    });
    producer.ConnectToNSQD(nsqdAddr_);
}

AppServer::AppServer(std::string& serverName, std::string& nsqdAddr) :
    serverName_(std::move(serverName)), peerCounter_(0), nsqdAddr_(std::move(nsqdAddr)) {
    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;
    mainServer_.init_asio();

    mainServer_.set_open_handler(bind(&AppServer::onOpen, this,
                                      _1));
    mainServer_.set_close_handler(bind(&AppServer::onClose, this,
                                       _1));
    mainServer_.set_message_handler(bind(&AppServer::onMessage, this,
                                         _1, _2));
}

void AppServer::onOpen(connection_hdl hdl) {
    LOG(INFO) << "Got a new connection: ";
    peerCounter_++;
    connections_[hdl] = serverName_ + std::to_string(peerCounter_);
    LOG(INFO) << "Size of m connections; " << connections_.size();
}

void AppServer::onClose(connection_hdl hdl) {
    LOG(INFO) << "Removing the connection: ";
    connections_.erase(hdl);
    LOG(INFO) << "Size of m connections; " << connections_.size();
}

void AppServer::onMessage(connection_hdl hdl,
                          server::message_ptr message_ptr) {
    // call the init method for the document connection
    // assuming message_ptr contains the document name for now
    auto it = connections_.find(hdl);
    if(it != connections_.end()) {
        // for now the payload will only contain the doc name as a string
        // there could be multiple incoming messages, switch case for the type
        LOG(INFO) << "received message: " << message_ptr->get_payload();
        initDocumentForConnection(message_ptr->get_payload(),
                                  connections_.at(hdl));
    }
}

void AppServer::run(uint32_t port) {
    mainServer_.set_reuse_addr(true);
    mainServer_.listen(port);
    mainServer_.start_accept();
    mainServer_.run();
}
void AppServer::initDocumentForConnection(const std::string &docName, const std::string& peerId) {
    // TODO: need to implement this
    // store the thread ref in the struct defined
    std::thread t1(&AppServer::initConsumeFromNsqd, this, docName, peerId);
    std::thread t2(&AppServer::initPushToNsqd, this, docName);
}
} // namespace nupad::app
