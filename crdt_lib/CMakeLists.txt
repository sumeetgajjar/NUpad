add_library(nupad_crdt
        src/clock.cpp)
target_include_directories(nupad_crdt PUBLIC include)
target_link_libraries(nupad_crdt PRIVATE glog::glog)

add_executable(vector_clock_test test/vector_clock_test.cpp)
target_include_directories(vector_clock_test PRIVATE include)
target_link_libraries(vector_clock_test PRIVATE nupad_crdt GTest::GTest)