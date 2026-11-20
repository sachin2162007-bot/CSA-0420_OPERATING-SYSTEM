#!/usr/bin/env bash
# =====================================================================
# run_experiments.sh - Task 03 Automated Experiment Suite
# Student Name: S. Sachin | Register Number: 192525045
# Course: CSA04 - Operating Systems | Assessment 3 (CO3)
# =====================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "======================================================================"
echo " TASK 03: SEGMENTATION ARCHITECTURE EXPERIMENT RUNNER"
echo " Student: S. Sachin | Reg: 192525045 | CSA04 CO3"
echo "======================================================================"

# 1. Compilation
echo "[STEP 1] Compiling all Task 03 C programs with GCC C11..."
gcc -std=c11 -Wall -Wextra segmentation_translation.c -o segmentation_translation
gcc -std=c11 -Wall -Wextra segmentation_fault_demo.c -o segmentation_fault_demo
gcc -std=c11 -Wall -Wextra fragmentation_simulator.c -o fragmentation_simulator
gcc -std=c11 -Wall -Wextra address_verification.c -o address_verification

echo -e "\n[STEP 2] Executing Segmentation Translation Engine:"
./segmentation_translation

echo -e "\n[STEP 3] Running 8-Address Verification Workings:"
./address_verification

echo -e "\n[STEP 4] Demonstrating Linux SIGSEGV Signal Handling and MMU Protection:"
./segmentation_fault_demo

echo -e "\n[STEP 5] Running Dynamic Segment Allocation and External Fragmentation Simulator:"
./fragmentation_simulator

echo "======================================================================"
echo " Task 03 Experiment Suite completed successfully."
echo "======================================================================"
