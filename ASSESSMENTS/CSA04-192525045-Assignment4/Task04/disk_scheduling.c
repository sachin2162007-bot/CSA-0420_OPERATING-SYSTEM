/*
 * Course: CSA04 - Operating Systems (CO4)
 * Student: S. Sachin (192525045)
 * Assignment: Assessment 4 - Optimising File Storage and Disk Access
 * Task: Task 04 - Disk Scheduling Algorithms
 * File: disk_scheduling.c
 *
 * Description:
 *   Implementation of Disk Scheduling Algorithms (FCFS, SSTF, SCAN, C-SCAN)
 *   as specified in the official Assessment 4 PDF.
 *   Head starts at cylinder 53.
 *   Request queue: 98, 183, 37, 122, 14, 124, 65, 67.
 *   Total cylinders: 200 (numbered 0 to 199).
 *
 * Compilation:
 *   gcc -std=c11 -Wall -Wextra -O2 disk_scheduling.c -o disk_scheduling
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define NUM_REQUESTS 8
#define TOTAL_CYLINDERS 200

// Supplied requests and initial head position from PDF
static const int initial_requests[NUM_REQUESTS] = {98, 183, 37, 122, 14, 124, 65, 67};
static const int INITIAL_HEAD = 53;

// FCFS Disk Scheduling
int calculate_fcfs(const int req[], int n, int head) {
    int total_movement = 0;
    int curr = head;
    for (int i = 0; i < n; i++) {
        total_movement += abs(req[i] - curr);
        curr = req[i];
    }
    return total_movement;
}

// SSTF Disk Scheduling ([RESOLVED] Completed assignment requirement)
int calculate_sstf(const int req[], int n, int head) {
    bool serviced[NUM_REQUESTS] = {false};
    int total_movement = 0;
    int curr = head;

    for (int count = 0; count < n; count++) {
        int min_dist = INT_MAX;
        int next_idx = -1;

        for (int i = 0; i < n; i++) {
            if (!serviced[i]) {
                int dist = abs(req[i] - curr);
                if (dist < min_dist) {
                    min_dist = dist;
                    next_idx = i;
                }
            }
        }

        if (next_idx != -1) {
            serviced[next_idx] = true;
            total_movement += min_dist;
            curr = req[next_idx];
        }
    }
    return total_movement;
}

// SCAN Disk Scheduling (Head moves towards 0 first, servicing downward then upward to 183)
int calculate_scan(const int req[], int n, int head) {
    // Sort requests
    int sorted[NUM_REQUESTS];
    for (int i = 0; i < n; i++) sorted[i] = req[i];
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (sorted[j] > sorted[j + 1]) {
                int tmp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = tmp;
            }
        }
    }

    // SCAN moves downward from 53 to 0, then reverses to the max request (183)
    // Movement = (head - 0) + (max_request - 0) = 53 + 183 = 236
    int min_val = 0; // Hits the lower boundary
    int max_val = sorted[n - 1]; // 183
    return (head - min_val) + (max_val - min_val);
}

// C-SCAN Disk Scheduling (Head moves upward towards 199, jumps to 0, services up to 37)
int calculate_cscan(const int req[], int n, int head) {
    // Sort requests
    int sorted[NUM_REQUESTS];
    for (int i = 0; i < n; i++) sorted[i] = req[i];
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (sorted[j] > sorted[j + 1]) {
                int tmp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = tmp;
            }
        }
    }

    // PDF Specification: C-SCAN moves from 53 to 199, returns to 0 (199 cyls), moves to 37
    // Total = (199 - 53) + (199 - 0) + (37 - 0) = 146 + 199 + 37 = 382
    int max_boundary = TOTAL_CYLINDERS - 1; // 199
    int min_boundary = 0;
    // Find the largest request less than head
    int last_serviced_below = 0;
    for (int i = 0; i < n; i++) {
        if (sorted[i] < head) {
            last_serviced_below = sorted[i]; // 37
        }
    }

    return (max_boundary - head) + (max_boundary - min_boundary) + (last_serviced_below - min_boundary);
}

int main(void) {
    int fcfs = calculate_fcfs(initial_requests, NUM_REQUESTS, INITIAL_HEAD);
    int sstf = calculate_sstf(initial_requests, NUM_REQUESTS, INITIAL_HEAD);
    int scan = calculate_scan(initial_requests, NUM_REQUESTS, INITIAL_HEAD);
    int cscan = calculate_cscan(initial_requests, NUM_REQUESTS, INITIAL_HEAD);

    printf("FCFS Total Head Movement : %d\n", fcfs);
    printf("SSTF Total Head Movement : %d\n", sstf);
    printf("SCAN Total Head Movement : %d\n", scan);
    printf("C-SCAN Total Head Movement: %d\n", cscan);

    return 0;
}
