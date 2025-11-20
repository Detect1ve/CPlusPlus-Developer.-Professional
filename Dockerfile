FROM ubuntu:24.04 AS GCC

RUN apt-get update \
 && apt-get install --no-install-recommends -y \
    build-essential=12.10ubuntu1 \
    ca-certificates=20240203 \
    wget=1.21.4-1ubuntu4.1 \
 && apt-get clean all \
 && rm -rf /var/lib/apt/lists/*

RUN wget --progress=dot:giga https://ftp.gnu.org/gnu/gcc/gcc-15.2.0/gcc-15.2.0.tar.gz \
 && tar -xf gcc-15.2.0.tar.gz \
 && rm -rf gcc-15.2.0.tar.gz

WORKDIR /gcc-15.2.0

# --disable-shared \

RUN ./contrib/download_prerequisites \
 && ./configure -v \
        --disable-multilib \
        --enable-checking=release \
        --enable-languages=c,c++ \
        --enable-pgo=instrument-and-build \
        --prefix=/opt/gcc-15.2.0 \
        --program-suffix=-15.2.0 \
 && make -j$(($(nproc) - 1)) \
 && make install \
 && rm -rf /gcc-15.2.0

FROM ubuntu:24.04

COPY --from=GCC /opt/gcc-15.2.0 /usr/local/gcc-15.2.0

RUN update-alternatives --install /usr/bin/gcc gcc /usr/local/gcc-15.2.0/bin/gcc-15.2.0 100 \
 && update-alternatives --install /usr/bin/g++ g++ /usr/local/gcc-15.2.0/bin/g++-15.2.0 100

RUN apt-get update \
 && apt-get install --no-install-recommends -y \
# CMake
    ca-certificates=20240203 \
    wget=1.21.4-1ubuntu4.1 \
# Clang
    gnupg=2.4.4-2ubuntu17.3 \
    lsb-release=12.0-2 \
    software-properties-common=0.99.49.3 \
# GIT
    git=1:2.43.0-1ubuntu7.3 \
# Make
    make=4.3-4.1build2 \
# Ninja
    ninja-build=1.11.1-2 \
 && apt-get clean all \
 && rm -rf /var/lib/apt/lists/*

SHELL ["/bin/bash", "-o", "pipefail", "-c"]
RUN wget -qO- "https://cmake.org/files/v4.1/cmake-4.1.2-linux-x86_64.tar.gz" | tar --strip-components=1 -xz -C /usr/local

RUN wget --progress=dot:giga https://apt.llvm.org/llvm.sh \
 && chmod +x llvm.sh \
 && ./llvm.sh 21 all \
 && rm llvm.sh

# RUN wget -qO- "https://github.com/danmar/cppcheck/archive/refs/tags/2.18.0.tar.gz" | tar -xz -C /tmp \
#  && cmake -S /tmp/cppcheck-2.18.0 -B /tmp/cppcheck-2.18.0/build -G Ninja \
#  && cmake --build /tmp/cppcheck-2.18.0/build \
#  && cmake --install /tmp/cppcheck-2.18.0/build \
#  && rm -rf /tmp/cppcheck-2.18.0
