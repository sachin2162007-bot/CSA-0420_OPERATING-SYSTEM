#!/usr/bin/env bash
# Task 03 Execution Script - Threads & Speedup
# Student: S. Sachin (192525045) | Course: CSA04

set -e
echo "=================================================================="
echo " Running Task 03: Threads (Traffic Camera Plate Reader)"
echo "=================================================================="

mkdir -p evidence

echo "[1/4] Compiling all camera processing variants with -pthread..."
gcc -std=c11 -Wall -Wextra src/camera_sequential.c -o camera_sequential
gcc -std=c11 -Wall -Wextra -pthread src/camera_threads.c -o camera_threads
gcc -std=c11 -Wall -Wextra -pthread src/camera_thread_pool.c -o camera_thread_pool
gcc -std=c11 -Wall -Wextra -pthread src/race_condition_demo.c -o race_condition_demo

echo "[2/4] Measuring Sequential Execution (64 cameras)..."
./camera_sequential | tee evidence/camera_sequential_output.txt

echo "[3/4] Measuring 1 Thread Per Camera (64 threads)..."
./camera_threads | tee evidence/camera_threads_64_output.txt

echo "[4/4] Benchmarking Thread Pools (4, 8, 16 threads)..."
./camera_thread_pool | tee evidence/camera_thread_pool_benchmark.txt

echo "[5/5] Demonstrating Shared Counter Race Condition..."
./race_condition_demo | tee evidence/race_condition_comparison.txt

echo "Task 03 execution and evidence capture complete."
