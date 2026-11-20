#!/usr/bin/env bash
# =====================================================================
# disk_information.sh - Linux Disk and Swap Partition Inspector
# Student Name: S. Sachin | Register Number: 192525045
# Course: CSA04 - Operating Systems | Assessment 3 (CO3)
# NON-DESTRUCTIVE INSPECTION SCRIPT
# =====================================================================

set -euo pipefail

echo "======================================================================"
echo " LINUX DISK PARTITION AND SWAP INSPECTION UTILITY"
echo " Student: S. Sachin | Reg: 192525045 | CSA04 CO3"
echo "======================================================================"

echo -e "\n[1. BLOCK DEVICES & MOUNT POINTS (lsblk)]:"
if command -v lsblk &> /dev/null; then
    lsblk -o NAME,SIZE,TYPE,FSTYPE,MOUNTPOINT
else
    echo "lsblk not available on this platform. (Simulated output in results.txt)"
fi

echo -e "\n[2. ACTIVE SWAP PARTITIONS / FILES (swapon --show)]:"
if command -v swapon &> /dev/null; then
    swapon --show || echo "No active swap partitions detected."
else
    echo "swapon utility not present in current environment."
fi

echo -e "\n[3. VIRTUAL FILESYSTEM SWAP METRICS (/proc/swaps)]:"
if [ -f /proc/swaps ]; then
    cat /proc/swaps
else
    echo "/proc/swaps not found."
fi

echo -e "\n[4. DISK PARTITION TABLE SUMMARY (fdisk -l / parted)]:"
echo "(Note: sudo privileges required on Linux for physical fdisk query)"
if command -v fdisk &> /dev/null; then
    fdisk -l 2>/dev/null || echo "Run as root to inspect physical disk partition tables."
else
    echo "fdisk command not installed."
fi

echo "======================================================================"
echo " Disk information query complete."
echo "======================================================================"
