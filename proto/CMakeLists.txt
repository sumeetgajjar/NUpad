include_directories(${protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(PROTO_SRC_FILES message.proto)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SRC_FILES})

# Generating a placeholder target to generate the src and header files of proto files
add_library(proto_deps OBJECT ${PROTO_SRCS} ${PROTO_HDRS})

set(PROTO_SRCS ${PROTO_SRCS} PARENT_SCOPE)
set(PROTO_HDRS ${PROTO_HDRS} PARENT_SCOPE)
set(PROTO_HDRS_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)