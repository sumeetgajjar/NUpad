set(PROTO_FILES test1.proto test2.proto)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

# Generating a placeholder target to generate the src and header files of proto files
add_library(proto_deps OBJECT ${PROTO_SRCS} ${PROTO_HDRS})
target_include_directories(proto_deps PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${protobuf_INCLUDE_DIRS})
target_link_libraries(proto_deps ${protobuf_LIBRARIES})