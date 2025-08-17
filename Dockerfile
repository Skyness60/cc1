FROM rust:1.83-slim

ENV DEBIAN_FRONTEND=noninteractive

# Toolchain for fcc pipeline: clang (pp+link), llvm (llc), binutils (as), 32-bit libs for -m32
RUN apt update && apt install -y \
    llvm-14 llvm-14-dev \
    clang-14 \
    binutils \
    gcc-multilib libc6-dev-i386 \
    make ca-certificates && \
    ln -sf /usr/bin/llc-14 /usr/local/bin/llc && \
    ln -sf /usr/bin/clang-14 /usr/local/bin/clang && \
    ln -sf /usr/bin/clang-14 /usr/local/bin/cc && \
    rm -rf /var/lib/apt/lists/*

# Pre-copy deps only
WORKDIR /usr/src/cc1
COPY Cargo.toml Cargo.lock ./
RUN mkdir src && echo "fn main() {}" > src/main.rs && \
    cargo build --release && \
    rm -rf src

# Copy sources
COPY . .

# Build
RUN cargo build --release && \
    cp target/release/cc1 /usr/local/bin/cc1

# Default shell
CMD ["bash"]
