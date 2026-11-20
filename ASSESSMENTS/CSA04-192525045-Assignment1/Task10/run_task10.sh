#!/usr/bin/env bash
set -euo pipefail

# Task 10 Execution and Evidence Generation Script
# Student: S. Sachin | Reg: 192525045 | Course: CSA04

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

mkdir -p evidence

echo "=== Compiling Task 10 Benchmark Program ==="
gcc -std=c11 -Wall -Wextra -pthread -O2 proc_vs_thread.c -o proc_vs_thread

echo "=== Running 3 Benchmark Iterations (proc_vs_thread_run1-3.txt) ==="
./proc_vs_thread 3 > evidence/proc_vs_thread_run1-3.txt 2>&1

echo "=== Running GNU time -v Profiling (proc_vs_thread_time_v.txt) ==="
/usr/bin/time -v ./proc_vs_thread 1 > evidence/proc_vs_thread_time_v.txt 2>&1

echo "=== Task 10 Evidence Generation Complete ==="
ls -lh evidence/
