add_library(nupad_crdt
        src/clock.cpp)
target_include_directories(nupad_crdt PUBLIC include)
target_link_libraries(nupad_crdt PRIVATE glog::glog)

add_executable(nupad_crdt_test
        test/crdt_unit_tests.cpp)
target_include_directories(nupad_crdt_test PRIVATE include)
target_link_libraries(nupad_crdt_test PRIVATE GTest::GTest)