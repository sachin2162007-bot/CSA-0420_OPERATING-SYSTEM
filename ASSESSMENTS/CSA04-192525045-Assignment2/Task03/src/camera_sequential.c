/**
 * Task 03: Sequential Traffic Camera Processing
 * Student Name: S. Sachin (192525045) | CSA04 OS Assessment 2
 * Processes 64 cameras one after another to establish baseline execution time.
 */

#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_CAMERAS 64
#define WORKLOAD_ITERS 15000000

void process_camera_frame(int cam_id) {
    volatile double hash = 1.0;
    for (int i = 0; i < WORKLOAD_ITERS; i++) {
        hash = (hash * 1.0000001) + 0.0000005;
    }
    // Simulation of OCR / plate identification
    if (cam_id % 10 == 0) {
        // Periodic trace output
    }
}

int main(void) {
    struct timespec start, end;
    printf("Processing %d traffic cameras sequentially...\n", NUM_CAMERAS);
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < NUM_CAMERAS; i++) {
        process_camera_frame(i + 1);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Sequential execution completed.\n");
    printf("Total Cameras: %d | Total Wall Time: %.4f seconds | Avg per Cam: %.4f ms\n",
           NUM_CAMERAS, elapsed, (elapsed / NUM_CAMERAS) * 1000.0);
    return 0;
}
