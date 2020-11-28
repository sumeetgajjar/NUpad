add_library(nupad_crdt
        src/clock.cpp
        src/operation.cpp
        src/context.cpp
        src/document.cpp
        ${PROTO_SRCS})
add_dependencies(nupad_crdt proto_deps)
target_include_directories(nupad_crdt PUBLIC include ${protobuf_INCLUDE_DIRS} ${PROTO_HDRS_DIR})
target_link_libraries(nupad_crdt PUBLIC glog::glog ${protobuf_LIBRARIES})

FILE(GLOB TEST_FILES test/*.cpp)
add_executable(run_all_tests ${TEST_FILES})
target_include_directories(run_all_tests PRIVATE include)
target_link_libraries(run_all_tests PRIVATE nupad_crdt GTest::GTest)

add_test_wrapper(run_all_tests)