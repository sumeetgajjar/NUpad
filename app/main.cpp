//
// Created by sumeet on 11/18/20.
//
#include "clock.h"
#include "list.h"
#include <glog/logging.h>
#include "app_server.h"
DEFINE_string(nsqd, "127.0.0.1", "The address of nsqd to connect to");

int main(int argc, char **argv) {
    using namespace nupad;
    google::InitGoogleLogging(argv[0]);
    FLAGS_alsologtostderr = 1;
    LOG(INFO) << "Hello world App";

    // TODO: make it global here?
    std::string server_name = "nupad_app";
    nupad::app::AppServer appServer{server_name, FLAGS_nsqd};
    appServer.run(9002);
}