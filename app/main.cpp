//
// Created by sumeet on 11/18/20.
//
#include <clock.h>
#include <list.h>
#include <glog/logging.h>
#include <evnsq/consumer.h>
#include <evpp/event_loop.h>

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
    clock::VectorClock::init(myPeerId);
    auto context = Context(myPeerId);
    crdt::DoublyLinkedList<char> crdtList(context);
    for (int i = 0; i < 10; ++i) {
        crdtList.insert(i, 'a' + i);
    }
    for (auto item: crdtList.getContents()) {
        LOG(INFO) << item;
    }
    LOG(INFO) << "size: " << crdtList.getContents().size();

    std::string nsqd_tcp_addr("127.0.0.1:4150");
    evpp::EventLoop loop;
    evnsq::Consumer client(&loop, "test", "ch1", evnsq::Option());
    client.SetMessageCallback(&OnMessage);
    client.ConnectToNSQDs(nsqd_tcp_addr);
    loop.Run();
}