# Stage 5B: Computational Complexity of Video Streaming Storage Operations

## 1. Complexity Comparison Table

| Subsystem Operation | Traditional File System | ZFS ARC + L2ARC Cache | Object Store (S3 REST) |
| :--- | :---: | :---: | :---: |
| **Chunk Read (Cache Hit)** | $O(d)$ (Directory lookup) | $\mathbf{O(1)}$ (Hash table index) | $O(1)$ (Consistent hash) |
| **Cache Eviction (LRU/ARC)** | $O(1)$ (Doubly linked list) | $\mathbf{O(1)}$ (Dual-list MRU/MFU) | Cloud Managed |
| **Bit Rot Scrubbing** | $O(B)$ (Sequential disk read) | $O(B)$ (Background scrub) | Automated |
| **Disk Scheduling Overhead** | $O(N \log N)$ (Elevator sort) | $\mathbf{O(1)}$ (`none` on NVMe) | Managed |

---

## 2. Cache Replacement Complexity: Adaptive Replacement Cache (ARC)
ZFS ARC dynamically self-tunes between recency (LRU) and frequency (LFU) using four doubly linked lists:
- $L_1$: Recent items (split into top $T_1$ and bottom ghost list $B_1$).
- $L_2$: Frequent items (split into top $T_2$ and bottom ghost list $B_2$).
- **Lookup**: $O(1)$ via in-memory hash table.
- **Cache Hit / Promotion**: $O(1)$ pointer update.
- **Tuning Adjustment**: Parameter $p$ adapts continuously based on cache miss hits in $B_1$ vs $B_2$, providing optimal hit ratios without manual tuning.
