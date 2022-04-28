FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install --yes --no-install-recommends \
    g++ \
    libsdl2-dev \
    libsdl2-image-dev \
    libsdl2-mixer-dev \
    libsdl2-net-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libglew-dev \
    libfreetype6-dev \
    libbullet-dev \
    libassimp-dev \
    libfontconfig-dev \
    libmicrohttpd-dev \
    libglm-dev \
    make \
    && apt clean \
    && rm -rf /var/lib/apt/lists/*
