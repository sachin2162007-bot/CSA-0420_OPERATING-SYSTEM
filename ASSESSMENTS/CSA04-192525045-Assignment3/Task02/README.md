# Task 02: Paging Architecture and Memory Management

**Course Code:** CSA04 — Operating Systems  
**Assignment:** Assessment 3 (CO3)  
**Student Name:** S. Sachin  
**Register Number:** 192525045  
**Bloom's Level:** K5 – Evaluate  
**Problem Context:** Memory Allocation for a Multi-User College Server  

---

## 1. Task Title
**Evaluation of Paging, Multi-Level Page Tables, TLB Performance, and Fragmentation for Multi-User Systems.**

---

## 2. Problem Context
A shared college Linux server concurrently runs hundreds of heterogeneous student programming assignments ranging from trivial 2 KB scripts to 100 MB data science jobs. When physical memory was allocated using variable-size contiguous blocks, severe external fragmentation developed over time. Countless tiny, non-contiguous holes were scattered across RAM, causing large student jobs to fail loading despite ample aggregate free memory. Paging was introduced to decouple logical program blocks from physical placement.

---

## 3. Objective
To design, implement, and evaluate:
1. Logical to physical address translation with page table lookup (`paging_translation.c`).
2. Completion of official `TODO 1` and recalculation under altered page size (`PAGE_SIZE = 2048`).
3. Mathematical memory scaling of 32-bit vs 48-bit page tables.
4. Effective Access Time (EAT) across TLB hit ratios of 80%, 95%, and 99%.
5. Quantitative simulation of internal fragmentation across 100 processes for 1 KB, 4 KB, and 2 MB page sizes.
6. Boundary and edge-case testing at page transitions and unmapped regions.

---

## 4. Concepts Involved
- **Paging:** Fixed-size memory allocation scheme dividing logical memory into pages and physical memory into frames.
- **External vs. Internal Fragmentation:** Paging completely eliminates external fragmentation by allowing non-contiguous frame allocation. However, internal fragmentation persists on the final page of each process (averaging $\frac{\text{Page Size}}{2}$).
- **Page Table Entry (PTE):** Holds frame number, valid/invalid (present) bit, read/write permissions, user/supervisor bit, dirty bit, and accessed bit.
- **Translation Lookaside Buffer (TLB):** Fast associative hardware cache storing recent virtual-to-physical translations to bypass slow DRAM page table walks.
- **Hierarchical Paging:** Multi-level tree (e.g. x86_64 PML4) avoiding huge contiguous page table storage.
- **Inverted Page Table (IPT):** Single global table indexed by physical frame number rather than process virtual address space.

---

## 5. Files Created
- `paging_translation.c`: Implementation of the official page-table exercise completing `TODO 1`.
- `page_table_analysis.c`: Analytical model for 32-bit, 48-bit multi-level, and inverted page table scaling.
- `tlb_calculator.c`: Effective Access Time simulator across hit ratios.
- `fragmentation_simulator.c`: Internal fragmentation benchmark for 100 processes across 1 KB, 4 KB, 2 MB page sizes.
- `address_boundary_test.c`: Edge-case tester for offset bounds, unmapped pages, and page fault triggers.
- `run_experiments.sh`: Automated bash script compiling and executing all Task 02 modules.
- `calculations.txt`: Complete mathematical workings and formulas.
- `results.txt`: Classified output verification file matching official expectations.

---

## 6. System Requirements
- GCC Compiler with C11 support (`-std=c11 -Wall -Wextra`)
- Standard POSIX libc runtime
- Bash shell (`getconf PAGESIZE`)

---

## 7. Compilation Commands
```bash
# Compile official translation with PAGE_SIZE = 1024
gcc -std=c11 -Wall -Wextra paging_translation.c -o paging_translation_1024

# Compile modified translation with PAGE_SIZE = 2048
gcc -std=c11 -Wall -Wextra -DPAGE_SIZE=2048 paging_translation.c -o paging_translation_2048

# Compile analysis and simulation tools
gcc -std=c11 -Wall -Wextra page_table_analysis.c -o page_table_analysis
gcc -std=c11 -Wall -Wextra tlb_calculator.c -o tlb_calculator
gcc -std=c11 -Wall -Wextra fragmentation_simulator.c -o fragmentation_simulator
gcc -std=c11 -Wall -Wextra address_boundary_test.c -o address_boundary_test
```

---

## 8. Execution Commands
```bash
./paging_translation_1024
./paging_translation_2048
./address_boundary_test
./page_table_analysis
./tlb_calculator
./fragmentation_simulator

# Or execute full automated test suite:
chmod +x run_experiments.sh
./run_experiments.sh
```

---

## 9. Expected Output (Official Problem)
```text
====================================================
 PAGING ADDRESS TRANSLATION (PAGE_SIZE = 1024)
 Student: S. Sachin | Reg: 192525045 | CSA04 CO3
====================================================
0 -> page 0 offset 0 : physical 5120
1500 -> page 1 offset 476 : physical 2524
3100 -> page 3 offset 28 : physical 7196
6200 -> page 6 offset 56 : physical 3128
9000 -> page 8 offset 808 : invalid address
2049 -> page 2 offset 1 : page fault
```

---

## 10. Illustrative Sample Output (PAGE_SIZE = 2048)
```text
0 -> page 0 offset 0 : physical 10240
1500 -> page 0 offset 1500 : physical 11740
3100 -> page 1 offset 1052 : physical 5148
6200 -> page 3 offset 56 : physical 14392
9000 -> page 4 offset 808 : physical 808
2049 -> page 1 offset 1 : physical 4097
```

---

## 11. Calculations
- **Address Translation:**
  $$p = \lfloor \frac{\text{Logical Address}}{\text{Page Size}} \rfloor, \quad d = \text{Logical Address} \pmod{\text{Page Size}}$$
  $$\text{Physical Address} = (\text{Frame Number} \times \text{Page Size}) + d$$
- **Effective Access Time (EAT):**
  $$\text{EAT} = \alpha(t_{\text{tlb}} + t_{\text{mem}}) + (1-\alpha)(t_{\text{tlb}} + (L+1)t_{\text{mem}})$$
  Where $L = 4$ for 64-bit multi-level paging.
  - At $\alpha = 80\%$: $\text{EAT} = 190.0\text{ ns}$ ($2.63\times$ speedup)
  - At $\alpha = 95\%$: $\text{EAT} = 130.0\text{ ns}$ ($3.85\times$ speedup)
  - At $\alpha = 99\%$: $\text{EAT} = 114.0\text{ ns}$ ($4.39\times$ speedup)

---

## 12. Experimental Investigation
1. **Internal Fragmentation across 100 processes:**
   - 1 KB page size: 512 B/proc (0.21% overhead).
   - 4 KB page size: 2,048 B/proc (0.84% overhead).
   - 2 MB page size: 1.85 MB/proc (88.42% catastrophic overhead).
2. **Boundary Testing:** Addresses at offset 0, offset 1023, and transition points across mapped/unmapped frames confirm robust page fault trap semantics.
3. **Linux Kernel Query:** `getconf PAGESIZE` confirms standard Linux systems enforce 4,096 bytes (4 KB) base page size.

---

## 13. Evaluation of Page Sizes
| Feature | 1 KB Page Size | 4 KB Page Size | 2 MB Huge Page |
| :--- | :--- | :--- | :--- |
| **Internal Fragmentation** | Lowest (~512 B) | Moderate (~2 KB) | Extreme (~1 MB) |
| **Page Table Overhead** | Severe (4x larger tables) | Balanced (1 page = 512 PTEs) | Minimal (1 PTE per 2 MB) |
| **TLB Coverage** | 64 entries = 64 KB | 64 entries = 256 KB | 64 entries = 128 MB |
| **I/O Transfer Cost** | High seek overhead | Optimal disk block match | High initial paging latency |

---

## 14. Alternative Concept: Inverted Page Table (IPT)
Instead of allocating per-process page tables whose total size grows with virtual address space and process count, an Inverted Page Table maintains **exactly one entry per physical frame**:
- **Space:** Proportional to **physical RAM**, not virtual address space ($O(\text{Physical RAM})$). On a 16 GB server, the entire table consumes only 64 MB regardless of running 10 or 1,000 processes.
- **Time Complexity:** Direct lookup is $O(N_{\text{frames}})$. To ensure practical performance, a **Hash Anchor Table (HAT)** hashes `(PID, Page#)` to achieve average $O(1)$ translation time.
- **Limitation:** Sharing pages across processes is difficult because each frame entry maps to only one logical owner.

---

## 15. Time Complexity
- **Direct Paging with Hardware TLB Hit:** $O(1)$ associative lookup (~10 ns).
- **Multi-Level Page Walk Miss:** $O(L)$ sequential DRAM memory references where $L$ is page depth ($L=4$ in x86_64).
- **Inverted Page Table with Hash:** $O(1)$ expected, $O(F)$ worst-case hash collision chain.

---

## 16. Space Complexity
- **Flat Page Table:** $O(2^V)$ per process where $V$ is virtual address bits ($512\text{ GB}$ on 48-bit, hence infeasible).
- **Multi-Level Paging:** $O(A)$ proportional to actual allocated virtual address space ($A \ll 2^V$).
- **Inverted Page Table:** $O(M)$ proportional strictly to physical RAM size $M$.

---

## 17. Advantages
- Completely eliminates external fragmentation.
- Enables memory protection (read/write/execute bits) per page.
- Allows shared libraries to map to identical physical frames.

---

## 18. Limitations
- Suffers from internal fragmentation on last allocated page.
- Requires multi-level tables and TLB hardware to offset memory access latency penalties.

---

## 19. Conclusion & Design Recommendation
For the college server, the recommended configuration is **4 KB Page Size with 4-Level Hierarchical Paging and TLB**. This ensures memory wasted to internal fragmentation remains negligible (<1%) while avoiding the massive page-table bloat of 1 KB pages and the catastrophic 88% memory waste of 2 MB huge pages on small student processes.

---

## 20. Image Placeholders
- `[INSERT IMAGE: paging_address_translation.png]`
- `[INSERT IMAGE: external_fragmentation.png]`
