/**
 * Task 03: One Thread Per Camera (64 Threads)
 * Student Name: S. Sachin (192525045) | CSA04 OS Assessment 2
 * Creates 64 concurrent POSIX threads, one per camera stream.
 */

#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_CAMERAS 64
#define WORKLOAD_ITERS 15000000

typedef struct {
    int camera_id;
    int plate_detected;
} CameraTask;

void *camera_worker(void *arg) {
    CameraTask *task = (CameraTask *)arg;
    volatile double hash = 1.0;
    for (int i = 0; i < WORKLOAD_ITERS; i++) {
        hash = (hash * 1.0000001) + 0.0000005;
    }
    task->plate_detected = 1;
    return NULL;
}

int main(void) {
    pthread_t threads[NUM_CAMERAS];
    CameraTask tasks[NUM_CAMERAS];
    struct timespec start, end;

    printf("Processing %d cameras using 1 thread per camera (64 threads)...\n", NUM_CAMERAS);
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < NUM_CAMERAS; i++) {
        tasks[i].camera_id = i + 1;
        tasks[i].plate_detected = 0;
        if (pthread_create(&threads[i], NULL, camera_worker, &tasks[i]) != 0) {
            perror("pthread_create");
            exit(1);
        }
    }

    for (int i = 0; i < NUM_CAMERAS; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("64-thread concurrent execution completed.\n");
    printf("Total Cameras: %d | Total Wall Time: %.4f seconds\n", NUM_CAMERAS, elapsed);
    return 0;
}
