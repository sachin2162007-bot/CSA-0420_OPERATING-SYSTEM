#!/usr/bin/env bash
set -euo pipefail

echo "================================================================"
echo " Task 04: Disk Scheduling Experiments"
echo " Student: S. Sachin (192525045) - CSA04 Operating Systems"
echo "================================================================"

make clean
make all

echo ""
echo "--- Running Official Verification Program (disk_scheduling) ---"
./disk_scheduling

echo ""
echo "--- Running 10,000 Request Simulator (disk_scheduling_simulator) ---"
./disk_scheduling_simulator

echo ""
echo "--- Running Linux I/O Scheduler Check Script ---"
bash ssd_scheduler_check.sh sda

echo ""
echo "Task 04 experiments completed successfully!"
