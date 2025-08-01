FROM rust:1.83-slim

RUN apt update && apt install -y \
    llvm-14-dev \
    libpolly-14-dev \
    clang-14 \
    gcc-multilib \
    make \
    && rm -rf /var/lib/apt/lists/*

# Pré-copie des fichiers de dépendances uniquement
WORKDIR /usr/src/cc1
COPY Cargo.toml Cargo.lock ./
RUN mkdir src && echo "fn main() {}" > src/main.rs
RUN cargo build --release
RUN rm -rf src

# Copie du vrai code source
COPY . .

# Build final
RUN cargo build --release

# Copier le binaire
RUN cp target/release/cc1 /cc1

CMD ["bash"]
