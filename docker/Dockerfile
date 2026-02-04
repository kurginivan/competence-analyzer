FROM ubuntu:22.04

# Set working directory
WORKDIR /app

# Install dependencies (console version only)
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    postgresql-client \
    libpq-dev \
    libpqxx-dev \
    pkg-config \
    curl \
    wget \
    && rm -rf /var/lib/apt/lists/*

# Copy project files
COPY . /app/

# Create build directory and compile
RUN rm -rf /app/build && \
    mkdir -p /app/build && \
    cd /app/build && \
    cmake -DCMAKE_VERBOSE_MAKEFILE=ON .. && \
    make -j$(nproc) || make VERBOSE=1

# Set entrypoint to run console version
ENTRYPOINT ["/app/build/competence-analyzer"]
