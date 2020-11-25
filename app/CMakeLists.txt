include_directories(${PROJECT_SOURCE_DIR}/3rdparty/install/include)

add_executable(nupad_app main.cpp)
add_dependencies(nupad_app nupad_crdt)
target_link_directories(nupad_app PRIVATE ${PROJECT_SOURCE_DIR}/3rdparty/install/lib)
target_link_libraries(nupad_app PRIVATE nupad_crdt)