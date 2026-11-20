# Task 3.3: Workload-Driven File System Recommendations

**Course:** CSA04 — Operating Systems | Assessment 4 (CO4)  
**Student:** S. Sachin | **Register Number:** 192525045  
**Topic:** File System Selection for Pen Drives, Database Servers, Laptops, and Backup Servers  

---

## 1. Device 1: USB Pen Drive (Cross-Platform Windows and Linux)
- **Primary Recommendation:** **exFAT (Extended File Allocation Table)**
- **Technical Justification:**
  - Modern cross-platform compatibility: Natively supported in modern Linux kernels (via `exfat` driver), Microsoft Windows (XP SP2+ through Windows 11), and macOS.
  - Eliminates the crippling 4 GiB single-file limit of legacy FAT32, enabling large ISO, VM, and video file transfers.
  - Lightweight flash-friendly design without heavy journaling overhead that causes write amplification and accelerates USB NAND flash wear.
- **Limitations:** Lacks POSIX permission bits, file ownership, and journaling protection.
- **Alternative Choice:** **FAT32** (for maximum compatibility with legacy cars, smart TVs, and embedded devices, accepting the 4 GB file cap).

---

## 2. Device 2: High-Performance Linux Database Server
- **Primary Recommendation:** **XFS**
- **Technical Justification:**
  - Exceptional multi-threaded parallel throughput powered by **Allocation Groups (AGs)**, allowing concurrent threads to allocate blocks simultaneously without locking a global table.
  - Efficient support for massive direct I/O (`O_DIRECT`), bypassing OS page caches to let enterprise database engines (PostgreSQL, MySQL, Oracle) manage their own buffer pools.
  - Delayed allocation (`alloc-on-flush`) prevents fragmentation during large bulk table inserts.
- **Limitations:** File-system volumes cannot be shrunk/downsized online.
- **Alternative Choice:** **ext4 with `data=ordered`** (highly stable, lower memory footprint, supports online resizing).

---

## 3. Device 3: Standard General-Purpose Laptop
- **Primary Recommendation:** **ext4**
- **Technical Justification:**
  - Industry benchmark for stability, maturity, and broad hardware compatibility across all Linux distributions.
  - Extremely light CPU and RAM resource footprint, maximizing laptop battery runtime.
  - Excellent performance on modern NVMe and SATA SSDs with TRIM support.
- **Limitations:** Lacks native subvolume snapshots and data checksumming.
- **Alternative Choice:** **Btrfs** (ideal if automated system-rollback snapshots via Snapper / Timeshift are desired prior to OS updates).

---

## 4. Device 4: Enterprise Backup Server Requiring Snapshots
- **Primary Recommendation:** **ZFS (OpenZFS)**
- **Technical Justification:**
  - **Instantaneous $O(1)$ CoW Snapshots:** Creates thousands of point-in-time read-only snapshots without freezing production backups or consuming initial storage.
  - **End-to-End Cryptographic Checksumming:** Every block is verified with Fletcher4 or SHA256; automatically detects and self-heals silent bit rot using parity drives.
  - **Integrated Volume Management (zpool):** Eliminates separate LVM layers and provides native RAID-Z, transparent zstd compression, and incremental network replication (`zfs send | zfs receive`).
- **Limitations:** High memory requirements (requires 1 GB RAM per 1 TB storage for adaptive replacement cache ARC); CDDL licensing prevents out-of-the-box in-tree Linux kernel distribution.
- **Alternative Choice:** **Btrfs** (built directly into mainline Linux kernel, supporting subvolume snapshots and multi-device pools).
