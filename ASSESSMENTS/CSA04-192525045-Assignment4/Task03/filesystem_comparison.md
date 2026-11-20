# Task 3.1: Modern File Systems Comparison

**Course:** CSA04 — Operating Systems | Assessment 4 (CO4)  
**Student:** S. Sachin | **Register Number:** 192525045  
**Topic:** Architectural Evaluation of Modern Storage File Systems  

---

## 1. Comprehensive File Systems Comparison Matrix

| Feature / Metric | FAT32 | NTFS | ext4 | XFS | Btrfs | ZFS | APFS |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **Max File Size** | 4 GiB ($- 1$ byte) | 16 TiB (impl.) / 16 EiB (spec) | 16 TiB (with 4 KB block) | 8 EiB ($- 1$ byte) | 16 EiB | 16 EiB | 8 EiB |
| **Max Volume Size** | 2 TiB (32-bit sector) / 16 TiB | 16 TiB (impl.) / 8 PiB | 1 EiB | 8 EiB ($- 1$ byte) | 16 EiB | 256 ZiB (effectively infinite) | 8 EiB |
| **Journaling** | No | Yes (USN Journal) | Yes (JBD2 subsystem) | Yes (Active log) | No (CoW design) | No (CoW / ZIL Intent Log) | No (Crash protection / CoW) |
| **Snapshots** | No | Yes (VSS shadow copies) | No (Requires LVM) | No (External LVM) | Yes (Native subvolume CoW) | Yes (Native, instantaneous, $O(1)$) | Yes (Native point-in-time) |
| **Checksums (Data/Meta)** | None | Metadata only | Metadata only (CRC32c) | Metadata only (v5 superblock) | Both (CRC32c / xxHash / SHA256) | Both (Fletcher4 / SHA256 / Blake3) | Metadata only |
| **Copy-on-Write (CoW)** | No | No | No | Optional (`reflink` extents) | Yes (Default on all extents) | Yes (Default throughout pool) | Yes (Cloning / Redirect-on-Write) |
| **Typical Use Cases** | USB flash drives, EFI system partitions, legacy firmware | Windows OS system drive, corporate Windows servers | Default Linux desktop & server distribution root | High-performance enterprise Linux, multi-terabyte DBs | Linux NAS, openSUSE root, developer workstations | Enterprise SAN/NAS, multi-petabyte archival, critical DBs | Apple macOS, iOS, iPadOS system and user drives |
| **Main Strengths** | Universal compatibility across all platforms and embedded devices | Robust ACLs, file compression, encryption (EFS), mature | Rock-solid stability, low CPU/RAM footprint, broad driver support | Exceptional multi-threaded I/O parallelism (Allocation Groups) | Native RAID, instantaneous snapshots, transparent compression | Self-healing bit rot repair, integrated volume manager (zpool) | Optimized for flash/SSD, fast directory sizing, native encryption |
| **Main Limitations** | 4 GB file size limit; no journaling; extreme corruption risk on crash | Closed-source specification; degraded performance on Linux | Inability to shrink online; lacks native data checksums | Difficult to shrink volume; small random writes can fragment | Unstable RAID 5/6 write-hole; high CPU on write-intensive DBs | High RAM requirement (ARC cache); CDDL license prevents kernel tree merge | Proprietary Apple ecosystem only; poor performance on spinning HDDs |

*Note on Implementation Limits:* Maximum file and volume limits depend on kernel architecture (32-bit vs 64-bit), page size (4 KB vs 64 KB), and cluster configuration. Stated values reflect standard 64-bit Linux/Unix implementations.
