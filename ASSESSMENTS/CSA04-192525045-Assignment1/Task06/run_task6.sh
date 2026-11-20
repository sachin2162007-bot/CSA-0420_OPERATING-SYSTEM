#!/usr/bin/env bash
set -euo pipefail

# Task 6 Execution and Evidence Generation Script
# Student: S. Sachin | Reg: 192525045 | Course: CSA04

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

mkdir -p evidence

echo "=== Compiling Task 6 Programs ==="
gcc -std=c11 -Wall -Wextra task6.c -o task6
gcc -std=c11 -Wall -Wextra -O2 copy_raw_bench.c -o copy_raw_bench
gcc -std=c11 -Wall -Wextra -O2 copy_stdio_bench.c -o copy_stdio_bench
gcc -std=c11 -Wall -Wextra -O2 copy_mmap_bench.c -o copy_mmap_bench

echo "=== Running task6 (10,000-byte test file verification) ==="
./task6 > evidence/task6_output.txt

echo "=== Running task6 under strace -c ==="
strace -c -o evidence/task6_strace_c.txt ./task6

echo "=== Generating 10 MB (10,485,760 bytes) benchmark input file ==="
# Distinct benchmark file clearly recording size
BENCH_FILE="bench_records_10mb.dat"
python3 -c "
with open('$BENCH_FILE', 'wb') as f:
    block = b'A' * 65536
    for _ in range(160): # 160 * 64KB = 10MB
        f.write(block)
" || dd if=/dev/urandom of="$BENCH_FILE" bs=1M count=10 status=none

echo "=== Benchmarking raw read/write copy across buffer sizes ==="
> evidence/copy_raw_times.txt
echo "# Buffer Size Benchmark on $BENCH_FILE (Size: $(wc -c < "$BENCH_FILE") bytes)" >> evidence/copy_raw_times.txt
for b in 1 64 512 1024 4096 8192 16384 65536 1048576; do
    echo "Running buffer size: $b bytes..."
    ./copy_raw_bench "$b" "$BENCH_FILE" copy.dat >> evidence/copy_raw_times.txt
    rm -f copy.dat
done

echo "=== Benchmarking stdio copy ==="
./copy_stdio_bench "$BENCH_FILE" copy.dat > evidence/copy_stdio_time.txt
rm -f copy.dat

echo "=== Benchmarking mmap copy ==="
./copy_mmap_bench "$BENCH_FILE" copy.dat > evidence/copy_mmap_time.txt
rm -f copy.dat

echo "=== Generating strace evidence for 4096-byte raw copy ==="
strace -c -o evidence/strace_c_raw_4096.txt ./copy_raw_bench 4096 "$BENCH_FILE" copy.dat
rm -f copy.dat

echo "=== Generating strace evidence for stdio copy ==="
strace -c -o evidence/strace_c_stdio.txt ./copy_stdio_bench "$BENCH_FILE" copy.dat
rm -f copy.dat

echo "=== Generating strace evidence for mmap copy ==="
strace -c -o evidence/strace_c_mmap.txt ./copy_mmap_bench "$BENCH_FILE" copy.dat
rm -f copy.dat

rm -f "$BENCH_FILE"

echo "=== Task 6 Evidence Generation Complete ==="
ls -lh evidence/
