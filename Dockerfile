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

COPY install-dependencies.sh conanfile.txt NUpad/
RUN cd NUpad && chmod +x install-dependencies.sh && ./install-dependencies.sh

COPY CMakeLists.txt NUpad/
COPY src NUpad/src
WORKDIR NUpad/build
RUN cmake .. && make

FROM ubuntu:18.04
ENV GLOG_alsologtostderr=1
COPY --from=buildStage /NUpad/build/NUpad /
ENTRYPOINT ["/NUpad"]