#!/usr/bin/env bash
# =====================================================================
# run_experiments.sh - Task 05 Automated Experiment Suite
# Student Name: S. Sachin | Register Number: 192525045
# Course: CSA04 - Operating Systems | Assessment 3 (CO3)
# =====================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "======================================================================"
echo " TASK 05: PAGE-REPLACEMENT TECHNIQUES EXPERIMENT RUNNER"
echo " Student: S. Sachin | Reg: 192525045 | CSA04 CO3"
echo "======================================================================"

# 1. Compilation
echo "[STEP 1] Compiling all Task 05 C programs with GCC C11..."
gcc -std=c11 -Wall -Wextra page_replacement.c -o page_replacement
gcc -std=c11 -Wall -Wextra locality_generator.c -o locality_generator
gcc -std=c11 -Wall -Wextra page_replacement_benchmark.c -o page_replacement_benchmark

echo -e "\n[STEP 2] Running Official Assignment Verification (Exact PDF Strings):"
./page_replacement

echo -e "\n[STEP 3] Generating 100,000-Reference Locality and Random Traces:"
./locality_generator

echo -e "\n[STEP 4] Executing 100,000-Reference Scaling Benchmark across 3-10 Frames:"
./page_replacement_benchmark

echo -e "\n[STEP 5] Inspecting Generated Benchmark CSV Data:"
if [ -f data/benchmark_results.csv ]; then
    cat data/benchmark_results.csv
fi

echo "======================================================================"
echo " Task 05 Experiment Suite completed successfully."
echo "======================================================================"
