#!/usr/bin/env bash
# Task 04 Execution Script - IPC Pipelines & Benchmarks
# Student: S. Sachin (192525045) | Course: CSA04

set -e
echo "=================================================================="
echo " Running Task 04: Inter-Process Communication (Weather Pipeline)"
echo "=================================================================="

mkdir -p evidence

echo "[1/4] Compiling pipe_ipc.c (Assignment PDF TODO 1)..."
gcc -std=c11 -Wall -Wextra src/pipe_ipc.c -o pipe_ipc

echo "[2/4] Executing standard pipe_ipc..."
./pipe_ipc | tee evidence/pipe_ipc_output.txt

echo "[3/4] Testing pipe EOF hang analysis (omitting close(fd[1]))..."
./pipe_ipc hang > evidence/pipe_eof_hang_analysis.txt 2>&1 || true

echo "[4/4] Compiling and running alternative IPC mechanisms and benchmarks..."
gcc -std=c11 -Wall -Wextra -pthread src/shared_memory_ipc.c -o shared_memory_ipc -lrt 2>/dev/null || gcc -std=c11 -Wall -Wextra -pthread src/shared_memory_ipc.c -o shared_memory_ipc
./shared_memory_ipc | tee evidence/shared_memory_ipc_output.txt

gcc -std=c11 -Wall -Wextra -pthread src/message_queue_ipc.c -o message_queue_ipc -lrt 2>/dev/null || gcc -std=c11 -Wall -Wextra -pthread src/message_queue_ipc.c -o message_queue_ipc
./message_queue_ipc | tee evidence/message_queue_ipc_output.txt

gcc -std=c11 -Wall -Wextra src/ipc_benchmark.c -o ipc_benchmark
./ipc_benchmark | tee evidence/ipc_benchmark_1mb_10mb_100mb.txt

echo "Task 04 execution and evidence capture complete."
