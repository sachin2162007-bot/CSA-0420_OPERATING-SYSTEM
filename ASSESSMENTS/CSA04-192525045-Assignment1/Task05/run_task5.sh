#!/usr/bin/env bash
set -euo pipefail

# Task 5 Execution and Evidence Generation Script
# Student: S. Sachin | Reg: 192525045 | Course: CSA04

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

mkdir -p evidence

echo "=== Compiling Task 5 Programs ==="
gcc -std=c11 -Wall -Wextra task5_syscall_demo.c -o task5_syscall_demo
gcc -std=c11 -Wall -Wextra copy_raw.c -o copy_raw
gcc -std=c11 -Wall -Wextra copy_stdio.c -o copy_stdio

echo "=== Generating 10,000-byte test file (records.dat) ==="
python3 -c "
with open('records.dat', 'w') as f:
    for i in range(10000):
        f.write(chr(ord('A') + (i % 26)))
" || head -c 10000 < /dev/zero | tr '\0' 'A' > records.dat

echo "=== Experiment 1: System Calls Demonstration Trace ==="
strace -o evidence/strace_syscall_demo.txt ./task5_syscall_demo

echo "=== Experiment 2: Raw POSIX Copy with 4096-byte buffer (strace -c) ==="
strace -c -o evidence/strace_c_raw_4096.txt ./copy_raw 4096 records.dat copy.dat

echo "=== Experiment 3: C Standard I/O Copy (strace -c) ==="
strace -c -o evidence/strace_c_stdio.txt ./copy_stdio records.dat copy.dat

# Cleanup temp files
rm -f records.dat copy.dat

echo "=== Task 5 Evidence Generation Complete ==="
ls -lh evidence/
