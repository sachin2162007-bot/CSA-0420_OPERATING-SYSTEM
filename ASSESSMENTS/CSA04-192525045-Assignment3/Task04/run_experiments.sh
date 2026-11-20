#!/usr/bin/env bash
# =====================================================================
# run_experiments.sh - Task 04 Automated Experiment Suite
# Student Name: S. Sachin | Register Number: 192525045
# Course: CSA04 - Operating Systems | Assessment 3 (CO3)
# =====================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "======================================================================"
echo " TASK 04: VIRTUAL MEMORY AND DEMAND PAGING EXPERIMENT RUNNER"
echo " Student: S. Sachin | Reg: 192525045 | CSA04 CO3"
echo "======================================================================"

# 1. Compilation
echo "[STEP 1] Compiling all Task 04 C programs with GCC C11..."
gcc -std=c11 -Wall -Wextra demand_paging_demo.c -o demand_paging_demo
gcc -std=c11 -Wall -Wextra sequential_memory.c -o sequential_memory
gcc -std=c11 -Wall -Wextra random_memory.c -o random_memory
gcc -std=c11 -Wall -Wextra cow_demo.c -o cow_demo
gcc -std=c11 -Wall -Wextra page_replacement_simulator.c -o page_replacement_simulator

echo -e "\n[STEP 2] Running Effective Access Time and Demand Paging Modeler:"
./demand_paging_demo

echo -e "\n[STEP 3] Running Sequential Memory Access Pattern Benchmark:"
./sequential_memory

echo -e "\n[STEP 4] Running Random Memory Access Pattern Benchmark:"
./random_memory

echo -e "\n[STEP 5] Running Copy-on-Write (COW) fork() Investigation:"
./cow_demo

echo -e "\n[STEP 6] Running Page Faults vs Number of Frames Simulator:"
./page_replacement_simulator

echo "======================================================================"
echo " Task 04 Experiment Suite completed successfully."
echo "======================================================================"
