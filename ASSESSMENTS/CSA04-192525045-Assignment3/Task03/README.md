# Task 03: Segmentation Architecture and Memory Protection

**Course Code:** CSA04 — Operating Systems  
**Assignment:** Assessment 3 (CO3)  
**Student Name:** S. Sachin  
**Register Number:** 192525045  
**Bloom's Level:** K5 – Evaluate  
**Problem Context:** Student Programs with Separate Code, Stack and Data  

---

## 1. Task Title
**Evaluation of Segmentation, External Fragmentation, Hardware Address Translation, and SIGSEGV Handling.**

---

## 2. Problem Context
Student programs comprise distinct logical components: read-only code shared across all students compiling with GCC, dynamically growing stacks, and heap data buffers. Pure paging views memory as an undifferentiated stream of fixed-size blocks, ignoring these natural logical boundaries. Pure segmentation models memory after the programmer's cognitive view (variable-sized logical segments), but introduces severe external fragmentation.

---

## 3. Objective
To design, implement, and evaluate:
1. Hardware segment table translation using base and limit registers.
2. Out-of-bounds offset detection and segmentation fault traps.
3. Two-stage Segmented Paging (Intel x86 GDT/LDT + Paging MMU).
4. Real Linux kernel SIGSEGV trap inspection using `sigaction` and `/proc/<pid>/maps`.
5. 8-address manual translation verification.
6. Dynamic external fragmentation simulation across 20 segment allocation/free cycles.

---

## 4. Concepts Involved
- **Logical Address in Segmentation:** A 2-tuple `(s, d)` where $s$ is segment number and $d$ is offset.
- **Segment Table:** Array containing Base Address and Limit (Length) for each segment, accompanied by access permissions (read, write, execute).
- **Limit Check Trap:** Hardware verifies $d < \text{Limit}$. If $d \ge \text{Limit}$, an internal processor trap (Interrupt 13 on x86) triggers a kernel `SIGSEGV`.
- **External Fragmentation:** Free memory is broken into numerous small, disconnected fragments between allocated segments.
- **Compaction:** Moving all allocated segments to one end of physical memory to combine free holes, incurring severe memory bus stall overhead.
- **Segmented Paging:** Intel x86 hybrid where logical addresses map via segment base to a linear address, which is then translated by a paging unit to physical memory.

---

## 5. Files Created
- `segmentation_translation.c`: Segment table translation engine with limit verification.
- `segmentation_fault_demo.c`: Linux signal handler intercepting `SIGSEGV` and printing faulting address.
- `fragmentation_simulator.c`: Simulator modeling 20 dynamic segment operations and external fragmentation.
- `address_verification.c`: Test runner checking 8 hand-calculated translations.
- `run_experiments.sh`: Automated compilation and experiment execution script.
- `calculations.txt`: Complete hand-calculated address translation workings and proofs.
- `results.txt`: Comprehensive experimental findings and comparative matrices.

---

## 6. System Requirements
- Ubuntu Linux (x86_64)
- GCC C11 Compiler (`-std=c11 -Wall -Wextra`)
- POSIX Signal Handling APIs (`sigaction`, `sigsetjmp`, `siglongjmp`)

---

## 7. Compilation Commands
```bash
gcc -std=c11 -Wall -Wextra segmentation_translation.c -o segmentation_translation
gcc -std=c11 -Wall -Wextra segmentation_fault_demo.c -o segmentation_fault_demo
gcc -std=c11 -Wall -Wextra fragmentation_simulator.c -o fragmentation_simulator
gcc -std=c11 -Wall -Wextra address_verification.c -o address_verification
```

---

## 8. Execution Commands
```bash
./segmentation_translation
./address_verification
./segmentation_fault_demo
./fragmentation_simulator

# Or automated execution:
chmod +x run_experiments.sh
./run_experiments.sh
```

---

## 9. Expected Output (8 Address Translations)
```text
Test #1: Logical (0,  450) -> Actual: PHYSICAL  1850  | Expected: PHYSICAL  1850
Test #2: Logical (0, 1050) -> Actual: SEGMENT FAULT   | Expected: SEGMENT FAULT
Test #3: Logical (1,    0) -> Actual: PHYSICAL  6300  | Expected: PHYSICAL  6300
Test #4: Logical (1,  450) -> Actual: SEGMENT FAULT   | Expected: SEGMENT FAULT
Test #5: Logical (2,  250) -> Actual: PHYSICAL  4550  | Expected: PHYSICAL  4550
Test #6: Logical (3,  800) -> Actual: PHYSICAL  4000  | Expected: PHYSICAL  4000
Test #7: Logical (4,  999) -> Actual: PHYSICAL  5699  | Expected: PHYSICAL  5699
Test #8: Logical (5,  100) -> Actual: INVALID SEGMENT | Expected: INVALID SEGMENT
```

---

## 10. Illustrative Sample Output (SIGSEGV Handling)
```text
>>> [KERNEL TRAP INTERCEPTED] <<<
Signal Number:       11 (SIGSEGV - Segmentation Violation)
Faulting Address:    (nil)
Fault Reason Code:   1 (SEGV_MAPERR: Address not mapped to object)
OS Action:           Kernel MMU page fault handler trapped violation,
                     verified VMA limits in task_struct->mm, identified
                     illegal access, and dispatched SIGSEGV to process.
```

---

## 11. Calculations
- **Translation Formula:**
  $$\text{If } d < \text{Limit}[s] \implies \text{Physical Address} = \text{Base}[s] + d$$
  $$\text{If } d \ge \text{Limit}[s] \implies \text{Trap: Segmentation Fault}$$
- **External Fragmentation Ratio:**
  $$\text{External Fragmentation} = \text{Total Free Memory} - \text{Largest Contiguous Block}$$
  In our simulation: Total Free = 454 KB, Largest Block = 150 KB $\implies$ Ext Frag = 304 KB (67% of free memory unusable).

---

## 12. Experimental Investigation
1. **8 Hand Translations:** Verified that offset limit violations immediately trigger faults without touching memory.
2. **Real Linux SIGSEGV:** Successfully trapped invalid memory references and distinguished unmapped memory (`SEGV_MAPERR`) from write-permission violations on read-only code (`SEGV_ACCERR`).
3. **Fragmentation Simulation:** Confirms that after freeing alternating segments, allocations as small as 160 KB fail despite having 454 KB aggregate free RAM.

---

## 13. Evaluation: Comparison of Memory Architectures
| Metric | Pure Paging | Pure Segmentation | Segmented Paging |
| :--- | :--- | :--- | :--- |
| **Programmer View** | Linear address space | Logical modules | Logical modules |
| **External Fragmentation** | None (0%) | High (Frequent holes) | None (0%) |
| **Internal Fragmentation** | Low (< 1% on last page) | None (0%) | Low (< 1% on last page) |
| **Table Storage** | High (Multi-level trees) | Negligible (~50 bytes) | Moderate |
| **Compaction Need** | Never | Mandatory | Never |

---

## 14. Alternative Concept: Pure Paging
If the operating system utilizes pure paging without segmentation:
- **Solution:** Programs are divided into uniform 4 KB pages. The compiler and linker arrange code, data, and stack into page-aligned Virtual Memory Areas (VMAs).
- **Protection:** Enforced by setting `R/W/X` bits directly in the Page Table Entries (PTEs).
- **Advantage:** Complete elimination of external fragmentation and compaction overhead.

---

## 15. Time Complexity
- **Segment Translation:** $O(1)$ hardware addition and comparison ($\text{Base} + d$).
- **Memory Compaction:** $O(M)$ where $M$ is total allocated memory to copy during compaction.

---

## 16. Space Complexity
- **Segment Table:** $O(S)$ where $S$ is number of segments (typically $S \le 10$, $\approx 80\text{ bytes}$).
- **Page Table:** $O(P)$ where $P$ is allocated pages ($O(N)$ with virtual space).

---

## 17. Advantages & Limitations
- **Advantages:** Natural match to modular software architecture; simple fine-grained protection; zero internal fragmentation.
- **Limitations:** Severe external fragmentation; high compaction overhead; variable segment sizing complicates hardware allocation.

---

## 18. Conclusion
Modern operating systems rely primarily on **Paging (Flat Memory Model)** because external fragmentation and the associated cost of memory compaction make pure segmentation unviable in multi-programming environments. Linux preserves the conceptual modularity of segmentation through kernel software abstractions (`struct vm_area_struct`) while relying on fixed-size paging at the hardware level.

---

## 19. Image Placeholders
- `[INSERT IMAGE: segmentation_architecture.png]`
