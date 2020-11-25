add_library(nupad_crdt
        src/clock.cpp
        src/operation.cpp
        src/context.cpp
        src/document.cpp)
target_include_directories(nupad_crdt PUBLIC include)
target_link_libraries(nupad_crdt PUBLIC glog::glog)

add_executable(run_all_tests test/vector_clock_test.cpp test/run_all_tests.cpp test/crdt_list_test.cpp)
target_include_directories(run_all_tests PRIVATE include)
target_link_libraries(run_all_tests PRIVATE nupad_crdt GTest::GTest)