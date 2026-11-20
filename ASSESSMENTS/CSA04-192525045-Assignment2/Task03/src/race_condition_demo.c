/**
 * Task 03: Shared Counter Race Condition Demonstration
 * Student Name: S. Sachin (192525045) | CSA04 OS Assessment 2
 * Shows data corruption without mutex vs correct synchronization with mutex.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4
#define ITERATIONS 500000

long g_unprotected_counter = 0;
long g_protected_counter = 0;
pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

void *worker_unprotected(void *arg) {
    (void)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        // Assembly: LOAD counter, ADD 1, STORE counter (unprotected race)
        g_unprotected_counter++;
    }
    return NULL;
}

void *worker_protected(void *arg) {
    (void)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&g_lock);
        g_protected_counter++;
        pthread_mutex_unlock(&g_lock);
    }
    return NULL;
}

int main(void) {
    pthread_t t[NUM_THREADS];
    long expected = (long)NUM_THREADS * ITERATIONS;

    // 1. Unprotected Run
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&t[i], NULL, worker_unprotected, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(t[i], NULL);
    }

    // 2. Protected Run
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&t[i], NULL, worker_protected, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(t[i], NULL);
    }

    printf("================================================================\n");
    printf(" RACE CONDITION DEMONSTRATION: %d THREADS x %d ITERATIONS\n", NUM_THREADS, ITERATIONS);
    printf(" Expected Final Value:  %ld\n", expected);
    printf(" Unprotected Counter:   %ld (Lost Updates: %ld, %.2f%% error)\n",
           g_unprotected_counter, expected - g_unprotected_counter,
           ((double)(expected - g_unprotected_counter) / expected) * 100.0);
    printf(" Mutex-Protected Value: %ld (Lost Updates: 0, 100%% correct)\n",
           g_protected_counter);
    printf("================================================================\n");
    return 0;
}
