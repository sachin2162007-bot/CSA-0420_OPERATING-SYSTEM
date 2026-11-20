/**
 * raid_capacity.c - RAID Usable Capacity & Storage Efficiency Calculator
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 3 (CO3)
 *
 * Evaluates storage configurations for N = 6 disks of C = 4 TB each across:
 *  - RAID 0, RAID 1, RAID 5, RAID 6, and RAID 10
 */

#include <stdio.h>

void evaluate_raid(int n_disks, double disk_size_tb) {
    double raw_capacity = n_disks * disk_size_tb;

    printf("===================================================================================\n");
    printf("  RAID USABLE CAPACITY AND EFFICIENCY EVALUATOR                                    \n");
    printf("  Student: S. Sachin | Reg: 192525045 | CSA04 CO3                                  \n");
    printf("  Disks: %d drives | Individual Drive Capacity: %.1f TB | Raw Storage: %.1f TB      \n",
           n_disks, disk_size_tb, raw_capacity);
    printf("===================================================================================\n");
    printf("%-9s | %-12s | %-12s | %-14s | %-12s | %s\n",
           "RAID Level", "Usable (TB)", "Parity (TB)", "Efficiency (%)", "Fault Tol", "Write Penalty");
    printf("-----------------------------------------------------------------------------------\n");

    // RAID 0: All striped
    double c_r0 = n_disks * disk_size_tb;
    printf("%-9s | %10.1f TB | %10.1f TB | %13.1f%% | %11s | %s\n",
           "RAID 0", c_r0, 0.0, (c_r0 / raw_capacity) * 100.0, "0 Disks", "1x (No overhead)");

    // RAID 1: Mirrored pairs (3 pairs of 2 disks)
    double c_r1 = (n_disks / 2.0) * disk_size_tb;
    printf("%-9s | %10.1f TB | %10.1f TB | %13.1f%% | %11s | %s\n",
           "RAID 1", c_r1, c_r1, (c_r1 / raw_capacity) * 100.0, "1 / pair (3 max)", "2x (Dual write)");

    // RAID 5: Single distributed parity (N-1)
    double c_r5 = (n_disks - 1) * disk_size_tb;
    printf("%-9s | %10.1f TB | %10.1f TB | %13.1f%% | %11s | %s\n",
           "RAID 5", c_r5, disk_size_tb, (c_r5 / raw_capacity) * 100.0, "1 Disk", "4x (2 Read, 2 Write)");

    // RAID 6: Dual distributed parity (N-2)
    double c_r6 = (n_disks - 2) * disk_size_tb;
    printf("%-9s | %10.1f TB | %10.1f TB | %13.1f%% | %11s | %s\n",
           "RAID 6", c_r6, 2.0 * disk_size_tb, (c_r6 / raw_capacity) * 100.0, "2 Disks", "6x (3 Read, 3 Write)");

    // RAID 10: Striped mirrors 1+0 (N/2)
    double c_r10 = (n_disks / 2.0) * disk_size_tb;
    printf("%-9s | %10.1f TB | %10.1f TB | %13.1f%% | %11s | %s\n",
           "RAID 10", c_r10, c_r10, (c_r10 / raw_capacity) * 100.0, "1 / pair (3 max)", "2x (Dual write)");

    printf("-----------------------------------------------------------------------------------\n");
    printf("Hospital Records Server Recommendation:\n");
    printf("  RAID 10 is recommended for live transactional EHR databases (Fast writes, rapid rebuild).\n");
    printf("  RAID 6 is recommended for long-term patient medical imaging archives (High capacity + 2-disk fault tolerance).\n");
    printf("===================================================================================\n");
}

int main(void) {
    evaluate_raid(6, 4.0);
    return 0;
}
