#!/usr/bin/env bash
# =====================================================================
# run_experiments.sh - Task 02 Automated Experiment Suite
# Student Name: S. Sachin | Register Number: 192525045
# Course: CSA04 - Operating Systems | Assessment 3 (CO3)
# =====================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "======================================================================"
echo " TASK 02: PAGING ARCHITECTURE EXPERIMENT RUNNER"
echo " Student: S. Sachin | Reg: 192525045 | CSA04 CO3"
echo "======================================================================"

# 1. Compilation
echo "[STEP 1] Compiling all Task 02 programs with GCC C11..."
gcc -std=c11 -Wall -Wextra paging_translation.c -o paging_translation_1024
gcc -std=c11 -Wall -Wextra -DPAGE_SIZE=2048 paging_translation.c -o paging_translation_2048
gcc -std=c11 -Wall -Wextra page_table_analysis.c -o page_table_analysis
gcc -std=c11 -Wall -Wextra tlb_calculator.c -o tlb_calculator
gcc -std=c11 -Wall -Wextra fragmentation_simulator.c -o fragmentation_simulator
gcc -std=c11 -Wall -Wextra address_boundary_test.c -o address_boundary_test

echo -e "\n[STEP 2] Running Official Paging Translation (PAGE_SIZE = 1024):"
./paging_translation_1024

echo -e "\n[STEP 3] Running Recalculated Paging Translation (PAGE_SIZE = 2048):"
./paging_translation_2048

echo -e "\n[STEP 4] Executing Address Boundary Condition Verification:"
./address_boundary_test

echo -e "\n[STEP 5] Running Multi-Level and Inverted Page Table Scaling Analysis:"
./page_table_analysis

echo -e "\n[STEP 6] Running TLB Effective Access Time (EAT) Benchmarks:"
./tlb_calculator

echo -e "\n[STEP 7] Running Internal Fragmentation Simulation across 100 Processes:"
./fragmentation_simulator

echo -e "\n[STEP 8] Querying Operating System Page Size (if on Linux):"
if command -v getconf &> /dev/null; then
    echo "Host Linux Page Size: $(getconf PAGESIZE) bytes"
else
    echo "getconf not available in current host environment."
fi

echo "======================================================================"
echo " Task 02 Experiment Suite completed successfully."
echo "======================================================================"
