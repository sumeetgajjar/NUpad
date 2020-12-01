//
// Created by madhurjain on 11/28/20.
//

#ifndef NUPAD_APP_APP_SERVER_H_
#define NUPAD_APP_APP_SERVER_H_

#include <iostream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <set>

namespace nupad::app {
using websocketpp::connection_hdl;
using websocketpp::lib::bind;
typedef websocketpp::server<websocketpp::config::asio> server;

struct DocumentThreads {
  std::string peerName;
  std::thread push_to_nsqd;
  std::thread consume_from_nsqd;
};

class AppServer {

    private:
    // cannot use unordered_map here because of owner_less comparator for shared objects
    // https://stackoverflow.com/questions/31378130/c11-unordered-set-with-stdowner-less-like-hashing
    typedef std::map<connection_hdl, std::string, std::owner_less<connection_hdl>>
      con_list;
    server mainServer_;
    con_list connections_;
    std::string serverName_;
    std::uint32_t peerCounter_;
    std::string nsqdAddr_;

    void initConsumeFromNsqd(const std::string& docName, const std::string &channelName);

    void initPushToNsqd(const std::string& docName);

    public:
    explicit AppServer(std::string &serverName, std::string &nsqdAddr);

    void onOpen(connection_hdl hdl);

    void onClose(connection_hdl hdl);

    void onMessage(connection_hdl hdl, server::message_ptr message_ptr);

    void run(uint32_t port);

    void initDocumentForConnection(const std::string &docName, const std::string& peerId);
};

} // namespace nupad::app

#endif // NUPAD_APP_APP_SERVER_H_
