#!/usr/bin/env bash
# Task 01 Execution Script - Processes and Process States
# Student: S. Sachin (192525045) | Course: CSA04

set -e
echo "=================================================================="
echo " Running Task 01: Processes & Process States (Online Exam Server)"
echo "=================================================================="

mkdir -p evidence

echo "[1/3] Compiling process_demo.c with C11 flags..."
gcc -std=c11 -Wall -Wextra -pedantic src/process_demo.c -o process_demo

echo "[2/3] Executing process_demo and logging terminal output..."
./process_demo 3 | tee evidence/task1_process_demo_output.txt

echo "[3/3] Inspecting process hierarchy and /proc system..."
./process_demo 4 > /dev/null 2>&1 &
BG_PID=$!
sleep 0.2

if command -v ps > /dev/null 2>&1; then
    echo "--- PS Process Tree Snapshot ---" > evidence/ps_process_hierarchy.txt
    ps -ejH >> evidence/ps_process_hierarchy.txt 2>&1 || ps -ef | grep process_demo >> evidence/ps_process_hierarchy.txt 2>&1 || true
fi

if [ -d "/proc/$BG_PID" ]; then
    echo "--- /proc Status Snapshot for PID $BG_PID ---" > evidence/proc_status_inspection.txt
    cat "/proc/$BG_PID/status" | head -n 30 >> evidence/proc_status_inspection.txt 2>&1 || true
fi

wait $BG_PID 2>/dev/null || true
echo "Task 01 execution and evidence capture complete."
