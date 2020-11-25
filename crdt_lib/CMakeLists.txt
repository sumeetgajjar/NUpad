add_library(nupad_crdt
        src/clock.cpp
        src/operation.cpp
        src/context.cpp
        src/document.cpp)
target_include_directories(nupad_crdt PUBLIC include)
target_link_libraries(nupad_crdt PUBLIC glog::glog)

FILE(GLOB TEST_FILES test/*.cpp)
add_executable(run_all_tests ${TEST_FILES})
target_include_directories(run_all_tests PRIVATE include)
target_link_libraries(run_all_tests PRIVATE nupad_crdt GTest::GTest)