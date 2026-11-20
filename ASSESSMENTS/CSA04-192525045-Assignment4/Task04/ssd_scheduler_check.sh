#!/usr/bin/env bash
# ==============================================================================
# Script: ssd_scheduler_check.sh
# Purpose: Inspect Linux I/O scheduler configuration for block devices
# Author: S. Sachin (192525045) - CSA04 Operating Systems CO4
# ==============================================================================

set -euo pipefail

DEV="${1:-sda}"
SYS_PATH="/sys/block/${DEV}/queue/scheduler"

echo "================================================================="
echo " Linux I/O Scheduler Inspector"
echo " Target Block Device: /dev/${DEV}"
echo "================================================================="

if [ ! -f "$SYS_PATH" ]; then
    echo "Warning: ${SYS_PATH} does not exist on this environment."
    echo "Displaying illustrative output for common Linux environments:"
    echo ""
    echo "[ILLUSTRATIVE LINUX SCHEDULER INSPECTION OUTPUT]"
    echo "Device /dev/nvme0n1 (Fast NVMe SSD):"
    echo "  Available Schedulers: [none] mq-deadline"
    echo "  Active Scheduler    : none (Raw hardware multi-queue submission)"
    echo ""
    echo "Device /dev/sda (Rotational HDD / Virtual Disk):"
    echo "  Available Schedulers: mq-deadline [bfq] kyber none"
    echo "  Active Scheduler    : bfq (Budget Fair Queueing for low seek latency)"
    exit 0
fi

CURRENT_SCHED=$(cat "$SYS_PATH")
echo "Available Schedulers : ${CURRENT_SCHED}"
echo "Active Scheduler     : $(echo "$CURRENT_SCHED" | grep -o '\[.*\]' | tr -d '[]')"
echo ""
echo "Scheduler Analysis:"
case "$CURRENT_SCHED" in
    *"[none]"*)
        echo "  [none]: Multi-queue bypass enabled. Ideal for NVMe SSDs where hardware"
        echo "          parallelism handles queueing without CPU serialization overhead."
        ;;
    *"[mq-deadline]"*)
        echo "  [mq-deadline]: Deadline-based queueing with read priority."
        echo "          Good balance for enterprise SSDs and mixed random workloads."
        ;;
    *"[bfq]"*)
        echo "  [bfq]: Budget Fair Queueing enabled. Optimal for rotational HDDs to"
        echo "         prevent starvation and provide per-application bandwidth fairness."
        ;;
    *)
        echo "  Custom or virtualized scheduler active: ${CURRENT_SCHED}"
        ;;
esac
echo "================================================================="
