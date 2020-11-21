//
// Created by sumeet on 11/18/20.
//
#include <clock.h>
#include <crdt_list.h>
#include <glog/logging.h>

int main(int argc, char **argv) {
    using namespace nupad;
    LOG(INFO) << "Hello world App";
    clock::VectorClock::init();
    crdt::CRDTList<char> crdtList;
    LOG(INFO) << "size: " << crdtList.getContents().size();
}