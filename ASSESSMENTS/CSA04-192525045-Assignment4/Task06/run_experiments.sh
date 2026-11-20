#!/usr/bin/env bash
set -euo pipefail

echo "=================================================================="
echo " Task 06: Video Streaming Storage Design Experiments"
echo " Student: S. Sachin (192525045) - CSA04 Operating Systems"
echo "=================================================================="

make clean
make all

echo ""
echo "--- Running 25,000-Stream Architecture Simulator ---"
./storage_simulator

echo ""
echo "--- Running 100,000-Request Zipfian Workload Simulator ---"
./disk_workload_simulator

echo ""
echo "Task 06 experiments completed successfully!"
