FROM ubuntu:18.04 as buildStage

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    wget \
    build-essential \
    git \
    ca-certificates \
    python3.8

RUN wget --no-check-certificate \
    https://github.com/Kitware/CMake/releases/download/v3.17.4/cmake-3.17.4-Linux-x86_64.tar.gz && \
    tar -xzvf cmake-3.17.4-Linux-x86_64.tar.gz && \
    wget --no-check-certificate \
    https://dl.bintray.com/conan/installers/conan-ubuntu-64_1_29_2.deb && \
    dpkg -i conan-ubuntu-64_1_29_2.deb

ENV PATH="/cmake-3.17.4-Linux-x86_64/bin:${PATH}"

RUN mkdir -p NUpad/build

COPY install-build-dependencies.sh conanfile.txt NUpad/
RUN cd NUpad && chmod +x install-build-dependencies.sh && ./install-build-dependencies.sh

COPY 3rdparty/evpp NUpad/3rdparty/evpp
COPY app NUpad/app
COPY crdt_lib NUpad/crdt_lib
COPY proto NUpad/proto

COPY CMakeLists.txt NUpad/
WORKDIR NUpad/build
RUN cmake .. && make && make test

FROM ubuntu:18.04
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    libevent-dev \
    libboost-dev

ENV GLOG_alsologtostderr=1
COPY --from=buildStage /NUpad/build/bin/* /bin/
COPY --from=buildStage /NUpad/build/evpp-prefix/src/evpp-build/lib/libevpp_concurrentqueue.so.0.7 /lib/x86_64-linux-gnu/