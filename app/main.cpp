//
// Created by sumeet on 11/18/20.
//
#include <clock.h>
#include <list.h>
#include <glog/logging.h>


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
}