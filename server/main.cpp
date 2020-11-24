//
// Created by sumeet on 11/18/20.
//
#include <glog/logging.h>
#include <grpc++/grpc++.h>
#include "proto/helloworld.pb.h"
#include "proto/helloworld.grpc.pb.h"

int main(int argc, char **argv) {
    LOG(INFO) << "Hello world Server";
}