# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.  The
# ASF licenses this file to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance with the
# License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations
# under the License.

FROM ubuntu:22.04 AS builder-base
# NOTE WE ARE NOT REMOVEING APT CACHE.
# This should only be used for temp build images that artifacts will be copied from
RUN apt-get update -qq && apt-get install -y -qq \
  curl \
  patch \
  xz-utils

###############################################################################
# Base image that should be used to prepare tools from nuttx-tools
###############################################################################
FROM builder-base AS nuttx-tools

RUN apt-get update -qq && DEBIAN_FRONTEND="noninteractive" apt-get install -y -qq \
  bison \
  cmake \
  flex \
  g++ \
  gawk \
  git \
  gperf \
  libncurses5-dev \
  make

RUN mkdir /tools
WORKDIR /tools

RUN mkdir -p /tools/nuttx-tools
RUN curl -s -L https://bitbucket.org/nuttx/tools/get/9ad3e1ee75c7.tar.gz \
  | tar -C nuttx-tools --strip-components=1 -xz

RUN mkdir bloaty -p \
  && git clone --depth 1 --branch v1.1 https://github.com/google/bloaty bloaty \
  && cd bloaty \
  && cmake -DCMAKE_SYSTEM_PREFIX_PATH=/tools/bloaty \
  && make install

RUN cd nuttx-tools/kconfig-frontends \
  && ./configure --enable-mconf --disable-gconf --disable-qconf --enable-static --prefix=/tools/kconfig-frontends \
  && make install

# Install Rust and targets supported from NuttX
ENV RUST_HOME=/tools/rust
ENV CARGO_HOME=$RUST_HOME/cargo
ENV RUSTUP_HOME=$RUST_HOME/rustup
RUN mkdir -p $CARGO_HOME \
  && curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y \
  && $CARGO_HOME/bin/rustup target add thumbv6m-none-eabi \
  && $CARGO_HOME/bin/rustup target add thumbv7m-none-eabi

CMD [ "/bin/bash" ]

###############################################################################
# Base image that should be used to prepare arch build images
###############################################################################
FROM builder-base AS nuttx-toolchain-base

RUN mkdir /tools
WORKDIR /tools

###############################################################################
# Build image for tool required by ARM builds
###############################################################################
FROM nuttx-toolchain-base AS nuttx-toolchain-arm
# Download the latest ARM clang toolchain prebuilt by ARM
RUN mkdir clang-arm-none-eabi && \
  curl -s -L  "https://github.com/ARM-software/LLVM-embedded-toolchain-for-Arm/releases/download/release-14.0.0/LLVMEmbeddedToolchainForArm-14.0.0-linux.tar.gz" \
  | tar -C clang-arm-none-eabi --strip-components 1 -xz

# Download the latest ARM GCC toolchain prebuilt by ARM
RUN mkdir gcc-arm-none-eabi && \
  curl -s -L  "https://developer.arm.com/-/media/Files/downloads/gnu/12.3.rel1/binrel/arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-eabi.tar.xz" \
  | tar -C gcc-arm-none-eabi --strip-components 1 -xJ

###############################################################################
# Build image for tool required by ARM64 builds
###############################################################################
FROM nuttx-toolchain-base AS nuttx-toolchain-arm64
# Download the latest ARM64 GCC toolchain prebuilt by ARM
RUN mkdir gcc-aarch64-none-elf && \
  curl -s -L  "https://developer.arm.com/-/media/Files/downloads/gnu/11.2-2022.02/binrel/gcc-arm-11.2-2022.02-x86_64-aarch64-none-elf.tar.xz" \
  | tar -C gcc-aarch64-none-elf --strip-components 1 -xJ

###############################################################################
# Build image for tool required by Pinguino builds
###############################################################################
FROM nuttx-toolchain-base AS nuttx-toolchain-pinguino
# Download the pinguino compilers. Note this includes both 8bit and 32bit
# toolchains and builds for multiple host systems. Only copy what is needed.
RUN mkdir pinguino-compilers && \
  curl -s -L "https://github.com/PinguinoIDE/pinguino-compilers/archive/62db5158d7f6d41c6fadb05de81cc31dd81a1958.tar.gz" \
  | tar -C pinguino-compilers --strip-components=2 --wildcards -xz */linux64

###############################################################################
# Build image for tool required by Renesas builds
###############################################################################
FROM nuttx-toolchain-base AS nuttx-toolchain-renesas
# Build Renesas RX GCC toolchain
RUN apt-get update -qq && DEBIAN_FRONTEND="noninteractive" apt-get install -y -qq \
  bison \
  flex \
  g++ \
  gcc \
  libncurses5-dev \
  m4 \
  make \
  texinfo \
  wget \
  bzip2

# Download toolchain source code
RUN mkdir -p /tools/renesas-tools/source/binutils && \
  curl -s -L "https://llvm-gcc-renesas.com/downloads/d.php?f=rx/binutils/8.3.0.202305-gnurx/binutils-2.36.1.tar.gz" \
  | tar -C renesas-tools/source/binutils --strip-components=1 -xz
RUN mkdir -p /tools/renesas-tools/source/gcc && \
  curl -s -L "https://llvm-gcc-renesas.com/downloads/d.php?f=rx/gcc/8.3.0.202305-gnurx/gcc-8.3.0.tar.gz" \
  | tar -C renesas-tools/source/gcc --strip-components=1 -xz
RUN mkdir -p /tools/renesas-tools/source/newlib && \
  curl -s -L "https://llvm-gcc-renesas.com/downloads/d.php?f=rx/newlib/8.3.0.202305-gnurx/newlib-4.1.0.tar.gz" \
  | tar -C renesas-tools/source/newlib --strip-components=1 -xz

# Install binutils
RUN cd renesas-tools/source/binutils && \
  chmod +x ./configure ./mkinstalldirs && \
  mkdir -p /tools/renesas-tools/build/binutils && cd /tools/renesas-tools/build/binutils && \
  /tools/renesas-tools/source/binutils/configure --target=rx-elf --prefix=/tools/renesas-toolchain/rx-elf-gcc --disable-werror &&\
  make && make install
ENV PATH="/tools/renesas-toolchain/rx-elf-gcc/bin:$PATH"

# Install gcc
RUN cd renesas-tools/source/gcc && \
  chmod +x ./contrib/download_prerequisites ./configure ./move-if-change ./libgcc/mkheader.sh && \
  ./contrib/download_prerequisites && \
  sed -i '1s/^/@documentencoding ISO-8859-1\n/' ./gcc/doc/gcc.texi && \
  sed -i 's/@tex/\n&/g' ./gcc/doc/gcc.texi && sed -i 's/@end tex/\n&/g' ./gcc/doc/gcc.texi && \
  mkdir -p /tools/renesas-tools/build/gcc && cd /tools/renesas-tools/build/gcc && \
  /tools/renesas-tools/source/gcc/configure --target=rx-elf --prefix=/tools/renesas-toolchain/rx-elf-gcc \
  --disable-shared --disable-multilib --disable-libssp --disable-libstdcxx-pch --disable-werror --enable-lto \
  --enable-gold --with-pkgversion=GCC_Build_1.02 --with-newlib --enable-languages=c && \
  make && make install
ENV PATH="/tools/renesas-toolchain/rx-elf-gcc/bin:$PATH"

# Install newlib
RUN cd renesas-tools/source/newlib && \
  chmod +x ./configure && \
  mkdir -p /tools/renesas-tools/build/newlib && cd /tools/renesas-tools/build/newlib && \
  /tools/renesas-tools/source/newlib/configure --target=rx-elf --prefix=/tools/renesas-toolchain/rx-elf-gcc && \
  make && make install
RUN cd /tools/renesas-tools/build/gcc && \
  make && make install

###############################################################################
# Build image for tool required by RISCV builds
###############################################################################
FROM nuttx-toolchain-base AS nuttx-toolchain-riscv
# Download the latest RISCV GCC toolchain prebuilt by SiFive
RUN mkdir riscv64-unknown-elf-gcc && \
  curl -s -L "https://static.dev.sifive.com/dev-tools/freedom-tools/v2020.12/riscv64-unknown-elf-toolchain-10.2.0-2020.12.8-x86_64-linux-ubuntu14.tar.gz" \
  | tar -C riscv64-unknown-elf-gcc --strip-components 1 -xz

###############################################################################
# Build image for tool required by SPARC builds
###############################################################################
FROM nuttx-toolchain-base AS nuttx-toolchain-sparc
# Download the SPARC GCC toolchain prebuilt by Gaisler
RUN mkdir sparc-gaisler-elf-gcc && \
  curl -s -L "https://www.gaisler.com/anonftp/bcc2/bin/bcc-2.1.0-gcc-linux64.tar.xz" \
  | tar -C sparc-gaisler-elf-gcc --strip-components 1 -xJ

###############################################################################
# Build image for tool required by ESP32 builds
###############################################################################
FROM nuttx-toolchain-base AS nuttx-toolchain-esp32
# Download the latest ESP32 GCC toolchain prebuilt by Espressif
RUN mkdir -p xtensa-esp32-elf-gcc && \
  curl -s -L "https://github.com/espressif/crosstool-NG/releases/download/esp-12.2.0_20230208/xtensa-esp32-elf-12.2.0_20230208-x86_64-linux-gnu.tar.xz" \
  | tar -C xtensa-esp32-elf-gcc --strip-components 1 -xJ

RUN mkdir -p xtensa-esp32s2-elf-gcc && \
  curl -s -L "https://github.com/espressif/crosstool-NG/releases/download/esp-12.2.0_20230208/xtensa-esp32s2-elf-12.2.0_20230208-x86_64-linux-gnu.tar.xz" \
  | tar -C xtensa-esp32s2-elf-gcc --strip-components 1 -xJ

RUN mkdir -p xtensa-esp32s3-elf-gcc && \
  curl -s -L "https://github.com/espressif/crosstool-NG/releases/download/esp-12.2.0_20230208/xtensa-esp32s3-elf-12.2.0_20230208-x86_64-linux-gnu.tar.xz" \
  | tar -C xtensa-esp32s3-elf-gcc --strip-components 1 -xJ

RUN echo "ESP Binaries: 2022/01/26"
RUN mkdir -p /tools/blobs && cd /tools/blobs \
  && curl -s -L "https://github.com/espressif/esp-nuttx-bootloader/releases/download/latest/bootloader-esp32.bin" -o bootloader-esp32.bin \
  && curl -s -L "https://github.com/espressif/esp-nuttx-bootloader/releases/download/latest/bootloader-esp32c3.bin" -o bootloader-esp32c3.bin \
  && curl -s -L "https://github.com/espressif/esp-nuttx-bootloader/releases/download/latest/bootloader-esp32s2.bin" -o bootloader-esp32s2.bin \
  && curl -s -L "https://github.com/espressif/esp-nuttx-bootloader/releases/download/latest/bootloader-esp32s3.bin" -o bootloader-esp32s3.bin \
  && curl -s -L "https://github.com/espressif/esp-nuttx-bootloader/releases/download/latest/partition-table-esp32.bin" -o partition-table-esp32.bin \
  && curl -s -L "https://github.com/espressif/esp-nuttx-bootloader/releases/download/latest/partition-table-esp32c3.bin" -o partition-table-esp32c3.bin \
  && curl -s -L "https://github.com/espressif/esp-nuttx-bootloader/releases/download/latest/partition-table-esp32s2.bin" -o partition-table-esp32s2.bin \
  && curl -s -L "https://github.com/espressif/esp-nuttx-bootloader/releases/download/latest/partition-table-esp32s3.bin" -o partition-table-esp32s3.bin

###############################################################################
# Build image for tool required by WASM builds
###############################################################################
FROM nuttx-toolchain-base AS nuttx-toolchain-wasm
# Download the latest WASI-enabled WebAssembly C/C++ toolchain prebuilt by WASM
RUN mkdir wasi-sdk && \
  curl -s -L  "https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-19/wasi-sdk-19.0-linux.tar.gz" \
  | tar -C wasi-sdk --strip-components 1 -xz

# Download the latest "wamrc" AOT compiler prebuilt by WAMR
RUN mkdir wamrc && \
  curl -s -L  "https://github.com/bytecodealliance/wasm-micro-runtime/releases/download/WAMR-1.1.2/wamrc-1.1.2-x86_64-ubuntu-20.04.tar.gz" \
  | tar -C wamrc -xz

###############################################################################
# Final Docker image used for running CI system.  This includes all toolchains
# supported by the CI system.
###############################################################################
FROM ubuntu:22.04
LABEL maintainer="dev@nuttx.apache.org"

RUN apt-get update -qq && DEBIAN_FRONTEND="noninteractive" apt-get install -y -qq \
  build-essential \
  curl \
  gcc \
  libssl-dev

RUN mkdir -p cmake && \
    curl -s -L wget https://cmake.org/files/v3.26/cmake-3.26.0.tar.gz \
    | tar -C cmake --strip-components=1 -xz && \
    cd cmake && ./bootstrap && make && make install && rm -rf cmake

RUN dpkg --add-architecture i386
# This is used for the final images so make sure to not store apt cache
# Note: xtensa-esp32-elf-gdb is linked to libpython2.7
RUN apt-get update -qq && DEBIAN_FRONTEND="noninteractive" apt-get install -y -qq --no-install-recommends \
  -o APT::Immediate-Configure=0 \
  avr-libc \
  ccache \
  clang \
  clang-tidy \
  gcc-avr \
  gcc-multilib \
  genromfs \
  gettext \
  git \
  lib32z1-dev \
  libasound2-dev libasound2-dev:i386 \
  libc6-dev-i386 \
  libcurl4-openssl-dev \
  libmp3lame-dev:i386 \
  libmad0-dev:i386 \
  libncurses5-dev \
  libpulse-dev libpulse-dev:i386 \
  libpython2.7 \
  libtinfo5 \
  libusb-1.0-0-dev libusb-1.0-0-dev:i386 \
  libv4l-dev libv4l-dev:i386 \
  libx11-dev libx11-dev:i386 \
  libxext-dev libxext-dev:i386 \
  linux-headers-generic \
  linux-libc-dev:i386 \
  ninja-build \
  qemu-system-arm \
  qemu-system-misc \
  python3 \
  python3-pip \
  python-is-python3 \
  u-boot-tools \
  unzip \
  wget \
  xxd \
  && rm -rf /var/lib/apt/lists/*

# Configure out base setup for adding python packages
ENV PIP_DISABLE_PIP_VERSION_CHECK=true
# This disables the cache with value 0. We do not want caching as it
# increases the images size.
ENV PIP_NO_CACHE_DIR=0
# We are using the minimal python installation from the system so include
# setuptools and also wheel so we can use the binary releases of packages
# instead of requiring them to be compiled.
RUN pip3 install setuptools
RUN pip3 install wheel
RUN pip3 install cmake-format
# Install CodeChecker and use it to statically analyze the code.
# RUN pip3 install CodeChecker
# Install cvt2utf to check for non-UTF characters.
RUN pip3 install cvt2utf
# Install pytest
RUN pip3 install cxxfilt
RUN pip3 install esptool
RUN pip3 install imgtool
RUN pip3 install kconfiglib
RUN pip3 install pexpect==4.8.0
RUN pip3 install pyelftools
RUN pip3 install pyserial==3.5
RUN pip3 install pytest==6.2.5
RUN pip3 install pytest-json==0.4.0
RUN pip3 install pytest-ordering==0.6
RUN pip3 install pytest-repeat==0.9.1

# Used to generate symbol table files

RUN mkdir /tools
WORKDIR /tools

# Pull in the tools we just built for nuttx
COPY --from=nuttx-tools /tools/bloaty/ bloaty/
ENV PATH="/tools/bloaty/bin:$PATH"
COPY --from=nuttx-tools /tools/kconfig-frontends/ kconfig-frontends/
ENV PATH="/tools/kconfig-frontends/bin:$PATH"

# Pull in the Rust toolchain including supported targets
COPY --from=nuttx-tools /tools/rust/ /tools/rust/
ENV CARGO_HOME=/tools/rust/cargo
ENV RUSTUP_HOME=/tools/rust/rustup
ENV PATH="/tools/rust/cargo/bin:$PATH"

# ARM toolchain
COPY --from=nuttx-toolchain-arm /tools/clang-arm-none-eabi/ clang-arm-none-eabi/
# RUN cp /usr/bin/clang-extdef-mapping-10 clang-arm-none-eabi/bin/clang-extdef-mapping
ENV PATH="/tools/clang-arm-none-eabi/bin:$PATH"

COPY --from=nuttx-toolchain-arm /tools/gcc-arm-none-eabi/ gcc-arm-none-eabi/
ENV PATH="/tools/gcc-arm-none-eabi/bin:$PATH"

# ARM64 toolchain
COPY --from=nuttx-toolchain-arm64 /tools/gcc-aarch64-none-elf/ gcc-aarch64-none-elf/
ENV PATH="/tools/gcc-aarch64-none-elf/bin:$PATH"

# MIPS toolchain
COPY --from=nuttx-toolchain-pinguino /tools/pinguino-compilers/p32/ pinguino-compilers/p32/
ENV PATH="/tools/pinguino-compilers/p32/bin:$PATH"

# Renesas toolchain
COPY --from=nuttx-toolchain-renesas /tools/renesas-toolchain/rx-elf-gcc/ renesas-toolchain/rx-elf-gcc/
ENV PATH="/tools/renesas-toolchain/rx-elf-gcc/bin:$PATH"

# RISCV toolchain
COPY --from=nuttx-toolchain-riscv /tools/riscv64-unknown-elf-gcc/ riscv64-unknown-elf-gcc/
ENV PATH="/tools/riscv64-unknown-elf-gcc/bin:$PATH"

# SPARC toolchain
COPY --from=nuttx-toolchain-sparc /tools/sparc-gaisler-elf-gcc/ sparc-gaisler-elf-gcc/
ENV PATH="/tools/sparc-gaisler-elf-gcc/bin:$PATH"

# ESP32 toolchain
COPY --from=nuttx-toolchain-esp32 /tools/xtensa-esp32-elf-gcc/ xtensa-esp32-elf-gcc/
ENV PATH="/tools/xtensa-esp32-elf-gcc/bin:$PATH"

# ESP32-S2 toolchain
COPY --from=nuttx-toolchain-esp32 /tools/xtensa-esp32s2-elf-gcc/ xtensa-esp32s2-elf-gcc/
ENV PATH="/tools/xtensa-esp32s2-elf-gcc/bin:$PATH"

# ESP32-S3 toolchain
COPY --from=nuttx-toolchain-esp32 /tools/xtensa-esp32s3-elf-gcc/ xtensa-esp32s3-elf-gcc/
ENV PATH="/tools/xtensa-esp32s3-elf-gcc/bin:$PATH"

RUN mkdir -p /tools/blobs/esp-bins
COPY --from=nuttx-toolchain-esp32 /tools/blobs/* /tools/blobs/esp-bins/

# WASI-enabled WebAssembly C/C++ toolchain
COPY --from=nuttx-toolchain-wasm /tools/wasi-sdk/ wasi-sdk/
ENV WASI_SDK_PATH="/tools/wasi-sdk"
ENV PATH="/tools/wamr:$PATH"

# Configure ccache
RUN mkdir -p /tools/ccache/bin && \
  ln -sf `which ccache` /tools/ccache/bin/aarch64-none-elf-gcc && \
  ln -sf `which ccache` /tools/ccache/bin/aarch64-none-elf-g++ && \
  ln -sf `which ccache` /tools/ccache/bin/arm-none-eabi-gcc && \
  ln -sf `which ccache` /tools/ccache/bin/arm-none-eabi-g++ && \
  ln -sf `which ccache` /tools/ccache/bin/avr-gcc && \
  ln -sf `which ccache` /tools/ccache/bin/avr-g++ && \
  ln -sf `which ccache` /tools/ccache/bin/cc && \
  ln -sf `which ccache` /tools/ccache/bin/c++ && \
  ln -sf `which ccache` /tools/ccache/bin/clang && \
  ln -sf `which ccache` /tools/ccache/bin/clang++ && \
  ln -sf `which ccache` /tools/ccache/bin/gcc && \
  ln -sf `which ccache` /tools/ccache/bin/g++ && \
  ln -sf `which ccache` /tools/ccache/bin/p32-gcc && \
  ln -sf `which ccache` /tools/ccache/bin/rx-elf-gcc && \
  ln -sf `which ccache` /tools/ccache/bin/riscv64-unknown-elf-gcc && \
  ln -sf `which ccache` /tools/ccache/bin/riscv64-unknown-elf-g++ && \
  ln -sf `which ccache` /tools/ccache/bin/sparc-gaisler-elf-gcc && \
  ln -sf `which ccache` /tools/ccache/bin/sparc-gaisler-elf-g++ && \
  ln -sf `which ccache` /tools/ccache/bin/xtensa-esp32-elf-gcc && \
  ln -sf `which ccache` /tools/ccache/bin/xtensa-esp32-elf-g++ && \
  ln -sf `which ccache` /tools/ccache/bin/xtensa-esp32s2-elf-gcc && \
  ln -sf `which ccache` /tools/ccache/bin/xtensa-esp32s2-elf-g++ && \
  ln -sf `which ccache` /tools/ccache/bin/xtensa-esp32s3-elf-gcc && \
  ln -sf `which ccache` /tools/ccache/bin/xtensa-esp32s3-elf-g++
ENV PATH="/tools/ccache/bin:$PATH"

CMD [ "/bin/bash" ]
