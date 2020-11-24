# Include generated *.pb.h files
# hack to get to the generated files in proto binary directory
include_directories("${CMAKE_CURRENT_BINARY_DIR}/..")

# As per https://github.com/inexorgame/conan-grpc/issues/42; grpc_cronet is missing in the build
list(REMOVE_ITEM CONAN_LIBS grpc_cronet)
message(STATUS "${CONAN_LIBS}")

add_executable(nupad_server main.cpp $<TARGET_OBJECTS:hw_proto_obj>)
add_dependencies(nupad_server nupad_crdt)
# dirty: need to link all the libraries
target_link_libraries(nupad_server PRIVATE glog::glog
        ${_PROTOBUF_LIBPROTOBUF} ${CONAN_LIBS})