FROM rust:1.83-slim

RUN apt update && apt install -y \
    llvm-14-dev \
    libpolly-14-dev \
    clang-14 \
    gcc-multilib \
    make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /usr/src/cc1
COPY . .

RUN cargo build --release

# Copy the binary to root for easy extraction
RUN cp target/release/cc1 /cc1

CMD ["bash"]
