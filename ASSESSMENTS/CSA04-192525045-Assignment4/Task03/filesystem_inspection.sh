#!/usr/bin/env bash
# =====================================================================
# filesystem_inspection.sh - Safe Linux Filesystem Inspection Utility
# Course: CSA04 - Operating Systems | Student: S. Sachin (192525045)
# NON-DESTRUCTIVE READ-ONLY SCRIPT
# =====================================================================

set -euo pipefail

TARGET_PATH="${1:-.}"

echo "======================================================================"
echo " LINUX FILE SYSTEM METADATA & INODE INSPECTION UTILITY"
echo " Student: S. Sachin | Reg: 192525045 | SIMATS Engineering"
echo " Target Target Path: $TARGET_PATH"
echo "======================================================================"

echo -e "\n[1. FILE INODE & ATTRIBUTE INSPECTION (stat)]:"
stat "$TARGET_PATH"

echo -e "\n[2. INODE USAGE & CAPACITY ACROSS MOUNTED FILESYSTEMS (df -i)]:"
df -ih

echo -e "\n[3. DIRECTORY INODE INDEX LISTING (ls -li)]:"
ls -lid "$TARGET_PATH"

echo -e "\n[4. BLOCK SIZE & FILESYSTEM PARAMETERS]:"
if command -v tune2fs &> /dev/null; then
    ROOT_DEV=$(df "$TARGET_PATH" | tail -1 | awk '{print $1}')
    echo "Inspecting device: $ROOT_DEV (requires root privileges for raw superblock query)"
    sudo tune2fs -l "$ROOT_DEV" 2>/dev/null | grep -E "Block size|Inode size|Inode count|Free blocks|Free inodes" || \
        echo "Root permission required for tune2fs; running stat fallback..."
else
    echo "tune2fs not installed in host environment. Checking stat block size:"
    stat -f -c "Block Size: %s bytes | Total Blocks: %b | Free Blocks: %f" "$TARGET_PATH"
fi

echo -e "\n======================================================================"
echo " Inspection completed safely without modifying storage state."
echo "======================================================================"
