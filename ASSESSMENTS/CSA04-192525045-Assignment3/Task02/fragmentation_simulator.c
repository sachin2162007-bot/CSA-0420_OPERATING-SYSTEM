/**
 * fragmentation_simulator.c - Internal Fragmentation Across Page Sizes
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 3 (CO3)
 *
 * Evaluates internal fragmentation for N = 100 student programs of
 * pseudo-random sizes between 2 KB and 500 KB across page sizes:
 *   - 1 KB (1,024 bytes)
 *   - 4 KB (4,096 bytes)
 *   - 2 MB (2,097,152 bytes - Huge Pages)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define NUM_PROCESSES 100

// Deterministic pseudo-random size generator for reproducible academic evaluation
uint32_t get_process_size(int pid) {
    // Generates sizes between 2,500 bytes and 480,000 bytes
    uint32_t hash = (uint32_t)(pid * 2654435761U);
    return 2500 + (hash % 477500);
}

void evaluate_page_size(uint64_t page_size, const char* label) {
    uint64_t total_requested = 0;
    uint64_t total_allocated = 0;
    uint64_t total_internal_frag = 0;
    uint64_t total_pages = 0;

    for (int i = 0; i < NUM_PROCESSES; i++) {
        uint32_t req = get_process_size(i + 1);
        uint64_t pages = (req + page_size - 1) / page_size;
        uint64_t alloc = pages * page_size;
        uint64_t frag = alloc - req;

        total_requested += req;
        total_allocated += alloc;
        total_internal_frag += frag;
        total_pages += pages;
    }

    double avg_frag_per_proc = (double)total_internal_frag / NUM_PROCESSES;
    double wasted_percentage = ((double)total_internal_frag / total_allocated) * 100.0;

    printf("%-8s | %10lu B | %12lu | %14lu B | %12.1f B | %8.2f%%\n",
           label, (unsigned long)page_size, (unsigned long)total_pages,
           (unsigned long)total_internal_frag, avg_frag_per_proc, wasted_percentage);
}

int main(void) {
    printf("===============================================================================\n");
    printf("  INTERNAL FRAGMENTATION BENCHMARK FOR 100 COLLEGE SERVER PROCESSES            \n");
    printf("  Student: S. Sachin | Reg: 192525045 | CSA04 CO3                              \n");
    printf("===============================================================================\n");
    printf("%-8s | %-12s | %-12s | %-16s | %-14s | %s\n",
           "PageSize", "Bytes", "Total Pages", "Total Frag (B)", "Avg Frag/Proc", "Wasted %%");
    printf("-------------------------------------------------------------------------------\n");

    evaluate_page_size(1024, "1 KB");
    evaluate_page_size(4096, "4 KB");
    evaluate_page_size(2097152, "2 MB");

    printf("-------------------------------------------------------------------------------\n");
    printf("\nAnalytical Insights:\n");
    printf("1. 1 KB Pages: Minimal internal fragmentation (avg ~512 B/proc, ~0.2%% waste),\n");
    printf("   but explodes page table entry count (over 24,000 pages to manage).\n");
    printf("2. 4 KB Pages: Ideal industrial sweet spot (avg ~2 KB/proc, ~0.85%% waste),\n");
    printf("   with manageable multi-level hierarchy overhead.\n");
    printf("3. 2 MB Huge Pages: Catastrophic internal fragmentation for student code!\n");
    printf("   A 50 KB program wastes 1.95 MB (97.5%% waste). Huge pages should be\n");
    printf("   reserved exclusively for large DB / ML memory pools, NOT general multi-user tasks.\n");
    printf("===============================================================================\n");

    return 0;
}
