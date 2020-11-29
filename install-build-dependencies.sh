#!/usr/bin/env bash

set -e
# Dependencies for evpp
apt install -y --no-install-recommends libevent-dev libboost-dev

CMAKE_MODULES_DIR="cmake"
if [ -d ${CMAKE_MODULES_DIR} ]; then
  echo "Removing cmake modules dir: '${CMAKE_MODULES_DIR}'"
  rm -rf ${CMAKE_MODULES_DIR}
fi

echo "Creating cmake modules dir: '${CMAKE_MODULES_DIR}'"
mkdir ${CMAKE_MODULES_DIR}

conan install . \
  --generator cmake_find_package \
  --install-folder=${CMAKE_MODULES_DIR} \
  -s os=Linux \
  -s os_build=Linux \
  -s arch=x86_64 \
  -s arch_build=x86_64 \
  -s compiler=gcc \
  -s compiler.version=7 \
  -s compiler.libcxx=libstdc++11 \
  -s build_type=Release \
  -s cppstd=17

# protobuf_generate_cpp in cmake uses the install protoc binary, it does not default to the
# compiler downloaded by conan. Hence adding the symlink
ln -s "$(ls ~/.conan/data/protobuf/3.12.4/_/_/package/*/bin/protoc)" /usr/bin/protoc