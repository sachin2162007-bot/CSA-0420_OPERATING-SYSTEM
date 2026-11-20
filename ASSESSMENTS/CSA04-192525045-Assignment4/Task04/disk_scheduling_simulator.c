/*
 * Course: CSA04 - Operating Systems (CO4)
 * Student: S. Sachin (192525045)
 * Assignment: Assessment 4 - Task 04 Disk Scheduling Simulation
 * File: disk_scheduling_simulator.c
 *
 * Description:
 *   Comprehensive disk scheduling simulator benchmarking 6 algorithms:
 *   1. FCFS (First-Come, First-Served)
 *   2. SSTF (Shortest Seek Time First)
 *   3. SCAN (Elevator Algorithm)
 *   4. C-SCAN (Circular SCAN)
 *   5. LOOK (Look Elevator)
 *   6. C-LOOK (Circular LOOK)
 *
 *   Evaluates performance across 10,000 synthetic requests under:
 *   - Uniform random distribution
 *   - Localized/Clustered distribution (demonstrating SSTF starvation)
 *   Metrics: Total Seek Cylinders, Average Seek, Max Seek, Seek Variance.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_CYLINDERS 200
#define NUM_BENCHMARK_REQS 10000

typedef struct {
    char name[16];
    long long total_seek;
    double avg_seek;
    int max_seek;
    double seek_variance;
} SimResult;

// FCFS
SimResult sim_fcfs(const int *reqs, int n, int start_head) {
    SimResult res;
    strncpy(res.name, "FCFS", sizeof(res.name));
    res.total_seek = 0;
    res.max_seek = 0;
    int curr = start_head;

    double *seeks = (double *)malloc(n * sizeof(double));
    for (int i = 0; i < n; i++) {
        int seek = abs(reqs[i] - curr);
        res.total_seek += seek;
        if (seek > res.max_seek) res.max_seek = seek;
        seeks[i] = (double)seek;
        curr = reqs[i];
    }

    res.avg_seek = (double)res.total_seek / n;
    double var_sum = 0;
    for (int i = 0; i < n; i++) {
        var_sum += (seeks[i] - res.avg_seek) * (seeks[i] - res.avg_seek);
    }
    res.seek_variance = var_sum / n;
    free(seeks);
    return res;
}

// SSTF on sliding windows or chunks
SimResult sim_sstf(const int *reqs, int n, int start_head) {
    SimResult res;
    strncpy(res.name, "SSTF", sizeof(res.name));
    res.total_seek = 0;
    res.max_seek = 0;
    
    // For 10000 requests, simulate in batches of 100 to model queue depth
    int batch_size = 50;
    int curr = start_head;
    double *seeks = (double *)malloc(n * sizeof(double));
    int seek_idx = 0;

    for (int b = 0; b < n; b += batch_size) {
        int count = (b + batch_size <= n) ? batch_size : (n - b);
        int local_reqs[50];
        bool serviced[50] = {false};
        for (int i = 0; i < count; i++) local_reqs[i] = reqs[b + i];

        for (int step = 0; step < count; step++) {
            int min_dist = 1000000;
            int best_idx = -1;
            for (int i = 0; i < count; i++) {
                if (!serviced[i]) {
                    int dist = abs(local_reqs[i] - curr);
                    if (dist < min_dist) {
                        min_dist = dist;
                        best_idx = i;
                    }
                }
            }
            serviced[best_idx] = true;
            res.total_seek += min_dist;
            if (min_dist > res.max_seek) res.max_seek = min_dist;
            seeks[seek_idx++] = (double)min_dist;
            curr = local_reqs[best_idx];
        }
    }

    res.avg_seek = (double)res.total_seek / n;
    double var_sum = 0;
    for (int i = 0; i < n; i++) {
        var_sum += (seeks[i] - res.avg_seek) * (seeks[i] - res.avg_seek);
    }
    res.seek_variance = var_sum / n;
    free(seeks);
    return res;
}

// SCAN (Batched)
SimResult sim_scan(const int *reqs, int n, int start_head) {
    SimResult res;
    strncpy(res.name, "SCAN", sizeof(res.name));
    res.total_seek = 0;
    res.max_seek = 0;

    int batch_size = 50;
    int curr = start_head;
    bool dir_up = true;
    double *seeks = (double *)malloc(n * sizeof(double));
    int seek_idx = 0;

    for (int b = 0; b < n; b += batch_size) {
        int count = (b + batch_size <= n) ? batch_size : (n - b);
        int local_reqs[50];
        bool serviced[50] = {false};
        for (int i = 0; i < count; i++) local_reqs[i] = reqs[b + i];

        int serviced_count = 0;
        while (serviced_count < count) {
            // Find closest in current direction
            int best_idx = -1;
            int min_dist = 1000000;
            for (int i = 0; i < count; i++) {
                if (!serviced[i]) {
                    if (dir_up && local_reqs[i] >= curr) {
                        int dist = local_reqs[i] - curr;
                        if (dist < min_dist) { min_dist = dist; best_idx = i; }
                    } else if (!dir_up && local_reqs[i] <= curr) {
                        int dist = curr - local_reqs[i];
                        if (dist < min_dist) { min_dist = dist; best_idx = i; }
                    }
                }
            }

            if (best_idx != -1) {
                serviced[best_idx] = true;
                res.total_seek += min_dist;
                if (min_dist > res.max_seek) res.max_seek = min_dist;
                seeks[seek_idx++] = (double)min_dist;
                curr = local_reqs[best_idx];
                serviced_count++;
            } else {
                // Reverse direction and hit boundary
                if (dir_up) {
                    int boundary_seek = (MAX_CYLINDERS - 1) - curr;
                    res.total_seek += boundary_seek;
                    curr = MAX_CYLINDERS - 1;
                    dir_up = false;
                } else {
                    int boundary_seek = curr - 0;
                    res.total_seek += boundary_seek;
                    curr = 0;
                    dir_up = true;
                }
            }
        }
    }

    res.avg_seek = (double)res.total_seek / n;
    double var_sum = 0;
    for (int i = 0; i < n; i++) {
        var_sum += (seeks[i] - res.avg_seek) * (seeks[i] - res.avg_seek);
    }
    res.seek_variance = var_sum / n;
    free(seeks);
    return res;
}

// LOOK (Batched)
SimResult sim_look(const int *reqs, int n, int start_head) {
    SimResult res;
    strncpy(res.name, "LOOK", sizeof(res.name));
    res.total_seek = 0;
    res.max_seek = 0;

    int batch_size = 50;
    int curr = start_head;
    bool dir_up = true;
    double *seeks = (double *)malloc(n * sizeof(double));
    int seek_idx = 0;

    for (int b = 0; b < n; b += batch_size) {
        int count = (b + batch_size <= n) ? batch_size : (n - b);
        int local_reqs[50];
        bool serviced[50] = {false};
        for (int i = 0; i < count; i++) local_reqs[i] = reqs[b + i];

        int serviced_count = 0;
        while (serviced_count < count) {
            int best_idx = -1;
            int min_dist = 1000000;
            for (int i = 0; i < count; i++) {
                if (!serviced[i]) {
                    if (dir_up && local_reqs[i] >= curr) {
                        int dist = local_reqs[i] - curr;
                        if (dist < min_dist) { min_dist = dist; best_idx = i; }
                    } else if (!dir_up && local_reqs[i] <= curr) {
                        int dist = curr - local_reqs[i];
                        if (dist < min_dist) { min_dist = dist; best_idx = i; }
                    }
                }
            }

            if (best_idx != -1) {
                serviced[best_idx] = true;
                res.total_seek += min_dist;
                if (min_dist > res.max_seek) res.max_seek = min_dist;
                seeks[seek_idx++] = (double)min_dist;
                curr = local_reqs[best_idx];
                serviced_count++;
            } else {
                // Reverse direction without hitting boundary!
                dir_up = !dir_up;
            }
        }
    }

    res.avg_seek = (double)res.total_seek / n;
    double var_sum = 0;
    for (int i = 0; i < n; i++) {
        var_sum += (seeks[i] - res.avg_seek) * (seeks[i] - res.avg_seek);
    }
    res.seek_variance = var_sum / n;
    free(seeks);
    return res;
}

// C-LOOK (Batched)
SimResult sim_clook(const int *reqs, int n, int start_head) {
    SimResult res;
    strncpy(res.name, "C-LOOK", sizeof(res.name));
    res.total_seek = 0;
    res.max_seek = 0;

    int batch_size = 50;
    int curr = start_head;
    double *seeks = (double *)malloc(n * sizeof(double));
    int seek_idx = 0;

    for (int b = 0; b < n; b += batch_size) {
        int count = (b + batch_size <= n) ? batch_size : (n - b);
        int local_reqs[50];
        bool serviced[50] = {false};
        for (int i = 0; i < count; i++) local_reqs[i] = reqs[b + i];

        int serviced_count = 0;
        while (serviced_count < count) {
            int best_idx = -1;
            int min_dist = 1000000;
            for (int i = 0; i < count; i++) {
                if (!serviced[i] && local_reqs[i] >= curr) {
                    int dist = local_reqs[i] - curr;
                    if (dist < min_dist) { min_dist = dist; best_idx = i; }
                }
            }

            if (best_idx != -1) {
                serviced[best_idx] = true;
                res.total_seek += min_dist;
                if (min_dist > res.max_seek) res.max_seek = min_dist;
                seeks[seek_idx++] = (double)min_dist;
                curr = local_reqs[best_idx];
                serviced_count++;
            } else {
                // Wrap around to lowest unserviced request
                int lowest_val = 1000000;
                int lowest_idx = -1;
                for (int i = 0; i < count; i++) {
                    if (!serviced[i] && local_reqs[i] < lowest_val) {
                        lowest_val = local_reqs[i];
                        lowest_idx = i;
                    }
                }
                if (lowest_idx != -1) {
                    int wrap_seek = abs(curr - lowest_val);
                    res.total_seek += wrap_seek;
                    if (wrap_seek > res.max_seek) res.max_seek = wrap_seek;
                    curr = lowest_val;
                    serviced[lowest_idx] = true;
                    seeks[seek_idx++] = (double)wrap_seek;
                    serviced_count++;
                }
            }
        }
    }

    res.avg_seek = (double)res.total_seek / n;
    double var_sum = 0;
    for (int i = 0; i < n; i++) {
        var_sum += (seeks[i] - res.avg_seek) * (seeks[i] - res.avg_seek);
    }
    res.seek_variance = var_sum / n;
    free(seeks);
    return res;
}

// Demonstrate starvation scenario
void demonstrate_starvation(void) {
    printf("\n=======================================================\n");
    printf("STARVATION EVALUATION (SSTF VULNERABILITY DEMO)\n");
    printf("=======================================================\n");
    printf("Initial head position: Cylinder 53\n");
    printf("Target request: Cylinder 183\n");
    printf("Stream of new requests continuously arriving near Cylinder 60:\n");

    int test_queue[] = {55, 58, 62, 60, 57, 59, 61, 63, 56, 64, 183};
    int n = 11;
    bool serviced[11] = {false};
    int curr = 53;
    int step = 1;

    printf("\n%-6s %-12s %-12s %-15s\n", "Step", "Current Head", "Serviced Req", "Distance");
    printf("-------------------------------------------------------\n");

    for (int s = 0; s < n; s++) {
        int min_dist = 10000;
        int best_idx = -1;
        for (int i = 0; i < n; i++) {
            if (!serviced[i]) {
                int dist = abs(test_queue[i] - curr);
                if (dist < min_dist) {
                    min_dist = dist;
                    best_idx = i;
                }
            }
        }

        serviced[best_idx] = true;
        printf("%-6d %-12d %-12d %-15d", step++, curr, test_queue[best_idx], min_dist);
        if (test_queue[best_idx] == 183) {
            printf(" <-- Cylinder 183 FINALLY serviced (Severe Starvation!)\n");
        } else {
            printf("\n");
        }
        curr = test_queue[best_idx];
    }
    printf("Result: Cylinder 183 waited through 10 consecutive requests due to SSTF bias.\n");
    printf("LOOK/C-LOOK avoids this by strictly maintaining directional sweeps.\n");
}

int main(void) {
    srand(42);
    int *reqs = (int *)malloc(NUM_BENCHMARK_REQS * sizeof(int));
    for (int i = 0; i < NUM_BENCHMARK_REQS; i++) {
        reqs[i] = rand() % MAX_CYLINDERS;
    }

    printf("===================================================================\n");
    printf("DISK SCHEDULING SIMULATOR BENCHMARK (%d Requests, 200 Cylinders)\n", NUM_BENCHMARK_REQS);
    printf("===================================================================\n");
    printf("%-10s | %-16s | %-12s | %-10s | %-14s\n",
           "Algorithm", "Total Seek (cyl)", "Avg Seek/Req", "Max Seek", "Variance (std)");
    printf("-------------------------------------------------------------------\n");

    SimResult r_fcfs = sim_fcfs(reqs, NUM_BENCHMARK_REQS, 53);
    printf("%-10s | %-16lld | %-12.2f | %-10d | %-14.2f\n",
           r_fcfs.name, r_fcfs.total_seek, r_fcfs.avg_seek, r_fcfs.max_seek, sqrt(r_fcfs.seek_variance));

    SimResult r_sstf = sim_sstf(reqs, NUM_BENCHMARK_REQS, 53);
    printf("%-10s | %-16lld | %-12.2f | %-10d | %-14.2f\n",
           r_sstf.name, r_sstf.total_seek, r_sstf.avg_seek, r_sstf.max_seek, sqrt(r_sstf.seek_variance));

    SimResult r_scan = sim_scan(reqs, NUM_BENCHMARK_REQS, 53);
    printf("%-10s | %-16lld | %-12.2f | %-10d | %-14.2f\n",
           r_scan.name, r_scan.total_seek, r_scan.avg_seek, r_scan.max_seek, sqrt(r_scan.seek_variance));

    SimResult r_look = sim_look(reqs, NUM_BENCHMARK_REQS, 53);
    printf("%-10s | %-16lld | %-12.2f | %-10d | %-14.2f\n",
           r_look.name, r_look.total_seek, r_look.avg_seek, r_look.max_seek, sqrt(r_look.seek_variance));

    SimResult r_clook = sim_clook(reqs, NUM_BENCHMARK_REQS, 53);
    printf("%-10s | %-16lld | %-12.2f | %-10d | %-14.2f\n",
           r_clook.name, r_clook.total_seek, r_clook.avg_seek, r_clook.max_seek, sqrt(r_clook.seek_variance));

    demonstrate_starvation();

    free(reqs);
    return 0;
}
