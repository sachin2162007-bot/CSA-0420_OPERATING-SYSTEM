# Task 2.3 & 2.4: Storage Cost, Inode Sizing, and Free-Space Calculations

**Course:** CSA04 — Operating Systems | Assessment 4 (CO4)  
**Student:** S. Sachin | **Register Number:** 192525045  
**Topic:** Mathematical Foundations of Inode Capacity, Block Traversal, and Free-Space Management  

---

## 1. 4 GB File Block Count Calculation (Task 2.3)

### Unit Convention
- In binary computational storage (IEC standard):  
  $$1\text{ GiB} = 1024\text{ MiB} = 1024 \times 1024\text{ KiB} = 1,048,576\text{ KiB} = 1,073,741,824\text{ Bytes}$$
  $$1\text{ Block} = 4\text{ KiB} = 4,096\text{ Bytes}$$
- Therefore, for a 4 GiB video recording file:
  $$\text{Number of Blocks} = \frac{4 \times 1024 \times 1024\text{ KiB}}{4\text{ KiB}} = 1,048,576\text{ Blocks} = 2^{20}\text{ Blocks}$$
*(Note: Under decimal metric standard $4\text{ GB} = 4 \times 10^9\text{ B}$, block count would be $1,000,000\text{ blocks}$. In modern operating systems, binary $2^{20} = 1,048,576$ blocks is the authoritative file-system allocation count).*

---

## 2. Disk Reads to Reach the Last Block (Block #1,048,575)

To read the final block of the 4 GB file:

1. **Contiguous Allocation:**
   $$\text{Disk Reads} = 1\text{ read}$$
   *Derivation:* The inode or directory stores `start_block`. Target block is computed algebraically: $\text{Physical Block} = \text{start\_block} + 1,048,575$. The disk head seeks directly to this block.

2. **Linked Allocation:**
   $$\text{Disk Reads} = 1,048,576\text{ reads}$$
   *Derivation:* Each data block stores a pointer to its successor. Reaching block $k$ requires reading blocks $0, 1, 2, \dots, k-1$ sequentially from physical storage. Accessing the final block incurs $1,048,576$ sequential disk I/O operations.

3. **FAT Allocation:**
   - **Case A (FAT resident in RAM - Standard Operating State):**  
     $$\text{Disk Reads} = 1\text{ read}$$  
     *Derivation:* The entire File Allocation Table is cached in system memory. The operating system traverses the chain in RAM in sub-microsecond software time, identifies the 1,048,576th block address, and issues exactly **1 physical disk read** for the data block.
   - **Case B (FAT Uncached on Disk):**  
     $$\text{Disk Reads} = 2\text{ reads}$$  
     *Derivation:* 1 disk read to fetch the relevant FAT sector containing the pointer, plus 1 disk read to fetch the target data block.

4. **Indexed Allocation (Standard Multi-Level Inode):**
   $$\text{Disk Reads} = 4\text{ reads (assuming Inode in RAM)}$$
   *Derivation:*
   - Block $1,048,575$ resides beyond direct (12), single-indirect (1,024), and double-indirect ($1,048,576$) ranges.
   - It is accessed via the **Triple-Indirect pointer**:
     1. Read Disk: Triple-indirect root block $\to$ yields double-indirect pointer.
     2. Read Disk: Double-indirect block $\to$ yields single-indirect pointer.
     3. Read Disk: Single-indirect block $\to$ yields data block address.
     4. Read Disk: Actual 4 KB Video Data Block.
   - Total Disk I/O Operations = **4 physical reads**.

---

## 3. UNIX Inode Maximum File Size Derivation

### Given Parameters:
- Inode structure: 12 Direct pointers, 1 Single-indirect, 1 Double-indirect, 1 Triple-indirect pointer.
- Block Size: $4\text{ KB} = 4,096\text{ bytes}$.
- Pointer Size (Block Address): $4\text{ bytes}$.

### Step-by-Step Calculation:
1. **Pointers per Indirect Block ($N$):**
   $$N = \frac{\text{Block Size}}{\text{Pointer Size}} = \frac{4,096\text{ bytes}}{4\text{ bytes}} = 1,024\text{ pointers} = 2^{10}$$

2. **Capacity Breakdown:**
   - **Direct Pointers (12):**  
     $$12 \text{ blocks} = 12 \times 4\text{ KB} = 48\text{ KB}$$
   - **Single-Indirect Pointer ($1 \times N$):**  
     $$1,024 \text{ blocks} = 1,024 \times 4\text{ KB} = 4,096\text{ KB} = 4\text{ MB}$$
   - **Double-Indirect Pointer ($1 \times N^2$):**  
     $$1,024^2 = 1,048,576 \text{ blocks} = 1,048,576 \times 4\text{ KB} = 4,194,304\text{ KB} = 4\text{ GB} = 4\text{ GiB}$$
   - **Triple-Indirect Pointer ($1 \times N^3$):**  
     $$1,024^3 = 1,073,741,824 \text{ blocks} = 1,073,741,824 \times 4\text{ KB} = 4,294,967,296\text{ KB} = 4\text{ TB} = 4\text{ TiB}$$

3. **Maximum Data Blocks Supported:**
   $$\text{Total Blocks} = 12 + N + N^2 + N^3 = 12 + 1,024 + 1,048,576 + 1,073,741,824 = \mathbf{1,074,791,436\text{ blocks}}$$

4. **Maximum File Size:**
   $$\text{Max Size} = 1,074,791,436 \times 4,096\text{ bytes} = 4,402,345,721,856\text{ Bytes}$$
   $$\text{Max Size in Binary Units} = \frac{4,402,345,721,856}{1024^4} = \mathbf{4.0039\text{ TiB} \approx 4\text{ TiB}}$$
   $$\text{Max Size in Decimal Units} = \frac{4,402,345,721,856}{10^{12}} = \mathbf{4.4023\text{ TB}}$$

---

## 4. Free-Space Management: 4 TB Storage Volume (Task 2.4)

### Given Parameters:
- Total Disk Capacity = $4\text{ TB} = 4 \times 1024^4\text{ bytes} = 4,398,046,511,104\text{ bytes}$.
- Block Size = $4\text{ KB} = 4,096\text{ bytes}$.

### Total Disk Blocks:
$$\text{Total Blocks} = \frac{4 \times 1024^4\text{ bytes}}{4 \times 1024\text{ bytes}} = 1024^3 = 2^{30}\text{ blocks} = 1,073,741,824\text{ blocks}$$

### Bit-Vector Size Calculation:
- A bit vector requires exactly 1 bit per disk block:
  $$\text{Number of Bits} = 1,073,741,824\text{ bits}$$
- Converting bits into bytes:
  $$\text{Bit-Vector Bytes} = \frac{1,073,741,824\text{ bits}}{8\text{ bits/byte}} = 134,217,728\text{ bytes}$$
- Converting into Megabytes:
  $$\text{Bit-Vector Size} = \frac{134,217,728\text{ bytes}}{1024 \times 1024} = \mathbf{128\text{ MB} = 128\text{ MiB}}$$

### Critical Evaluation of Free-Space Algorithms:
1. **Bit Vector:** Consumes a fixed **128 MB** RAM footprint. Easily kept in system memory; CPU bitwise scan instructions (`bsfq`, `popcnt`) locate contiguous runs in nanoseconds. Ideal for high-throughput video recording.
2. **Linked Free List:** If 50% of the drive is free ($536,870,912\text{ blocks}$), a 4-byte pointer per block requires **2 GB of RAM/disk**, with severe rotational seek overhead to allocate multi-block files.
3. **Grouping:** Stores 1,023 free block addresses per physical block. Reduces I/O by $1,023\times$ compared to linked lists.
4. **Counting:** Stores `(start_block, count)` extents. Highly compact for video streaming where contiguous chunks of 64–256 blocks are frequently allocated and released together.

---

## 5. Critical Inconsistency Analysis in the Official Question PDF

### The Stated Contradiction:
Page 35 of the official assignment PDF asks:
> *"Explain why file E (10 blocks) cannot be stored contiguously although 21 blocks are free, and how indexed allocation solves the problem."*

However, the official expected output on the exact same page displays:
```text
AAAAAA........CCCCC.............
E (10 blocks) contiguous: start 19
E (10 blocks) indexed: index block 6, data blocks 7 8 9 10 11 12 13 29 30 31
AAAAAAeEEEEEEECCCCCEEEEEEEEEEEEE
```

### Detailed Mathematical & Structural Explanation:
1. **Disk State Analysis:**
   - Disk length = 32 blocks (0 to 31).
   - After allocating A (6 blocks: 0–5), B (8 blocks: 6–13), C (5 blocks: 14–18), D (7 blocks: 19–25), and releasing B and D:
     - Blocks 0–5: `AAAAAA` (6 blocks used)
     - Blocks 6–13: `........` (**8 free blocks**)
     - Blocks 14–18: `CCCCC` (5 blocks used)
     - Blocks 19–25: `.......` (**7 free blocks**)
     - Blocks 26–31: `......` (**6 unallocated free blocks**)
2. **True Free Block Distribution:**
   - Free Hole 1: Blocks 6–13 $\to$ Length = 8 blocks.
   - Free Hole 2: Blocks 19–31 $\to$ Length = **13 blocks** ($7 + 6 = 13$).
   - Total Free Blocks = $8 + 13 = \mathbf{21\text{ free blocks}}$.
3. **Why Contiguous Allocation Actually Succeeds:**
   - First-fit scans for a run of 10 consecutive '.' blocks.
   - At block 6: Length is 8 ($8 < 10$), so it skips.
   - At block 19: Length is 13 ($13 \ge 10$), so it **successfully allocates blocks 19 to 28** to file E!
   - This returns start block **19**, matching the official program output line: `E (10 blocks) contiguous: start 19`.
4. **Pedagogical Resolution:**
   - The question author intended to illustrate external fragmentation where total free blocks ($21$) exceed request size ($10$), but no single hole is large enough (e.g. if blocks 26–31 had been pre-allocated, leaving holes of size 8 and 7, neither of which can hold 10 blocks).
   - In accordance with academic integrity guidelines, **our implementation adheres strictly to the official starter code logic**, preserving the exact expected output, while fully explaining the theoretical premise of external fragmentation.
