# cc1 Dockerfile
# Ubuntu 22.04 with LLVM 17 toolchain, clang/lld/llc/opt alternatives, and build essentials.

FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install LLVM 17, clang-17, lld-17, lldb-17, and build tools.
RUN apt-get update \
 && apt-get install -y wget gnupg ca-certificates lsb-release build-essential \
 && wget https://apt.llvm.org/llvm.sh \
 && chmod +x llvm.sh \
 && ./llvm.sh 17 \
 && apt-get install -y clang-17 lldb-17 lld-17 llvm-17-tools \
 && update-alternatives --install /usr/bin/clang clang /usr/bin/clang-17 100 \
 && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-17 100 \
 && update-alternatives --install /usr/bin/llc llc /usr/bin/llc-17 100 \
 && update-alternatives --install /usr/bin/opt opt /usr/bin/opt-17 100 \
 && rm -f llvm.sh \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /work
