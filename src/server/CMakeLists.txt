add_executable(nupad_server main.cpp)
target_link_libraries(nupad_server PRIVATE glog::glog gflags::gflags)