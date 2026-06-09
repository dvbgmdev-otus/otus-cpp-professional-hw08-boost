FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libboost-filesystem-dev \
    libboost-program-options-dev \
    libboost-system-dev \
    libboost-test-dev \
    clang-tidy \
    lcov \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
