#!/usr/bin/env bash
set -euo pipefail

# =============================================================================
# SMART HOSPITAL OPERATING SYSTEMS ASSIGNMENT 1 (CO1)
# Master Automated Evidence Collection Runner
#
# Student: S. Sachin
# Register Number: 192525045
# Course: CSA04 - Operating Systems
# Assignment Directory: CSA04-192525045-Assignment1
# =============================================================================

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"

echo "============================================================================="
echo "  STARTING SMART HOSPITAL EVIDENCE GENERATION SUITE"
echo "  Student: S. Sachin | Register No: 192525045"
echo "  Date: $(date)"
echo "  System: $(uname -a)"
echo "============================================================================="

# 1. Environment Verification
echo ""
echo "[-] Verifying Required Tools..."
for tool in gcc strace /usr/bin/time python3; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        echo "[ERROR] Required tool '$tool' is not installed or not in PATH."
        exit 1
    fi
    echo "  [OK] Found $tool: $(command -v "$tool")"
done

gcc --version | head -n 1
strace --version | head -n 1

# 2. Execute Task 05
echo ""
echo "============================================================================="
echo "[+] EXECUTING TASK 05: SYSTEM CALLS"
echo "============================================================================="
chmod +x Task05/run_task5.sh
(cd Task05 && ./run_task5.sh)

# 3. Execute Task 06
echo ""
echo "============================================================================="
echo "[+] EXECUTING TASK 06: BUFFER SIZE EXPERIMENT"
echo "============================================================================="
chmod +x Task06/run_task6.sh
(cd Task06 && ./run_task6.sh)

# 4. Execute Task 07
echo ""
echo "============================================================================="
echo "[+] EXECUTING TASK 07: PROCESS CONTROL SYSTEM CALLS"
echo "============================================================================="
chmod +x Task07/run_task7.sh
(cd Task07 && ./run_task7.sh)

# 5. Execute Task 10
echo ""
echo "============================================================================="
echo "[+] EXECUTING TASK 10: PROCESS & THREAD PERFORMANCE MEASUREMENT"
echo "============================================================================="
chmod +x Task10/run_task10.sh
(cd Task10 && ./run_task10.sh)

# 6. Verification of Generated Files
echo ""
echo "============================================================================="
echo "[*] VALIDATING GENERATED EVIDENCE FILES"
echo "============================================================================="

REQUIRED_FILES=(
    "Task05/evidence/strace_syscall_demo.txt"
    "Task05/evidence/strace_c_raw_4096.txt"
    "Task05/evidence/strace_c_stdio.txt"
    "Task06/evidence/task6_output.txt"
    "Task06/evidence/task6_strace_c.txt"
    "Task06/evidence/copy_raw_times.txt"
    "Task06/evidence/copy_stdio_time.txt"
    "Task06/evidence/copy_mmap_time.txt"
    "Task06/evidence/strace_c_raw_4096.txt"
    "Task06/evidence/strace_c_stdio.txt"
    "Task06/evidence/strace_c_mmap.txt"
    "Task07/evidence/task7_terminal_output.txt"
    "Task07/evidence/task7_exp_redirected.txt"
    "Task07/evidence/spawn_bench_1000.txt"
    "Task10/evidence/proc_vs_thread_run1-3.txt"
    "Task10/evidence/proc_vs_thread_time_v.txt"
)

ALL_PASSED=1
for file in "${REQUIRED_FILES[@]}"; do
    if [ -f "$file" ] && [ -s "$file" ]; then
        echo "  [PRESENT & NON-EMPTY] $file ($(wc -l < "$file") lines, $(wc -c < "$file") bytes)"
    else
        echo "  [MISSING OR EMPTY]    $file"
        ALL_PASSED=0
    fi
done

if [ "$ALL_PASSED" -eq 1 ]; then
    echo ""
    echo "============================================================================="
    echo "  ALL REQUIRED EVIDENCE FILES SUCCESSFULLY GENERATED AND VALIDATED!"
    echo "============================================================================="
else
    echo ""
    echo "[WARNING] One or more evidence files failed validation."
    exit 1
fi
