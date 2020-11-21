add_library(nupad_crdt
        src/clock.cpp
        )

target_include_directories(nupad_crdt PUBLIC include)

target_link_libraries(nupad_crdt PRIVATE glog::glog)