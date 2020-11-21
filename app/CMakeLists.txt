add_executable(nupad_app main.cpp)
add_dependencies(nupad_app nupad_crdt)
target_link_libraries(nupad_app PRIVATE nupad_crdt glog::glog)