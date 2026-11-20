# Task 3.4: Linux Filesystem Inspection Commands Reference

**Course:** CSA04 — Operating Systems | Assessment 4 (CO4)  
**Student:** S. Sachin | **Register Number:** 192525045  
**Topic:** Safe, Read-Only Inspection of Inodes, Block Sizes, and Superblock Metadata  

---

## 1. Core Inspection Commands

### 1.1 `stat <filepath>`
Inspects the low-level metadata of a target file or directory directly from its inode:
```bash
stat /home/sachin/Task03/filesystem_inspection.sh
```
- **Output Metrics:**
  - Inode number (`Inode: 14829103`)
  - File size in bytes (`Size: 1809`)
  - Number of 512-byte blocks allocated (`Blocks: 8`)
  - Optimal I/O transfer block size (`IO Block: 4096`)
  - Device major/minor numbers
  - Access, Modify, and Change timestamps (`atime`, `mtime`, `ctime`).

### 1.2 `df -i`
Displays inode consumption and availability across all mounted filesystems:
```bash
df -ih
```
- **Output Metrics:** Total inode table capacity, consumed inodes, free inodes, and percentage used. Essential for diagnosing systems that cannot create new files despite having gigabytes of free disk space (inode exhaustion).

### 1.3 `ls -i` / `ls -li`
Lists directory contents alongside their hardware inode index numbers:
```bash
ls -lai Task03/
```
- Proves whether two directory entries reference the same physical file via hard links (identical inode numbers).

### 1.4 `tune2fs -l <device>` & `dumpe2fs -h <device>`
Extracts superblock metadata parameters directly from ext4 filesystems (requires root privileges):
```bash
sudo tune2fs -l /dev/sda1 | grep -E "Block size|Inode size|Free blocks|Filesystem features"
```
- **Key Parameters:**
  - `Block size: 4096`: Confirms the standard 4 KB allocation unit.
  - `Inode size: 256`: Inode descriptor size in bytes.
  - `Filesystem features`: Lists enabled capabilities (e.g. `has_journal`, `extents`, `dir_index`, `metadata_csum`).
