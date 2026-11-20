# Task 1: Algorithmic Directory Search Complexity Analysis

**Course:** CSA04 — Operating Systems | Assessment 4 (CO4)  
**Student:** S. Sachin | **Register Number:** 192525045  
**Topic:** Time and Space Complexity Comparison of Flat vs Hierarchical Directory Lookups  

---

## 1. Flat Directory Search Complexity

### Mathematical Formulation
Let:
- $S = 12,000$ (Number of enrolled students).
- $F = 50$ (Average files created per student).
- Total directory entries: $N = S \times F = 600,000$ entries.

In a flat linear directory structure, directory entries are stored sequentially in a table or linked list of directory data blocks:

$$\text{Search Time Complexity (Linear Scan)}:$$
- **Best Case:** $O(1)$ (File found at first entry).
- **Average Case:** $O(N) = \frac{N}{2} \text{ comparisons} = 300,000\text{ block record inspections}$.
- **Worst Case (File not found / Last entry):** $O(N) = 600,000\text{ inspections}$.
- **Space Complexity:** $O(N)$ contiguous directory table memory.

### Physical Disk I/O Impact
If each directory entry requires 64 bytes:
$$\text{Directory Size} = 600,000 \times 64\text{ B} = 38,400,000\text{ B} \approx 36.62\text{ MB}$$
At 4 KB block size:
$$\text{Directory Data Blocks} = \frac{36.62\text{ MB}}{4\text{ KB}} = 9,375\text{ blocks}$$
Every uncached file lookup or insertion requires reading up to **9,375 disk blocks**, resulting in catastrophic latency on magnetic storage (~90 seconds for linear disk seeks).

---

## 2. Hierarchical Directory Lookup Complexity

### Mathematical Formulation
In a hierarchical tree of depth $D$, directory lookups proceed path segment by path segment:
$$\text{Path} = /\text{college}/\text{departments}/\text{aiml}/\text{batch\_2023}/\text{semester\_4}/\text{students}/192525045\_\text{sachin}/\text{assignments}/\text{csa04\_os}/\text{task1.md}$$

Here, depth $D = 8$ levels.
Let $M_k$ be the number of entries in the directory at level $k$:
- Level 1 (`/college`): 3 entries.
- Level 2 (`departments`): 10 departments.
- Level 3 (`aiml`): 4 batches.
- Level 4 (`batch_2023`): 2 semesters.
- Level 5 (`semester_4`): 2 subdirectories (`courses`, `students`).
- Level 6 (`students`): 120 students in that specific branch/year.
- Level 7 (`192525045_sachin`): 3 folders (`assignments`, `projects`, `documents`).
- Level 8 (`assignments/csa04_os`): ~10 files.

### Search Time Complexity
Using linear scan within each bounded subdirectory:
$$\text{Total Comparisons} = \sum_{k=1}^{D} M_k \le 3 + 10 + 4 + 2 + 2 + 120 + 3 + 10 = 154\text{ comparisons}$$
Compare this with the flat directory:
$$\text{Speedup Factor} = \frac{300,000}{154} \approx \mathbf{1,948\times\text{ faster!}}$$

### Modern Indexing Complexity (B-Tree / HTree)
Modern file systems (such as ext4 with `dir_index` or XFS) index subdirectories using $B^+$-trees or hashed trees:
- Lookup per path component: $O(\log M_k)$ or $O(1)$ hash lookup.
- Total Hierarchical Lookup Complexity:
  $$T_{\text{hierarchical}} = O(D \cdot \log M) \approx O(D)$$
  Where $D$ is constant path depth ($D \ll N$).
- **Space Complexity:** $O(N)$ distributed across localized inode metadata blocks, allowing efficient LRU page caching of frequently traversed path components.
