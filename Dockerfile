FROM ubuntu:24.04 AS gcc

RUN apt-get update \
 && apt-get install --no-install-recommends -y \
    bzip2=1.0.8-5.1build0.1 \
    ca-certificates=20240203 \
    wget=1.21.4-1ubuntu4.1 \
    g++=4:13.2.0-7ubuntu1 \
    make=4.3-4.1build2 \
 && apt-get clean all \
 && rm -rf /var/lib/apt/lists/*

RUN wget --progress=dot:giga https://ftp.gnu.org/gnu/gcc/gcc-16.1.0/gcc-16.1.0.tar.gz \
 && tar -xf gcc-16.1.0.tar.gz \
 && rm -rf gcc-16.1.0.tar.gz

WORKDIR /gcc-16.1.0
RUN ./contrib/download_prerequisites

# --disable-shared \

WORKDIR /gcc_build
RUN ../gcc-16.1.0/configure -v \
        --disable-multilib \
        --enable-checking=release \
        --enable-languages=c,c++ \
        --enable-pgo=instrument-and-build \
        --prefix=/opt/gcc-16.1.0 \
        --program-suffix=-16.1.0 \
 && make -j$(($(nproc) - 1)) \
 && make install \
 && rm -rf /gcc-16.1.0 /gcc_build

FROM ubuntu:24.04

COPY --from=gcc /opt/gcc-16.1.0 /usr/local/gcc-16.1.0

RUN update-alternatives --install /usr/bin/gcc gcc /usr/local/gcc-16.1.0/bin/gcc-16.1.0 100 \
 && update-alternatives --install /usr/bin/g++ g++ /usr/local/gcc-16.1.0/bin/g++-16.1.0 100 \
 && update-alternatives --install /usr/bin/gcc-ar gcc-ar /usr/local/gcc-16.1.0/bin/gcc-ar-16.1.0 100 \
 && update-alternatives --install /usr/bin/gcc-ranlib gcc-ranlib /usr/local/gcc-16.1.0/bin/gcc-ranlib-16.1.0 100

RUN apt-get update \
 && apt-get install --no-install-recommends -y \
# CMake
    ca-certificates=20240203 \
    wget=1.21.4-1ubuntu4.1 \
# Clang
    gpg=2.4.4-2ubuntu17.4 \
    lsb-release=12.0-2 \
    software-properties-common=0.99.49.4 \
# GIT
    git=1:2.43.0-1ubuntu7.3 \
# Make
    make=4.3-4.1build2 \
# Ninja
    ninja-build=1.11.1-2 \
 && apt-get clean \
 && rm -rf /var/lib/apt/lists/*

RUN echo "/usr/local/gcc-16.1.0/lib64" > /etc/ld.so.conf.d/gcc-16.1.0.conf \
 && ldconfig

SHELL ["/bin/bash", "-o", "pipefail", "-c"]
RUN wget -qO- "https://cmake.org/files/v4.3/cmake-4.3.2-linux-x86_64.tar.gz" | tar --strip-components=1 -xz -C /usr/local

RUN wget --progress=dot:giga https://apt.llvm.org/llvm.sh \
 && chmod +x llvm.sh \
 && ./llvm.sh 22 all \
 && rm llvm.sh \
 && apt-get clean \
 && rm -rf /var/lib/apt/lists/*

RUN wget -qO- "https://github.com/danmar/cppcheck/archive/refs/tags/2.20.0.tar.gz" | tar -xz -C /tmp \
 && cmake -S /tmp/cppcheck-2.20.0 -B /tmp/cppcheck-2.20.0/build -G Ninja -D CMAKE_BUILD_TYPE=Release \
 && cmake --build /tmp/cppcheck-2.20.0/build \
 && cmake --install /tmp/cppcheck-2.20.0/build \
 && rm -rf /tmp/cppcheck-2.20.0
