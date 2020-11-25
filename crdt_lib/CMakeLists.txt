add_library(nupad_crdt
        src/clock.cpp
        src/operation.cpp
        src/context.cpp
        src/document.cpp)
target_include_directories(nupad_crdt PUBLIC include)
target_link_libraries(nupad_crdt glog::glog)

add_executable(vector_clock_test test/vector_clock_test.cpp)
target_include_directories(vector_clock_test PRIVATE include)
target_link_libraries(vector_clock_test PRIVATE nupad_crdt GTest::GTest)

add_executable(crdt_list_test test/crdt_list_test.cpp test/test_utils.h)
target_include_directories(crdt_list_test PRIVATE include)
target_link_libraries(crdt_list_test PRIVATE nupad_crdt GTest::GTest)

add_test_wrapper(crdt_list_test)
add_test_wrapper(vector_clock_test)