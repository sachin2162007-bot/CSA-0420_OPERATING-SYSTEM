#!/usr/bin/env bash
set -euo pipefail

# Task 7 Execution and Evidence Generation Script
# Student: S. Sachin | Reg: 192525045 | Course: CSA04

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

mkdir -p evidence

echo "=== Compiling Task 7 Programs ==="
gcc -std=c11 -Wall -Wextra task7.c -o task7
gcc -std=c11 -Wall -Wextra task7_experiment.c -o task7_experiment
gcc -std=c11 -Wall -Wextra -O2 spawn_bench_1000.c -o spawn_bench_1000

echo "=== Experiment 1: Running task7 standard terminal output ==="
./task7 > evidence/task7_terminal_output.txt 2>&1

echo "=== Experiment 2: Running task7_experiment redirected to file ==="
# Without second fflush and with exit(3), redirection causes duplicate output
./task7_experiment > evidence/task7_exp_redirected.txt 2>&1

echo "=== Experiment 3: Running process-creation benchmark (3 runs + median) ==="
./spawn_bench_1000 > evidence/spawn_bench_1000.txt 2>&1

echo "=== Task 7 Evidence Generation Complete ==="
ls -lh evidence/
