# Dockerfile for CSA04 Operating Systems Assessment 3 (CO3)
# Student: S. Sachin | Register Number: 192525045

FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    make \
    libc6-dev \
    bash \
    coreutils \
    time \
    util-linux \
    python3 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . /app

RUN chmod +x run_all_experiments.sh Task*/run_experiments.sh Task*/*.sh 2>/dev/null || true

CMD ["/bin/bash", "./run_all_experiments.sh"]
