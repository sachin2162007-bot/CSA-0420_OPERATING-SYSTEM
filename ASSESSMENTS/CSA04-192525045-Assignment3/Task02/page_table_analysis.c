/**
 * page_table_analysis.c - Multi-Level Page Table Size Calculations
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 3 (CO3)
 *
 * Evaluates page table memory requirements for:
 *  1. 32-bit linear address space with 4 KB pages
 *  2. 48-bit linear address space with 4 KB pages
 *  3. Inverted page table design comparison
 */

#include <stdio.h>
#include <stdint.h>
#include <math.h>

void analyze_32bit(void) {
    printf("------------------------------------------------------------------\n");
    printf(" 1. 32-BIT ARCHITECTURE PAGE TABLE ANALYSIS (4 KB Page Size)\n");
    printf("------------------------------------------------------------------\n");
    uint64_t addr_space = 1ULL << 32;          // 4 GB
    uint64_t page_size = 4096;                  // 4 KB
    uint64_t num_pages = addr_space / page_size; // 1,048,576 pages (2^20)
    uint64_t pte_size = 4;                      // 4 bytes per PTE (32-bit entry)
    uint64_t single_level_size = num_pages * pte_size; // 4 MB

    printf("Virtual Address Bits:     32 bits\n");
    printf("Total Virtual Space:      4 GB (%llu bytes)\n", (unsigned long long)addr_space);
    printf("Page Size:                4 KB (4,096 bytes, Offset: 12 bits)\n");
    printf("Number of Pages:          %llu (2^20 pages, Page Number: 20 bits)\n", (unsigned long long)num_pages);
    printf("Page Table Entry (PTE):   %llu bytes\n", (unsigned long long)pte_size);
    printf("Single-Level Table Size:  %llu bytes (%.2f MB per process)\n",
           (unsigned long long)single_level_size, (double)single_level_size / (1024 * 1024));
    printf("Analysis: For 100 processes, flat page tables consume 400 MB RAM!\n");
    printf("Solution: 2-Level Paging (10-bit P1, 10-bit P2, 12-bit offset).\n");
    printf("          Only page directory (4 KB) + allocated second-level tables kept in RAM.\n");
}

void analyze_48bit(void) {
    printf("\n------------------------------------------------------------------\n");
    printf(" 2. 48-BIT ARCHITECTURE PAGE TABLE ANALYSIS (x86_64 Canonical 4 KB)\n");
    printf("------------------------------------------------------------------\n");
    uint64_t total_va_space = 1ULL << 48;       // 256 TB
    uint64_t page_size = 4096;                  // 4 KB (12 bits)
    uint64_t num_pages = 1ULL << 36;            // 68,719,476,736 pages (2^36)
    uint64_t pte_size = 8;                      // 8 bytes per PTE (64-bit entry)
    uint64_t flat_size = num_pages * pte_size;  // 512 GB!

    printf("Virtual Address Bits:     48 bits (Canonical Virtual Address)\n");
    printf("Total Virtual Space:      256 TB (%llu bytes)\n", (unsigned long long)total_va_space);
    printf("Page Size:                4 KB (Offset: 12 bits)\n");
    printf("Virtual Page Numbers:     %llu (2^36 pages)\n", (unsigned long long)num_pages);
    printf("PTE Size:                 %llu bytes (64-bit descriptor)\n", (unsigned long long)pte_size);
    printf("Hypothetical Flat Table:  %llu GB per process!\n", (unsigned long long)(flat_size / (1ULL << 30)));
    printf("Impracticability:         A flat page table would exceed physical RAM size.\n");
    printf("Actual Implementation:    4-Level Hierarchical Paging (PML4 -> PDPT -> PD -> PT):\n");
    printf("                          - PML4: 9 bits (512 entries x 8B = 4 KB)\n");
    printf("                          - PDPT: 9 bits (512 entries x 8B = 4 KB)\n");
    printf("                          - PD:   9 bits (512 entries x 8B = 4 KB)\n");
    printf("                          - PT:   9 bits (512 entries x 8B = 4 KB)\n");
    printf("                          - Offset: 12 bits (4096 B)\n");
    printf("                          A minimal process needs only 4 tables x 4 KB = 16 KB RAM.\n");
}

void analyze_inverted(void) {
    printf("\n------------------------------------------------------------------\n");
    printf(" 3. ALTERNATIVE CONCEPT: INVERTED PAGE TABLE (IPT)\n");
    printf("------------------------------------------------------------------\n");
    uint64_t physical_ram = 16ULL * 1024 * 1024 * 1024; // 16 GB Physical RAM
    uint64_t page_size = 4096;                          // 4 KB
    uint64_t num_frames = physical_ram / page_size;      // 4,194,304 frames
    uint64_t ipt_entry_size = 16;                        // 16 bytes (PID + Page# + flags)
    uint64_t total_ipt_size = num_frames * ipt_entry_size; // 64 MB

    printf("Physical RAM Size:        16 GB\n");
    printf("Total Physical Frames:    %llu frames\n", (unsigned long long)num_frames);
    printf("IPT Entry Size:           16 bytes (Process ID + Logical Page + Chain Ptr)\n");
    printf("Global Inverted Table:    %llu bytes (%.2f MB total for ENTIRE SYSTEM)\n",
           (unsigned long long)total_ipt_size, (double)total_ipt_size / (1024 * 1024));
    printf("Key Distinction:          Space is proportional to PHYSICAL RAM, not virtual address space.\n");
    printf("Trade-off:                Linear search is O(N_frames); requires Hash Anchor Table (HAT)\n");
    printf("                          to achieve average O(1) translation time.\n");
    printf("------------------------------------------------------------------\n");
}

int main(void) {
    printf("==================================================================\n");
    printf("  PAGE TABLE MEMORY ARCHITECTURE AND SCALING EVALUATION          \n");
    printf("  Student: S. Sachin | Reg: 192525045 | CSA04 CO3                 \n");
    printf("==================================================================\n");
    analyze_32bit();
    analyze_48bit();
    analyze_inverted();
    return 0;
}
