#!/usr/bin/env bash
# =====================================================================
# run_experiments.sh - Task 02 Automated Compilation and Experiment Suite
# Course: CSA04 - Operating Systems | Student: S. Sachin (192525045)
# =====================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "======================================================================"
echo " TASK 02: FILE ALLOCATION & SIZING EXPERIMENT RUNNER"
echo " Student: S. Sachin | Reg: 192525045 | SIMATS Engineering"
echo "======================================================================"

# 1. Compilation
echo "[STEP 1] Compiling all Task 02 programs with GCC C11..."
make clean > /dev/null 2>&1 || true
make all

echo -e "\n[STEP 2] Executing Official File Allocation Simulator (Contiguous & Indexed):"
./file_allocation

echo -e "\n[STEP 3] Executing UNIX Inode and Traversal Cost Calculator:"
./inode_calculator

echo -e "\n[STEP 4] Executing 4 TB Free-Space Management Evaluator:"
./free_space_calculator

echo "======================================================================"
echo " Task 02 Experiment Suite completed successfully."
echo "======================================================================"
