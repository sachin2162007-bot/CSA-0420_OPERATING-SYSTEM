/**
 * demand_paging_demo.c - Effective Access Time and Page Fault Modeling
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 3 (CO3)
 *
 * Implements mathematical evaluation of:
 *  - EAT for p = 1/1,000, 1/100,000, 1/1,000,000
 *  - Exact threshold page fault rate for 10% slowdown
 *  - Radiologist 24 GB working set model on 16 GB RAM
 */

#include <stdio.h>

void compute_eat(double p, double t_mem_ns, double t_fault_ns) {
    // EAT = (1 - p) * t_mem + p * t_fault
    double eat_ns = (1.0 - p) * t_mem_ns + p * t_fault_ns;
    double slowdown = (eat_ns - t_mem_ns) / t_mem_ns * 100.0;
    printf("Fault Rate p: 1 in %-10.0f (p = %11.8f) | EAT: %10.2f ns | Slowdown: %7.2f%%\n",
           1.0 / p, p, eat_ns, slowdown);
}

int main(void) {
    double t_mem = 100.0;             // 100 ns DRAM access time
    double t_fault = 8000000.0;       // 8 ms = 8,000,000 ns Disk I/O service time

    printf("======================================================================\n");
    printf(" DEMAND PAGING EFFECTIVE ACCESS TIME (EAT) CALCULATOR                \n");
    printf(" Student: S. Sachin | Reg: 192525045 | CSA04 CO3                     \n");
    printf(" Memory Access Time = %.0f ns | Page Fault Service Time = %.0f ns     \n", t_mem, t_fault);
    printf("======================================================================\n\n");

    printf("[1. EAT EVALUATION AT SPECIFIED PAGE FAULT RATES]:\n");
    compute_eat(1.0 / 1000.0, t_mem, t_fault);
    compute_eat(1.0 / 100000.0, t_mem, t_fault);
    compute_eat(1.0 / 1000000.0, t_mem, t_fault);

    printf("\n[2. MAXIMUM ACCEPTABLE PAGE FAULT RATE FOR 10%% SLOWDOWN]:\n");
    // Target EAT <= t_mem * 1.10 = 110 ns
    // 100 + p * (8,000,000 - 100) <= 110
    // p * 7,999,900 <= 10  =>  p <= 10 / 7,999,900
    double target_eat = t_mem * 1.10;
    double max_p = (target_eat - t_mem) / (t_fault - t_mem);
    double one_in_n = 1.0 / max_p;

    printf("  Target Max EAT:           %.2f ns (10%% slowdown)\n", target_eat);
    printf("  Max Allowable Fault Rate: p <= %.10f\n", max_p);
    printf("  Practical Requirement:    No more than 1 page fault every %.0f memory accesses!\n", one_in_n);

    printf("\n[3. RADIOLOGIST 24 GB APPLICATION MEMORY ANALYSIS]:\n");
    printf("  Program Virtual Footprint: 24 GB (High-resolution volumetric scan)\n");
    printf("  Available Physical RAM:    16 GB (OS + Cache leaves ~14 GB available)\n");
    printf("  Demand Paging Feasibility: 24 GB program can execute successfully\n");
    printf("  if its Working Set (active temporal scan slice) is <= 14 GB.\n");
    printf("  If the algorithm performs global random 3D raycasting across the entire\n");
    printf("  24 GB volume, the working set will exceed RAM, causing devastating thrashing!\n");
    printf("======================================================================\n");

    return 0;
}
