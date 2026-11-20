#!/usr/bin/env bash
# Task 05 Execution Script - Synchronisation, Semaphores & Verification
# Student: S. Sachin (192525045) | Course: CSA04

set -e
echo "=================================================================="
echo " Running Task 05: Synchronisation (Railway Ticket Booking)"
echo "=================================================================="

mkdir -p evidence

echo "[1/6] Compiling bounded_buffer.c (Assignment PDF TODO 1 & 2)..."
gcc -std=c11 -Wall -Wextra -pthread src/bounded_buffer.c -o bounded_buffer

echo "[2/6] Executing standard bounded_buffer (1 prod, 1 cons)..."
./bounded_buffer | tee evidence/bounded_buffer_single_output.txt

echo "[3/6] Running 100 iterations verification check for sum = 210..."
echo "=== 100 RUNS VERIFICATION FOR SUM = 210 ===" > evidence/bounded_buffer_100_runs_verification.txt
ALL_CORRECT=1
for i in $(seq 1 100); do
    RES=$(./bounded_buffer)
    if [[ "$RES" != *"sum = 210 (expected 210)"* ]]; then
        echo "Run $i: MISMATCH -> $RES" >> evidence/bounded_buffer_100_runs_verification.txt
        ALL_CORRECT=0
    fi
done
if [ $ALL_CORRECT -eq 1 ]; then
    echo "SUCCESS: All 100 runs completed with exact sum = 210 (100% Deterministic Guarantee)." >> evidence/bounded_buffer_100_runs_verification.txt
fi

echo "[4/6] Testing 4 producers and 4 consumers..."
./bounded_buffer -multi | tee evidence/bounded_buffer_4prod_4cons_output.txt

echo "[5/6] Testing without mutex to evaluate race condition..."
./bounded_buffer -multi -nomutex > evidence/bounded_buffer_no_mutex_race.txt 2>&1 || true

echo "[6/6] Compiling and running alternative synchronisation models and benchmarks..."
gcc -std=c11 -Wall -Wextra -pthread src/semaphore_demo.c -o semaphore_demo
./semaphore_demo > evidence/semaphore_demo_output.txt

gcc -std=c11 -Wall -Wextra -pthread src/readers_writers.c -o readers_writers
./readers_writers | tee evidence/readers_writers_output.txt

gcc -std=c11 -Wall -Wextra -pthread src/dining_philosophers.c -o dining_philosophers
./dining_philosophers | tee evidence/dining_philosophers_output.txt

gcc -std=c11 -Wall -Wextra -pthread src/monitor_simulation.c -o monitor_simulation
./monitor_simulation | tee evidence/monitor_simulation_output.txt

gcc -std=c11 -Wall -Wextra -pthread src/booking_lock_benchmark.c -o booking_lock_benchmark
./booking_lock_benchmark | tee evidence/booking_lock_granularity_benchmark.txt

gcc -std=c11 -Wall -Wextra -pthread src/atomic_vs_spinlock.c -o atomic_vs_spinlock
./atomic_vs_spinlock | tee evidence/atomic_vs_spinlock_benchmark.txt

echo "Task 05 execution and evidence capture complete."
