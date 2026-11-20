/**
 * tlb_calculator.c - Translation Lookaside Buffer (TLB) Access Time Evaluator
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 3 (CO3)
 *
 * Evaluates Effective Access Time (EAT) across hit ratios:
 *   - alpha = 80%, 95%, 99%
 * With single-level and multi-level (4-level) page walks.
 */

#include <stdio.h>

void calculate_eat(double tlb_hit_ratio, double tlb_search_time, double mem_access_time, int page_levels) {
    // EAT for single-level paging:
    // Hit: TLB search + 1 Memory Access (to fetch data)
    // Miss: TLB search + page_levels Memory Accesses (page walk) + 1 Memory Access (fetch data)
    double t_hit = tlb_search_time + mem_access_time;
    double t_miss = tlb_search_time + (page_levels + 1) * mem_access_time;
    double eat = (tlb_hit_ratio * t_hit) + ((1.0 - tlb_hit_ratio) * t_miss);
    double no_tlb_eat = (page_levels + 1) * mem_access_time;
    double speedup = no_tlb_eat / eat;

    printf("Hit Ratio: %5.1f%% | EAT (1-level): %6.2f ns | EAT (%d-level): %6.2f ns | Speedup: %.2fx\n",
           tlb_hit_ratio * 100.0,
           (tlb_hit_ratio * (tlb_search_time + mem_access_time)) + ((1.0 - tlb_hit_ratio) * (tlb_search_time + 2.0 * mem_access_time)),
           page_levels,
           eat,
           speedup);
}

int main(void) {
    double tlb_search = 10.0;     // 10 ns TLB associative lookup
    double mem_access = 100.0;    // 100 ns Main Memory DRAM latency
    int levels = 4;               // x86_64 4-level paging

    printf("======================================================================\n");
    printf("  TLB EFFECTIVE ACCESS TIME (EAT) SIMULATOR                          \n");
    printf("  Student: S. Sachin | Reg: 192525045 | CSA04 CO3                    \n");
    printf("  Parameters: TLB Search Time = %.1f ns | Memory Access Time = %.1f ns\n", tlb_search, mem_access);
    printf("======================================================================\n");

    printf("\n[Baseline Without TLB]:\n");
    printf("  Single-Level Page Table: 2 * 100 ns = 200.0 ns\n");
    printf("  4-Level Page Table:      (4 + 1) * 100 ns = 500.0 ns (5x slowdown!)\n\n");

    printf("[Effective Access Time Across Hit Ratios]:\n");
    double ratios[] = {0.80, 0.90, 0.95, 0.98, 0.99, 0.999};
    int n = sizeof(ratios) / sizeof(ratios[0]);
    for (int i = 0; i < n; i++) {
        calculate_eat(ratios[i], tlb_search, mem_access, levels);
    }

    printf("\nConclusion:\n");
    printf("  A 99%% TLB hit ratio reduces 4-level translation overhead from 500 ns to 113.9 ns,\n");
    printf("  achieving a 4.39x memory access performance speedup!\n");
    printf("======================================================================\n");
    return 0;
}
