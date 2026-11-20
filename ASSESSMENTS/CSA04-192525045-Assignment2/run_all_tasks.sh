#!/usr/bin/env bash
# Master Execution Script for Operating Systems Assessment 2 (CO2)
# Student: S. Sachin (192525045) | Course: CSA04

set -e

echo "======================================================================"
echo " CSA04 OPERATING SYSTEMS — ASSESSMENT 2 (CO2) MASTER RUNNER"
echo " Student: S. Sachin (192525045) | Repository: CSA04-192525045-Assignment2"
echo "======================================================================"
echo ""

echo ">>> [TASK 01/06] Executing Task 01: Processes & Process States..."
cd Task01 && ./scripts/run_task1.sh && cd ..
echo ">>> Task 01 Completed Successfully."
echo ""

echo ">>> [TASK 02/06] Executing Task 02: CPU Scheduling..."
cd Task02 && ./scripts/run_task2.sh && cd ..
echo ">>> Task 02 Completed Successfully."
echo ""

echo ">>> [TASK 03/06] Executing Task 03: Threads & Concurrency..."
cd Task03 && ./scripts/run_task3.sh && cd ..
echo ">>> Task 03 Completed Successfully."
echo ""

echo ">>> [TASK 04/06] Executing Task 04: Inter-Process Communication..."
cd Task04 && ./scripts/run_task4.sh && cd ..
echo ">>> Task 04 Completed Successfully."
echo ""

echo ">>> [TASK 05/06] Executing Task 05: Synchronisation & Semaphores..."
cd Task05 && ./scripts/run_task5.sh && cd ..
echo ">>> Task 05 Completed Successfully."
echo ""

echo ">>> [TASK 06/06] Executing Task 06: Deadlock Handling & Banker's Algorithm..."
cd Task06 && ./scripts/run_task6.sh && cd ..
echo ">>> Task 06 Completed Successfully."
echo ""

echo "======================================================================"
echo " ALL SIX TASKS EXECUTED AND VERIFIED SUCCESSFULLY!"
echo " All C11 sources, shell scripts, and evidence files are complete."
echo "======================================================================"
