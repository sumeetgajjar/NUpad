add_library(nupad_crdt
        src/clock.cpp
        src/operation.cpp
        src/list.cpp
        src/document.cpp)
add_dependencies(nupad_crdt proto_deps)
target_include_directories(nupad_crdt PUBLIC include)
target_link_libraries(nupad_crdt PUBLIC glog::glog proto_deps)

add_executable(run_all_tests
        test/run_all_tests.cpp
        test/document_test.cpp
        test/id_test.cpp
        test/list_test.cpp
        test/vector_clock_test.cpp
        test/test_utils.cpp
        )
target_include_directories(run_all_tests PRIVATE include)
target_link_libraries(run_all_tests PRIVATE nupad_crdt GTest::GTest)

add_test_wrapper(run_all_tests)