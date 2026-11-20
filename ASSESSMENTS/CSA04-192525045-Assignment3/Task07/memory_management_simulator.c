/**
 * memory_management_simulator.c - Edge Device System Memory Simulator
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 3 (CO3)
 *
 * Simulates the 2 GB Smart Factory Edge Device running:
 *   1. Real-Time Control Task (100 MB pinned via mlock)
 *   2. Machine-Learning Inference Task (1,536 MB demand-paged)
 *   3. Industrial Logging Task (50 MB buffered)
 *   4. Evaluates feasibility of adding Fourth Task (500 MB)
 */

#include <stdio.h>
#include <stdbool.h>

#define TOTAL_RAM_MB 2048 // 2 GB
#define OS_RESERVED_MB 128 // Linux Kernel + Drivers

typedef struct {
    const char* name;
    int memory_req_mb;
    bool is_realtime;
    bool is_locked;
    int frames_allocated_mb;
} Task;

void simulate_edge_system(bool include_fourth_task) {
    Task tasks[4] = {
        {"Real-Time Control Task", 100,  true,  true,  100},
        {"ML Vision Inference",    1536, false, false, 0},
        {"System Logging Daemon",  50,   false, false, 0},
        {"Proposed 4th Task",      500,  false, false, 0}
    };
    int num_tasks = include_fourth_task ? 4 : 3;

    int available_ram = TOTAL_RAM_MB - OS_RESERVED_MB;
    int locked_ram = tasks[0].memory_req_mb;
    int dynamic_ram_pool = available_ram - locked_ram;

    int total_demand = OS_RESERVED_MB;
    for (int i = 0; i < num_tasks; i++) total_demand += tasks[i].memory_req_mb;

    printf("=======================================================================\n");
    printf("  SMART FACTORY MEMORY SIMULATOR: %s\n", include_fourth_task ? "WITH 4TH TASK (500 MB)" : "BASELINE (3 TASKS)");
    printf("  Physical RAM: %d MB | OS Reserved: %d MB | Usable Dynamic RAM: %d MB\n",
           TOTAL_RAM_MB, OS_RESERVED_MB, dynamic_ram_pool);
    printf("=======================================================================\n");

    printf("%-24s | %-12s | %-10s | %-14s | %s\n",
           "Task Name", "Demand (MB)", "Type", "Memory State", "Allocation Status");
    printf("-----------------------------------------------------------------------\n");

    for (int i = 0; i < num_tasks; i++) {
        if (tasks[i].is_locked) {
            printf("%-24s | %10d MB | %-10s | %-14s | GUARANTEED PINNED (100%%)\n",
                   tasks[i].name, tasks[i].memory_req_mb, "Real-Time", "Pinned (mlock)");
        } else {
            // Allocate proportionally from dynamic pool
            int remaining_demand = total_demand - OS_RESERVED_MB - locked_ram;
            double share = (double)tasks[i].memory_req_mb / remaining_demand;
            int alloc = (int)(share * dynamic_ram_pool);
            tasks[i].frames_allocated_mb = alloc;

            const char* status = (alloc >= tasks[i].memory_req_mb) ? "Fully Resident" : "Demand-Paged (OOM Risk)";
            if (include_fourth_task && total_demand > TOTAL_RAM_MB) {
                status = "OOM COLLAPSE / THRASHING";
            }
            printf("%-24s | %10d MB | %-10s | %-14s | %d MB allocated (%s)\n",
                   tasks[i].name, tasks[i].memory_req_mb, "Standard", "Virtual Only", alloc, status);
        }
    }

    printf("-----------------------------------------------------------------------\n");
    printf("Total System Memory Demand: %d MB / %d MB Physical RAM\n", total_demand, TOTAL_RAM_MB);

    if (total_demand <= TOTAL_RAM_MB) {
        printf("VERDICT: STABLE & FEASIBLE. Real-time task is strictly isolated;\n");
        printf("ML task and Logger execute without exceeding physical RAM boundaries.\n");
    } else {
        printf("VERDICT: INFEASIBLE WITHOUT INTERVENTION! Total demand exceeds RAM by %d MB.\n",
               total_demand - TOTAL_RAM_MB);
        printf("Because the edge device has NO SWAP DISK, overcommit will trigger the\n");
        printf("Linux Out-Of-Memory (OOM) Killer, terminating the ML task or Logger!\n");
        printf("Required Engineering Mitigation: Quantize ML model to <= 1,000 MB or reject Task 4.\n");
    }
    printf("=======================================================================\n\n");
}

int main(void) {
    simulate_edge_system(false); // 3 Tasks baseline
    simulate_edge_system(true);  // With 4th Task
    return 0;
}
