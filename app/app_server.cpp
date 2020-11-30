//
// Created by madhurjain on 11/28/20.
//
#include <glog/logging.h>
#include <utility>
#include "app_server.h"

namespace nupad::app {

AppServer::AppServer(std::string& server_name) :
    serverName_(std::move(server_name)), peerCounter_(0) {
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
        LOG(INFO) << "received message: " << message_ptr->get_payload();
        init_document_for_connection(message_ptr->get_payload(), connections_.at(hdl));
    }
}

void AppServer::run(uint32_t port) {
    mainServer_.set_reuse_addr(true);
    mainServer_.listen(port);
    mainServer_.start_accept();
    mainServer_.run();
}
void AppServer::init_document_for_connection(const std::string &docName, const std::string& peerId) {
    // TODO: need to implement this
}
} // namespace nupad::app
