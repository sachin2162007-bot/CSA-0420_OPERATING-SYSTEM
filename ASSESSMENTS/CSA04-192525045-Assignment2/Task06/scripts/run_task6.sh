#!/usr/bin/env bash
# Task 06 Execution Script - Deadlock & Banker's Algorithm
# Student: S. Sachin (192525045) | Course: CSA04

set -e
echo "=================================================================="
echo " Running Task 06: Deadlock Handling and Banker's Algorithm"
echo "=================================================================="

mkdir -p evidence

echo "[1/4] Compiling bankers_algorithm.c (Assignment PDF TODO 1)..."
gcc -std=c11 -Wall -Wextra src/bankers_algorithm.c -o bankers_algorithm

echo "[2/4] Executing Banker's algorithm and validating expected output..."
./bankers_algorithm | tee evidence/bankers_algorithm_output.txt

echo "[3/4] Compiling and running Banker's algorithm scalability benchmark (10, 100, 1,000)..."
gcc -std=c11 -Wall -Wextra src/bankers_benchmark.c -o bankers_benchmark
./bankers_benchmark | tee evidence/bankers_benchmark_10_100_1000.txt

echo "[4/4] Compiling and running deadlock demonstration and RAG cycle detection..."
gcc -std=c11 -Wall -Wextra -pthread src/deadlock_demo.c -o deadlock_demo
./deadlock_demo > evidence/deadlock_demo_output.txt 2>&1 || true

gcc -std=c11 -Wall -Wextra src/resource_allocation_graph.c -o resource_allocation_graph
./resource_allocation_graph | tee evidence/resource_allocation_graph_output.txt

echo "Task 06 execution and evidence capture complete."
