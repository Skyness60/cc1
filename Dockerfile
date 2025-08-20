FROM rust:1.83-slim

ENV DEBIAN_FRONTEND=noninteractive

# Toolchain for fcc pipeline: clang (pp+link), llvm (llc), binutils (as), 32-bit libs for -m32
RUN apt update && apt install -y \
    binutils gcc-multilib libc6-dev-i386 \
    make ca-certificates \
    llvm-19 llvm-19-dev clang-19 && \
    ln -sf /usr/bin/llc-19 /usr/local/bin/llc && \
    ln -sf /usr/bin/clang-19 /usr/local/bin/clang && \
    ln -sf /usr/bin/clang-19 /usr/local/bin/cc && \
    rm -rf /var/lib/apt/lists/*

# Set working directory to /workspace for consistent behavior
WORKDIR /workspace

# Pre-copy deps only
COPY Cargo.toml Cargo.lock ./
RUN mkdir src && echo "fn main() {}" > src/main.rs && \
    cargo build --release && \
    rm -rf src

# Copy sources
COPY . .

# Build and install cc1
RUN cargo build --release && \
    cp target/release/cc1 /usr/local/bin/cc1 && \
    chmod +x /usr/local/bin/cc1

# Verify all tools are available
RUN clang --version && llc --version && as --version && /usr/local/bin/cc1 --help || true

# Default shell
CMD ["bash"]
