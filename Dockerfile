FROM ubuntu:24.04

RUN apt-get update \
 && apt-get install --no-install-recommends -y \
# For CMake install
 ca-certificates=20240203 \
 wget=1.21.4-1ubuntu4.1 \
# GCC
 g++-14=14.2.0-4ubuntu2~24.04 \
# GIT
 git=1:2.43.0-1ubuntu7.2 \
# Ninja
 ninja-build=1.11.1-2 \
# For #include <openssl/evp.h>
 libssl-dev=3.0.13-0ubuntu3.5 \
 && apt-get clean all \
 && rm -rf /var/lib/apt/lists/*

SHELL ["/bin/bash", "-o", "pipefail", "-c"]
RUN wget -qO- "https://cmake.org/files/v3.31/cmake-3.31.6-linux-x86_64.tar.gz" | tar --strip-components=1 -xz -C /usr/local

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 10 \
 && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 10

# WORKDIR /app
# RUN echo "$PWD"
# RUN pwd
# RUN ls
# RUN pwd
# RUN echo ${PWD}
# RUN --mount=type=bind dst=$(pwd) source=$(pwd)
# WORKDIR /code
# VOLUME . /code
