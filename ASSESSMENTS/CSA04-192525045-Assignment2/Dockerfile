# Turnkey Ubuntu C11 Execution Container for OS Assessment 2
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    make \
    time \
    bash \
    coreutils \
    procps \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . /app

RUN chmod +x run_all_tasks.sh Task*/scripts/*.sh

CMD ["./run_all_tasks.sh"]
