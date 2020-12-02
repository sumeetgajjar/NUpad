//
// Created by madhurjain on 11/28/20.
//

#ifndef NUPAD_APP_APP_SERVER_H_
#define NUPAD_APP_APP_SERVER_H_

#include <iostream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <set>
#include <document.h>

namespace nupad::app {
using websocketpp::connection_hdl;
using websocketpp::lib::bind;
typedef websocketpp::server<websocketpp::config::asio> server;

struct DocHandle {
  bool initialized;
  std::string peerName;
  nupad::Document *doc;
  std::mutex *docMutex;
  std::thread* changeConsumer;
  connection_hdl connHdl;
};

class AppServer {

    private:
    // cannot use unordered_map here because of owner_less comparator for shared objects
    // https://stackoverflow.com/questions/31378130/c11-unordered-set-with-stdowner-less-like-hashing
    typedef std::map<connection_hdl, DocHandle, std::owner_less<connection_hdl>>
      documentConnectionMap;
    server server_;
    documentConnectionMap docConn_;
    std::string serverName_;
    std::uint32_t peerCounter_;
    std::string nsqdAddr_;

    void initChangeConsumer(DocHandle& docHandle);

    void initChangeProducer(const std::string& docName);

    public:
    explicit AppServer(std::string serverName, std::string nsqdAddr);

    void onOpen(connection_hdl hdl);

    void onClose(connection_hdl hdl);

    void onMessage(connection_hdl hdl, server::message_ptr message_ptr);

    void run(uint32_t port);

    void initializeDocHandle(const std::string &rawJsonMessage, DocHandle& docHolder);
    void sendMessage(connection_hdl hdl, const std::string &message);
    void processUIInput(const std::string &rawJson, DocHandle &docHandle);
};

} // namespace nupad::app

#endif // NUPAD_APP_APP_SERVER_H_
