# Task 2.1 & 2.2: Workload Analysis and File-Allocation Comparison

**Course:** CSA04 — Operating Systems | Assessment 4 (CO4)  
**Student:** S. Sachin | **Register Number:** 192525045  
**Topic:** Storage Architecture for a 32-Camera 30-Day Video-Surveillance Recorder (NVR)  

---

## 1. Workload Operations Profile (Task 2.1)
An enterprise Network Video Recorder (NVR) manages continuous incoming feeds from **32 HD security cameras**. The storage workload exhibits distinct operational characteristics:

| Operation | Access Pattern | Relative Frequency | System Impact & Technical Constraints |
| :--- | :--- | :--- | :--- |
| **File Creation** | Metadata Write | Low (32 files created per 1-hour or 24-hour recording segment) | Inode / directory entry allocation; timestamp initialization |
| **Sequential Writes** | Streaming Sequential | Extremely High (Continuous multi-megabit streaming 24/7/365) | Disk write buffer flushing; critical need for zero block-allocation latency |
| **File Growth** | Dynamic Extension | Continuous (Files grow steadily to 4 GB–10 GB per clip) | Allocation method must support unbounded expansion without pre-allocation failures |
| **Sequential Reads** | Streaming Sequential | Moderate (Routine security review / audit playback) | High read-ahead cache hits; minimal rotational seek latency |
| **Random / Direct Access** | Direct Seek by Timestamp | Low to Moderate (Incident investigation, scrubbing by minute) | Must translate `Timestamp -> Byte Offset -> Block Address` in $O(1)$ or minimal seeks |
| **File Deletion** | Bulk Metadata Update | Periodic (Purge oldest files once age exceeds 30 days) | Batch deallocation of large block extents; free-space reclamation |
| **Metadata Updates** | Inode Sync | Periodic (Updating file size, timestamps, sync flags) | Synchronous or journaled commits to ensure crash consistency |

---

## 2. Comparative Evaluation of File-Allocation Methods (Task 2.2)

| Criteria | Contiguous Allocation | Linked Allocation | FAT Allocation | Indexed / Extent Allocation |
| :--- | :--- | :--- | :--- | :--- |
| **Sequential Access Speed** | Outstanding (Zero seek overhead) | Good (Follows block pointers) | Good (FAT cached in RAM) | Excellent (Direct multi-block extents) |
| **Direct / Random Access** | Instantaneous: $O(1)$ arithmetic | Unusable: $O(N)$ block-by-block disk scan | $O(N)$ lookup in memory table | Fast: $O(1)$ tree traversal (1–3 reads) |
| **File Growth Support** | Severe failure (Pre-allocation required; cannot expand past neighbours) | Excellent (Append any free block) | Excellent (Append any free cluster) | Excellent (Allocate new index block or extents dynamically) |
| **External Fragmentation** | Catastrophic (Requires periodic defragmentation/compaction) | Zero external fragmentation | Zero external fragmentation | Zero external fragmentation |
| **Internal Fragmentation** | Low (< 1 block at end) | Low (< 1 block at end) | Moderate (Higher with large clusters) | Low (< 1 block + index blocks) |
| **Reliability / Fault Risk** | High (Loss of start/length loses file) | Extreme (Single bad pointer breaks entire file) | Critical single point of failure (FAT table loss) | High reliability (Hierarchical pointers; isolated extent maps) |
| **Metadata Overhead** | Minimal (Start block + length) | 4–8 bytes per data block (reduces payload) | 4 bytes per disk block in memory | 1–4 index blocks per multi-gigabyte file |

---

## 3. Justified Recommendation for Surveillance Storage
**Recommended Allocation Method: Extent-Based Indexed Allocation (as in XFS / ext4).**

### Justification:
1. **Dynamic, Continuous Expansion:** Video streams cannot predict their exact terminal size. Extent-based indexing allocates contiguous sequences of blocks (e.g. 1 MB–16 MB runs) dynamically without pre-allocation lockups or fragmentation.
2. **Deterministic Time-Scrubbing ($O(1)$ Direct Seeking):** Security operators searching for an incident at `14:32:15` require instant random seeking. Extent trees allow computing the exact target block in 1–2 memory lookups, whereas linked allocation requires reading millions of intermediate blocks from disk.
3. **High Write Throughput with Low Metadata Overhead:** Unlike basic indexed allocation that requires an indirect pointer per 4 KB block, extent allocation stores `(start_block, block_count)` tuples, reducing metadata by over 95% while keeping streaming I/O sequential.
