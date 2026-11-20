/**
 * Task 06: Deadlock Conditions Demonstration
 * Student Name: S. Sachin (192525045) | CSA04 OS Assessment 2
 * Illustrates the 4 Coffman conditions: Mutual Exclusion, Hold and Wait,
 * No Preemption, and Circular Wait.
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t g_gpu_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t g_lic_mutex = PTHREAD_MUTEX_INITIALIZER;

void *job1(void *arg) {
    (void)arg;
    printf("[JOB 1] Acquiring GPU...\n");
    pthread_mutex_lock(&g_gpu_mutex);
    printf("[JOB 1] Acquired GPU. Holding GPU and sleeping...\n");
    sleep(1);

    printf("[JOB 1] Now requesting Simulation License (holds GPU, waits for License)...\n");
    pthread_mutex_lock(&g_lic_mutex);
    printf("[JOB 1] Acquired License!\n");

    pthread_mutex_unlock(&g_lic_mutex);
    pthread_mutex_unlock(&g_gpu_mutex);
    return NULL;
}

void *job2(void *arg) {
    (void)arg;
    printf("  [JOB 2] Acquiring Simulation License...\n");
    pthread_mutex_lock(&g_lic_mutex);
    printf("  [JOB 2] Acquired License. Holding License and sleeping...\n");
    sleep(1);

    printf("  [JOB 2] Now requesting GPU (holds License, waits for GPU)...\n");
    pthread_mutex_lock(&g_gpu_mutex);
    printf("  [JOB 2] Acquired GPU!\n");

    pthread_mutex_unlock(&g_gpu_mutex);
    pthread_mutex_unlock(&g_lic_mutex);
    return NULL;
}

int main(void) {
    printf("Demonstrating 4 Deadlock Conditions (Simulation with Timeout)...\n");
    printf("1. Mutual Exclusion: GPU & License can only be held by 1 job\n");
    printf("2. Hold and Wait: Job 1 holds GPU & waits for License; Job 2 holds License & waits for GPU\n");
    printf("3. No Preemption: Mutexes cannot be forcibly reclaimed\n");
    printf("4. Circular Wait: Job 1 -> License -> Job 2 -> GPU -> Job 1\n\n");

    pthread_t t1, t2;
    pthread_create(&t1, NULL, job1, NULL);
    pthread_create(&t2, NULL, job2, NULL);

    sleep(3);
    printf("\n*** DEADLOCK DETECTED! Both threads are blocked indefinitely in circular wait. ***\n");
    return 0;
}
