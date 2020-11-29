//
// Created by madhurjain on 11/28/20.
//

#ifndef NUPAD_APP_APP_WEBSOCKET_H_
#define NUPAD_APP_APP_WEBSOCKET_H_

#include <iostream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <set>

namespace websocket {
using websocketpp::connection_hdl;
using websocketpp::lib::bind;
typedef websocketpp::server<websocketpp::config::asio> server;

class MainWebsocketServer {

    private:
    // cannot use unordered_map here because of owner_less comparator for shared objects
    // https://stackoverflow.com/questions/31378130/c11-unordered-set-with-stdowner-less-like-hashing
    typedef std::map<connection_hdl, std::string, std::owner_less<connection_hdl>>
      con_list;
    server mainServer_;
    con_list connections_;
    std::string serverName_;
    std::uint32_t peerCounter_;

    public:
    explicit MainWebsocketServer(std::string &server_name);

    void onOpen(connection_hdl hdl);

    void onClose(connection_hdl hdl);

    void onMessage(connection_hdl hdl, server::message_ptr message_ptr);

    void run(uint32_t port);

    void init_document_for_connection(const std::string &docName, const std::string& peerId);
};

} // namespace websocket

#endif // NUPAD_APP_APP_WEBSOCKET_H_
