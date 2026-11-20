#!/usr/bin/env bash
# Task 02 Execution Script - CPU Scheduling & Quantum Sweep
# Student: S. Sachin (192525045) | Course: CSA04

set -e
echo "=================================================================="
echo " Running Task 02: CPU Scheduling (Cloud Video-Encoding Service)"
echo "=================================================================="

mkdir -p evidence

echo "[1/4] Compiling Round Robin (round_robin.c)..."
gcc -std=c11 -Wall -Wextra src/round_robin.c -o round_robin

echo "[2/4] Executing Round Robin with default quantum = 3 (Assignment PDF)..."
./round_robin 3 | tee evidence/round_robin_q3_output.txt

echo "[3/4] Running Time Quantum Sweep (q = 1, 3, 5, 20)..."
echo "=== ROUND ROBIN TIME QUANTUM SWEEP ===" > evidence/round_robin_quantum_sweep.txt
for q in 1 3 5 20; do
    echo "--------------------------------------------------------" >> evidence/round_robin_quantum_sweep.txt
    echo "QUANTUM = $q" >> evidence/round_robin_quantum_sweep.txt
    echo "--------------------------------------------------------" >> evidence/round_robin_quantum_sweep.txt
    ./round_robin $q >> evidence/round_robin_quantum_sweep.txt
    echo "" >> evidence/round_robin_quantum_sweep.txt
done

echo "[4/4] Compiling and running complete scheduling comparison..."
gcc -std=c11 -Wall -Wextra src/scheduling_comparison.c -o scheduling_comparison
./scheduling_comparison | tee evidence/scheduling_all_algorithms_comparison.txt

echo "Task 02 execution and evidence capture complete."
