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

int main(int argc, char **argv) {
    using namespace nupad;
    google::InitGoogleLogging(argv[0]);
    FLAGS_alsologtostderr = 1;

    nupad::app::AppServer appServer{FLAGS_app_name, FLAGS_nsqd_url};
    appServer.run(9002);
}