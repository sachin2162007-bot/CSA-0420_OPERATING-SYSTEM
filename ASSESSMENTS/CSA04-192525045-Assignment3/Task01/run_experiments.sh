#!/usr/bin/env bash
# =====================================================================
# run_experiments.sh - Task 01 Automated Experiment Suite
# Student Name: S. Sachin | Register Number: 192525045
# Course: CSA04 - Operating Systems | Assessment 3 (CO3)
# =====================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "======================================================================"
echo " TASK 01: LINKING, LOADING AND ADDRESS BINDING EXPERIMENT RUNNER"
echo " Student: S. Sachin | Reg: 192525045 | CSA04 CO3"
echo "======================================================================"

# 1. Compilation
echo "[STEP 1] Compiling all libraries and executables via Makefile..."
make clean > /dev/null 2>&1 || true
make all

echo -e "\n[STEP 2] Inspecting Executable Sizes (Static vs Dynamic):"
ls -lh crypto_static crypto_shared crypto_runtime libcrypto_demo.so libcrypto_demo.a

echo -e "\n[STEP 3] Inspecting Dynamic Shared Dependencies (ldd):"
echo "--- ldd crypto_static (Expected: not a dynamic executable or minimal) ---"
ldd crypto_static || echo "Statically linked binary has no dynamic library dependencies."
echo -e "\n--- ldd crypto_shared (Expected: references libcrypto_demo.so and libc.so) ---"
ldd crypto_shared
echo -e "\n--- ldd crypto_runtime (Expected: references libdl.so / libc.so, NOT libcrypto) ---"
ldd crypto_runtime

echo -e "\n[STEP 4] Executing Static Binary:"
./crypto_static

echo -e "\n[STEP 5] Executing Shared Binary (Load-time Dynamic Linking):"
./crypto_shared

echo -e "\n[STEP 6] Executing Runtime Loading Binary (dlopen / dlsym):"
./crypto_runtime

echo -e "\n[STEP 7] Demonstrating Hot-Patching (Replacing Shared Library Without Recompiling Binary):"
echo "Current version in shared binary:"
./crypto_shared | grep "Version:"
echo "Applying security patch v2.0 to libcrypto_demo.so..."
make patch_v2
echo "Rerunning unchanged crypto_shared executable:"
./crypto_shared | grep "Version:"
echo "Rerunning unchanged crypto_runtime executable:"
./crypto_runtime | grep "Loaded Module Version:"

echo -e "\n[STEP 8] Demonstrating Missing Shared Library Behavior:"
echo "Temporarily renaming libcrypto_demo.so -> libcrypto_demo.so.bak"
mv libcrypto_demo.so libcrypto_demo.so.bak

echo "Testing crypto_shared (Expected: dynamic linker error / program termination):"
./crypto_shared || echo "-> Observed: Dynamic linker ld.so aborted process launch (library missing)."

echo "Testing crypto_runtime (Expected: graceful degradation / error handling):"
./crypto_runtime || echo "-> Observed: Handled missing library safely via dlerror() without crashing core."

echo "Restoring libcrypto_demo.so"
mv libcrypto_demo.so.bak libcrypto_demo.so

echo -e "\n[STEP 9] Running Quantitative Metrics Comparison:"
./static_dynamic_comparison

echo "======================================================================"
echo " Task 01 Experiment Suite completed successfully."
echo "======================================================================"
