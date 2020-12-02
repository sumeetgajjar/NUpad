//
// Created by sumeet on 11/18/20.
//
#include "clock.h"
#include <glog/logging.h>
#include "app_server.h"

DEFINE_string(app_name, "", "The name of the NUpad app");
DEFINE_validator(app_name, [](const char *, const std::string &value) {
    return !value.empty();
});
DEFINE_string(nsqd_url, "127.0.0.1:4150", "The address of nsqd to connect to");
DEFINE_validator(nsqd_url, [](const char *, const std::string &value) {
    return !value.empty();
});
DEFINE_uint32(app_server_port, 9002, "AppServer listening port");


int main(int argc, char **argv) {
    using namespace nupad;
    google::InitGoogleLogging(argv[0]);
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    nupad::app::AppServer appServer{FLAGS_app_name, FLAGS_nsqd_url};
    LOG(INFO) << "starting AppServer on port: " << FLAGS_app_server_port;
    appServer.run(FLAGS_app_server_port);
}