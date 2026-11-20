/**
 * Task 05: Railway Booking Lock Granularity Benchmark
 * Student Name: S. Sachin (192525045) | CSA04 OS Assessment 2
 *
 * Compares 3 locking strategies under 1, 4, and 16 concurrent threads:
 * 1. Single Global Lock for entire railway booking system
 * 2. Per-Train Lock (10 trains)
 * 3. Per-Coach Lock (10 trains x 10 coaches = 100 coach locks)
 */

#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define TOTAL_BOOKINGS 200000
#define NUM_TRAINS 10
#define COACHES_PER_TRAIN 10

// Lock granularities
pthread_mutex_t g_global_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t g_train_locks[NUM_TRAINS];
pthread_mutex_t g_coach_locks[NUM_TRAINS][COACHES_PER_TRAIN];

int g_train_seats[NUM_TRAINS][COACHES_PER_TRAIN];

typedef struct {
    int num_threads;
    int strategy; // 1: Global, 2: Train, 3: Coach
} BenchParam;

void *bench_worker(void *arg) {
    BenchParam *param = (BenchParam *)arg;
    int ops = TOTAL_BOOKINGS / param->num_threads;

    for (int i = 0; i < ops; i++) {
        int t = i % NUM_TRAINS;
        int c = (i / NUM_TRAINS) % COACHES_PER_TRAIN;

        if (param->strategy == 1) {
            // Strategy 1: Global Lock
            pthread_mutex_lock(&g_global_lock);
            g_train_seats[t][c]++;
            pthread_mutex_unlock(&g_global_lock);
        } else if (param->strategy == 2) {
            // Strategy 2: Per-Train Lock
            pthread_mutex_lock(&g_train_locks[t]);
            g_train_seats[t][c]++;
            pthread_mutex_unlock(&g_train_locks[t]);
        } else if (param->strategy == 3) {
            // Strategy 3: Per-Coach Lock
            pthread_mutex_lock(&g_coach_locks[t][c]);
            g_train_seats[t][c]++;
            pthread_mutex_unlock(&g_coach_locks[t][c]);
        }
    }
    return NULL;
}

double run_granularity_bench(int threads, int strategy) {
    pthread_t th[threads];
    BenchParam param = {threads, strategy};

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < threads; i++) {
        pthread_create(&th[i], NULL, bench_worker, &param);
    }
    for (int i = 0; i < threads; i++) {
        pthread_join(th[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main(void) {
    // Initialize locks
    for (int t = 0; t < NUM_TRAINS; t++) {
        pthread_mutex_init(&g_train_locks[t], NULL);
        for (int c = 0; c < COACHES_PER_TRAIN; c++) {
            pthread_mutex_init(&g_coach_locks[t][c], NULL);
        }
    }

    printf("=======================================================================\n");
    printf(" RAILWAY BOOKING LOCK GRANULARITY BENCHMARK (200,000 Bookings)\n");
    printf(" Student: S. Sachin (192525045) | Assessment 2 (CO2)\n");
    printf("=======================================================================\n\n");

    int thread_counts[] = {1, 4, 16};
    const char *names[] = {"1. Global Lock", "2. Per-Train Lock", "3. Per-Coach Lock"};

    for (int s = 1; s <= 3; s++) {
        printf("--- Locking Strategy: %s ---\n", names[s - 1]);
        for (int tc = 0; tc < 3; tc++) {
            int t = thread_counts[tc];
            double elapsed = run_granularity_bench(t, s);
            printf("  Threads: %2d | Time: %6.4f s | Throughput: %10.2f bookings/sec\n",
                   t, elapsed, TOTAL_BOOKINGS / elapsed);
        }
        printf("\n");
    }
    return 0;
}
