#!/usr/bin/env bash
# =====================================================================
# disk_benchmark.sh - Safe Non-Destructive Storage Read Speed Benchmark
# Student Name: S. Sachin | Register Number: 192525045
# Course: CSA04 - Operating Systems | Assessment 3 (CO3)
# =====================================================================

set -euo pipefail

TMP_BENCH_FILE="/tmp/os_bench_test_file.dat"
BLOCK_COUNT=64
BLOCK_SIZE="1M" # 64 MB test file

echo "======================================================================"
echo " SAFE DISK READ SPEED BENCHMARK (NON-DESTRUCTIVE)"
echo " Student: S. Sachin | Reg: 192525045 | CSA04 CO3"
echo "======================================================================"

# Cleanup handler on exit
trap 'rm -f "$TMP_BENCH_FILE"' EXIT

echo "[STEP 1] Generating temporary safe 64 MB benchmark file in /tmp..."
if command -v dd &> /dev/null; then
    dd if=/dev/zero of="$TMP_BENCH_FILE" bs="$BLOCK_SIZE" count="$BLOCK_COUNT" status=none
    echo "File generated: $TMP_BENCH_FILE (64 MB)"

    echo -e "\n[STEP 2] Measuring Buffered Cache Read Speed (RAM Cache):"
    dd if="$TMP_BENCH_FILE" of=/dev/null bs="$BLOCK_SIZE" count="$BLOCK_COUNT" status=progress

    echo -e "\n[STEP 3] Measuring Direct/Synchronous Read Speed (Bypassing OS Cache):"
    dd if="$TMP_BENCH_FILE" of=/dev/null bs="$BLOCK_SIZE" count="$BLOCK_COUNT" iflag=direct status=progress 2>/dev/null || \
        echo "Direct I/O flag not supported on current filesystem; standard read verified."
else
    echo "dd command unavailable. Running illustrative Python fallback..."
    python3 -c "
import time, os
data = b'0' * (64 * 1024 * 1024)
p = 'bench_temp.dat'
open(p, 'wb').write(data)
t0 = time.time()
r = open(p, 'rb').read()
t1 = time.time()
os.remove(p)
speed_mb = 64.0 / (t1 - t0)
print(f'Safe Python Stream Read Speed: {speed_mb:.1f} MB/s')
"
fi

echo -e "\n[STEP 4] Storage Performance Comparison Context:"
echo "  Standard Mechanical HDD:  120 MB/s to 180 MB/s (10 ms seek latency)"
echo "  SATA III SSD:             500 MB/s to 550 MB/s (0.1 ms access latency)"
echo "  NVMe PCIe Gen 4 SSD:      5,000 MB/s to 7,000 MB/s (0.02 ms access latency)"

echo "======================================================================"
echo " Disk benchmark complete. Temporary files cleanly removed."
echo "======================================================================"
