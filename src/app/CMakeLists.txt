add_executable(nupad_app main.cpp)
target_link_libraries(nupad_app PRIVATE nupad_crdt glog::glog gflags::gflags)