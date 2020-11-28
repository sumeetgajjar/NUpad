//
// Created by madhurjain on 11/28/20.
//

#ifndef NUPAD_APP_APP_WEBSOCKET_H_
#define NUPAD_APP_APP_WEBSOCKET_H_

#include <iostream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

namespace websocket {
using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
typedef websocketpp::server<websocketpp::config::asio> server;
// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;
void on_message(server *s, websocketpp::connection_hdl hdl, message_ptr msg);
void run_websocket_thread();
} // namespace websocket

#endif // NUPAD_APP_APP_WEBSOCKET_H_
