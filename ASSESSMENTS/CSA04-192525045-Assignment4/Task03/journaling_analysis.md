# Task 3.2: Journaling, Consistency, and Crash Recovery

**Course:** CSA04 — Operating Systems | Assessment 4 (CO4)  
**Student:** S. Sachin | **Register Number:** 192525045  
**Topic:** Failure Mechanics and Comparative Analysis of ext4 Journaling Modes  

---

## 1. File-System Failure Mechanics During Power Loss
When an operating system writes data to an existing file, several discrete storage mutations must occur:
1. **Data Blocks:** Payload bytes are written to physical disk blocks.
2. **Inode Allocation:** Inode modification timestamps (`mtime`), file size, and extent pointers are updated.
3. **Free-Space Allocation Bitmaps:** Block and inode allocation bitmaps are modified to mark newly consumed sectors as in-use.

If sudden power loss or kernel panic strikes mid-write, the storage volume enters an **inconsistent state**:
- *Torn Write:* Partial data blocks are committed without inode pointers.
- *Orphaned Blocks:* Free-space bitmaps mark blocks as allocated, but no directory entry or inode references them (leaked disk space).
- *Dangling Pointers:* An inode points to a block that was never flushed or was allocated to a different file, causing silent cross-file data corruption.

Without journaling or Copy-on-Write, recovering consistency requires running a full-disk scan (`fsck`). On multi-terabyte volumes, `fsck` must inspect billions of metadata records, halting system boot for hours.

---

## 2. Consistency Architectures: Journaling vs. Copy-on-Write (CoW)

### 2.1 Write-Ahead Logging (Journaling)
- Operates via a dedicated circular on-disk log (the journal).
- Before modifying the actual filesystem structures, the operating system writes a summary of the pending transaction into the journal.
- Once the journal transaction commits, the changes are applied to the primary filesystem locations (checkpointing).
- **Crash Recovery:** On reboot, the kernel inspects the journal. If an uncheckpointed committed transaction is found, it replays the logged blocks into place in a fraction of a second, restoring metadata consistency without scanning the disk.

### 2.2 Copy-on-Write (CoW)
- Adopted by ZFS and Btrfs.
- When existing data or metadata is modified, the new blocks are written to **previously free physical sectors**.
- The parent metadata pointers are updated to reference the new sectors, propagating up an acyclic tree to the root pointer.
- The new root pointer is committed atomically in a single disk write.
- **Crash Recovery:** If power fails during the write, the root pointer still references the older consistent version. The partially written new blocks are simply marked free during garbage collection. Zero `fsck` or replay is needed.

---

## 3. Comparison of ext4 Journaling Modes

The Linux `ext4` filesystem implements the Journaling Block Device (JBD2) layer, configurable via the `data=` mount option:

```text
mount -o data=ordered /dev/sda1 /mnt/data
```

| Journaling Mode | Operational Mechanism | Crash Guarantee | Performance & I/O Overhead |
| :--- | :--- | :--- | :--- |
| **`data=journal`** | **Full Data & Metadata Journaling:** Both user data payload and file-system metadata are written to the journal before writing to the primary filesystem. | **Highest Guarantee:** Zero metadata or user payload corruption; transactions roll back cleanly on crash. | **Lowest Performance:** Every byte is written twice (double-write penalty), halving sequential write throughput. |
| **`data=ordered`** *(Default)* | **Ordered Metadata Journaling:** User data payload is flushed directly to its final storage blocks *before* the corresponding metadata commit transaction is written to the journal. | **Balanced Guarantee:** Metadata consistency guaranteed; prevents stale/garbage data from appearing in expanded files after crash. | **Optimal Performance:** Minimal write amplification; user data written once, metadata journaled. |
| **`data=writeback`** | **Relaxed Metadata Journaling:** Only metadata changes are logged. User data payload blocks can be written before, during, or after metadata is committed to the journal. | **Weakest Guarantee:** Metadata remains consistent, but files may contain old deleted data or null bytes after an abrupt reboot. | **Maximum Speed:** Highest write throughput; uninhibited asynchronous dirty page flushing. |
