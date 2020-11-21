add_executable(nupad_server main.cpp)
add_dependencies(nupad_server nupad_crdt)
target_link_libraries(nupad_server PRIVATE glog::glog)