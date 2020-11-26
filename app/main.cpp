//
// Created by sumeet on 11/18/20.
//
#include <clock.h>
#include <list.h>
#include <glog/logging.h>



int main(int argc, char **argv) {
    using namespace nupad;
    LOG(INFO) << "Hello world App";
    auto myPeerId = "1";
    auto context = Context(myPeerId, clock::VectorClock(myPeerId));
    crdt::DoublyLinkedList<char> crdtList(context);
    for (int i = 0; i < 10; ++i) {
        crdtList.insert(i, 'a' + i);
    }
    for (auto item: crdtList.getContents()) {
        LOG(INFO) << item;
    }
    LOG(INFO) << "size: " << crdtList.getContents().size();
}