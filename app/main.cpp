//
// Created by sumeet on 11/18/20.
//
#include "clock.h"
#include "list.h"
#include "test1.pb.h"
#include <glog/logging.h>
#include <evnsq/consumer.h>
#include <evpp/event_loop.h>
#include <google/protobuf/util/json_util.h>


int OnMessage(const evnsq::Message *msg) {
    LOG(INFO) << "Received a message, id=" << msg->id << " message=[" << msg->body.ToString() << "]";
    return 0;
}

int main(int argc, char **argv) {
    using namespace nupad;
    google::InitGoogleLogging(argv[0]);
    FLAGS_alsologtostderr = 1;
    LOG(INFO) << "Hello world App";
    auto myPeerId = "1";
    auto context = Context(myPeerId);
    crdt::DoublyLinkedList<char> crdtList(context);
    for (int i = 0; i < 10; ++i) {
        crdtList.insert(i, 'a' + i);
    }
    for (auto item: crdtList.getContents()) {
        LOG(INFO) << item;
    }
    LOG(INFO) << "size: " << crdtList.getContents().size();

    common::Message1 m;
    m.set_id(11);
    LOG(INFO) << "Message: " << m.DebugString();
    std::string jsonOut;
    google::protobuf::util::JsonOptions jsonOptions;
    google::protobuf::util::MessageToJsonString(m, &jsonOut, jsonOptions);
    LOG(INFO) << "JSON Message: " << jsonOut;

    std::string nsqd_tcp_addr("127.0.0.1:4150");
    nsqd_tcp_addr = "127.0.0.1:4150";
    std::string lookupd_http_url("http://127.0.0.1:4161/lookup?topic=test");

    if (argc == 2) {
        if (strncmp(argv[1], "http", 4) == 0) {
            lookupd_http_url = argv[1];
        } else {
            nsqd_tcp_addr = argv[1];
        }
    }

    evpp::EventLoop loop;
    evnsq::Consumer client(&loop, "test", "ch1", evnsq::Option());
    client.SetMessageCallback(&OnMessage);
    client.ConnectToNSQDs(nsqd_tcp_addr);
    loop.Run();
}