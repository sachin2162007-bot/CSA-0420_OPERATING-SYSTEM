#!/usr/bin/env bash
# =====================================================================
# run_experiments.sh - Task 06 Automated Experiment Suite
# Student Name: S. Sachin | Register Number: 192525045
# Course: CSA04 - Operating Systems | Assessment 3 (CO3)
# =====================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "======================================================================"
echo " TASK 06: DISK MANAGEMENT AND RAID SYSTEMS EXPERIMENT RUNNER"
echo " Student: S. Sachin | Reg: 192525045 | CSA04 CO3"
echo "======================================================================"

# 1. Compilation
echo "[STEP 1] Compiling RAID Capacity and Reliability Modelers..."
gcc -std=c11 -Wall -Wextra raid_capacity.c -o raid_capacity
gcc -std=c11 -Wall -Wextra raid_reliability.c -o raid_reliability -lm

echo -e "\n[STEP 2] Evaluating 6-Disk x 4 TB RAID Usable Capacity:"
./raid_capacity

echo -e "\n[STEP 3] Evaluating Probability of Data Loss During Rebuild:"
./raid_reliability

echo -e "\n[STEP 4] Querying System Storage and Swap Information:"
bash disk_information.sh

echo -e "\n[STEP 5] Running Safe Non-Destructive Disk Read Speed Benchmark:"
bash disk_benchmark.sh

echo "======================================================================"
echo " Task 06 Experiment Suite completed successfully."
echo "======================================================================"
