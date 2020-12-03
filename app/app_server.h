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
#include <ui.pb.h>
#include <evpp/event_loop.h>
#include <evnsq/producer.h>
#include <google/protobuf/util/json_util.h>

namespace nupad::app {
    using websocketpp::connection_hdl;
    using websocketpp::lib::bind;

    class AppServer {
    private:
        typedef websocketpp::server<websocketpp::config::asio> server;
        struct DocHandle {
            bool initialized;
            std::string peerName;
            nupad::Document *doc;
            std::mutex *docMutex;
            evpp::EventLoop *changeConsumerLoop;
            std::thread *changeConsumerThread;
            connection_hdl connHdl;
        };

        // cannot use unordered_map here because of owner_less comparator for shared objects
        // https://stackoverflow.com/questions/31378130/c11-unordered-set-with-stdowner-less-like-hashing
        typedef std::map<connection_hdl, DocHandle, std::owner_less<connection_hdl>> documentConnectionMap;
        server server_;
        documentConnectionMap docConn_;
        std::string serverName_;
        std::uint32_t peerCounter_;
        std::string nsqdAddr_;
        google::protobuf::util::JsonPrintOptions jsonPrintOptions_;

        evpp::EventLoop changePublisherLoop_;
        evnsq::Producer publisher_;
        std::thread publisherLoopThread_;

        std::atomic<bool> nsqdReady_;
        std::unordered_map<std::string, std::vector<common::Change>> docChangeCache;

        void initChangeConsumer(DocHandle &docHandle);

        void publishBufferedChanges();

        void onOpen(connection_hdl hdl);

        void onClose(connection_hdl hdl);

        void onMessage(connection_hdl hdl, server::message_ptr message_ptr);

        void initializeDocHandle(const std::string &rawJsonMessage, DocHandle &docHandle);

        void sendMessage(connection_hdl hdl, const std::string &message);

        void processUIInput(const std::string &rawJson, DocHandle &docHandle);

        void onChangeMessage(common::Change &change, DocHandle &docHandle, nupad::ui::RemoteChange &remoteChange);

        std::string getNewPeerName();

        void sendInitResponse(const DocHandle& handle);

        void syncPastChanges(Document &presentDoc);
    public:
        explicit AppServer(std::string serverName, std::string nsqdAddr);

        void run(uint32_t port);
    };

} // namespace nupad::app

#endif // NUPAD_APP_APP_SERVER_H_
