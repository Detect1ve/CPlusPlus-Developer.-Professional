FROM ubuntu:24.04

RUN apt-get update \
 && apt-get install --no-install-recommends -y \
# For CMake install
 ca-certificates=20240203 \
 wget=1.21.4-1ubuntu4.1 \
# GCC
 build-essential=12.10ubuntu1 \
 libmpc-dev=1.3.1-1build1.1 \
#  Uncomment when g++-15 can be installed via api-get
#  g++-14=14.2.0-4ubuntu2~24.04 \
# GIT
 git=1:2.43.0-1ubuntu7.2 \
# Ninja
 ninja-build=1.11.1-2 \
# For #include <openssl/evp.h>
 libssl-dev=3.0.13-0ubuntu3.5 \
 && apt-get clean all \
 && rm -rf /var/lib/apt/lists/*

SHELL ["/bin/bash", "-o", "pipefail", "-c"]
RUN wget -qO- "https://cmake.org/files/v4.0/cmake-4.0.3-linux-x86_64.tar.gz" | tar --strip-components=1 -xz -C /usr/local

RUN wget -nv https://ftp.gnu.org/gnu/gcc/gcc-15.1.0/gcc-15.1.0.tar.gz \
 && tar -xf gcc-15.1.0.tar.gz
WORKDIR gcc-15.1.0
RUN ./configure -v --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=x86_64-linux-gnu --prefix=/usr/local/gcc-15.1.0 --enable-checking=release --enable-languages=c,c++ --disable-multilib --program-suffix=-15.1.0
RUN make -j$(($(nproc) - 1))
RUN make install
RUN update-alternatives --install /usr/bin/gcc gcc /usr/local/gcc-15.1.0/bin/gcc-15.1.0 10 \
 && update-alternatives --install /usr/bin/g++ g++ /usr/local/gcc-15.1.0/bin/g++-15.1.0 10

# RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 10 \
#  && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 10
