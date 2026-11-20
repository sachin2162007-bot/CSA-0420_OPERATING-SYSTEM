#!/usr/bin/env bash
# =====================================================================
# run_experiments.sh - Task 07 Automated Experiment Suite
# Student Name: S. Sachin | Register Number: 192525045
# Course: CSA04 - Operating Systems | Assessment 3 (CO3)
# =====================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "======================================================================"
echo " TASK 07: MEMORY MANAGEMENT PLAN FROM FIRST PRINCIPLES RUNNER"
echo " Student: S. Sachin | Reg: 192525045 | CSA04 CO3"
echo "======================================================================"

# 1. Compilation
echo "[STEP 1] Compiling all Task 07 C programs with GCC C11..."
gcc -std=c11 -Wall -Wextra realtime_memory_lock.c -o realtime_memory_lock
gcc -std=c11 -Wall -Wextra memory_management_simulator.c -o memory_management_simulator
gcc -std=c11 -Wall -Wextra page_replacement_simulator.c -o page_replacement_simulator

echo -e "\n[STEP 2] Running Real-Time Memory Locking Worst-Case Delay Evaluator:"
./realtime_memory_lock

echo -e "\n[STEP 3] Running 3-Task Baseline and 4-Task Scaling Memory Simulator:"
./memory_management_simulator

echo -e "\n[STEP 4] Running Partitioned Page Replacement Verification:"
./page_replacement_simulator

echo "======================================================================"
echo " Task 07 Experiment Suite completed successfully."
echo "======================================================================"
