cmake_minimum_required(VERSION 3.17)
project(NUpad)

# Setting the compiler options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_COMPILER gcc-7)
set(CMAKE_CXX_COMPILER g++-7)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wall)

include(${PROJECT_SOURCE_DIR}/cmake/conanbuildinfo.cmake)
conan_basic_setup()

# Setting the output dirs
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# finding all dependencies
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
find_package(gflags 2.2.2 REQUIRED COMPONENTS nothreads_static)
find_package(glog 0.4.0 REQUIRED)
find_package(GTest 1.10.0 REQUIRED)

include(ExternalProject)
ExternalProject_Add(
        evpp
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/evpp
        CMAKE_ARGS -DCMAKE_MODULE_PATH=${PROJECT_SOURCE_DIR}/cmake -DEVPP_VCPKG_BUILD=1 -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/3rdparty/install
        BUILD_COMMAND make -j 12
)

enable_testing()
macro(add_test_wrapper target)
    add_test(NAME ${target} COMMAND $<TARGET_FILE:${target}>)
endmacro(add_test_wrapper)
list(APPEND CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR}/cmake)
## Find Protobuf installation
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")
set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
find_program(_PROTOBUF_PROTOC protoc ${CONAN_BIN_DIRS_PROTOBUF} NO_DEFAULT_PATH)

add_subdirectory(app)
add_subdirectory(proto)
add_subdirectory(crdt_lib)
add_subdirectory(server)