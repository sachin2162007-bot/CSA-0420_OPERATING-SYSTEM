# Task 03: Modern File Systems Report

**Course**: CSA04 – Operating Systems (CO4)  
**Student**: S. Sachin (192525045)  
**Marks Allocated**: 9 Marks  

## 1. Modern Filesystem Comparative Analysis
Comprehensive evaluation of 7 modern filesystems: FAT32, NTFS, ext4, Btrfs, XFS, ZFS, and F2FS across maximum file sizes, volume capacities, data protection mechanisms, and hardware affinities.

## 2. Linux Journaling Modes Deep-Dive
- `data=journal`: Full data and metadata journaling; highest crash durability; 50% write penalty.
- `data=ordered`: Metadata journaled after flushing data; prevents stale data exposure; optimal balance.
- `data=writeback`: Metadata-only journaling; maximum write speed; risk of stale data on crash.

## 3. Workload-Specific Filesystem Recommendations
- High-Performance Database Server: **XFS** with Allocation Groups and Direct I/O (`O_DIRECT`).
- Mobile Smartphone Flash Storage: **F2FS** (Flash-Friendly File System) minimizing write amplification.
- Mission-Critical Enterprise Storage Pool: **OpenZFS** with 128-bit Merkle tree checksums and self-healing CoW.
