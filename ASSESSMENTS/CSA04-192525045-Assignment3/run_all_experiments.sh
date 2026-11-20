#!/usr/bin/env bash
# =====================================================================
# run_all_experiments.sh - Master Test Orchestration Suite
# Assessment 3 (CO3) - Operating Systems
# Student Name: S. Sachin | Register Number: 192525045
# =====================================================================

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"

echo "======================================================================"
echo " OPERATING SYSTEMS ASSESSMENT 3 (CO3) - MASTER EXPERIMENT RUNNER"
echo " Student: S. Sachin | Register Number: 192525045 | Course: CSA04"
echo "======================================================================"

run_task() {
    local task_dir="$1"
    local task_name="$2"
    echo -e "\n######################################################################"
    echo " EXECUTING: $task_name ($task_dir)"
    echo "######################################################################"
    if [ -d "$task_dir" ] && [ -f "$task_dir/run_experiments.sh" ]; then
        (cd "$task_dir" && bash run_experiments.sh)
        echo ">>> $task_name completed successfully."
    else
        echo ">>> ERROR: Script not found in $task_dir"
    fi
}

run_task "Task01" "Task 01 - Linking, Loading and Address Binding"
run_task "Task02" "Task 02 - Paging Architecture and Memory Scaling"
run_task "Task03" "Task 03 - Segmentation and Memory Protection"
run_task "Task04" "Task 04 - Virtual Memory and Demand Paging"
run_task "Task05" "Task 05 - Page-Replacement Techniques"
run_task "Task06" "Task 06 - Disk Management and RAID Systems"
run_task "Task07" "Task 07 - Memory-Management Plan from First Principles"

echo -e "\n======================================================================"
echo " ALL SEVEN ASSESSMENT 3 TASKS COMPLETED SUCCESSFULLY!"
echo " Results and calculation proofs are documented in respective task folders."
echo "======================================================================"
