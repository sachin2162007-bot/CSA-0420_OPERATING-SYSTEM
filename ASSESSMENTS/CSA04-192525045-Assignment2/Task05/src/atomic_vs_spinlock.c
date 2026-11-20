/**
 * Task 05: Atomic Operations vs Spinlocks vs Mutexes
 * Student Name: S. Sachin (192525045) | CSA04 OS Assessment 2
 * Evaluates performance under 1, 4, and 16 threads for short critical sections.
 */

#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>

#define TOTAL_OPS 2000000

atomic_long g_atomic_counter = 0;
long g_spinlock_counter = 0;
long g_mutex_counter = 0;

pthread_spinlock_t g_spinlock;
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int threads;
    int mode; // 1: Atomic, 2: Spinlock, 3: Mutex
} Param;

void *worker(void *arg) {
    Param *p = (Param *)arg;
    int ops = TOTAL_OPS / p->threads;

    if (p->mode == 1) {
        for (int i = 0; i < ops; i++) {
            atomic_fetch_add(&g_atomic_counter, 1);
        }
    } else if (p->mode == 2) {
        for (int i = 0; i < ops; i++) {
            pthread_spin_lock(&g_spinlock);
            g_spinlock_counter++;
            pthread_spin_unlock(&g_spinlock);
        }
    } else if (p->mode == 3) {
        for (int i = 0; i < ops; i++) {
            pthread_mutex_lock(&g_mutex);
            g_mutex_counter++;
            pthread_mutex_unlock(&g_mutex);
        }
    }
    return NULL;
}

double run_test(int threads, int mode) {
    pthread_t th[threads];
    Param p = {threads, mode};
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < threads; i++) pthread_create(&th[i], NULL, worker, &p);
    for (int i = 0; i < threads; i++) pthread_join(th[i], NULL);

    clock_gettime(CLOCK_MONOTONIC, &end);
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main(void) {
    pthread_spin_init(&g_spinlock, PTHREAD_PROCESS_PRIVATE);

    printf("=======================================================================\n");
    printf(" SYNCHRONISATION PRIMITIVE COMPARISON: ATOMIC vs SPINLOCK vs MUTEX\n");
    printf(" Student: S. Sachin (192525045) | Assessment 2 (CO2)\n");
    printf(" 2,000,000 Total Updates per Benchmark\n");
    printf("=======================================================================\n\n");

    int thread_counts[] = {1, 4, 16};

    printf("[1] C11 Atomic Operations (atomic_fetch_add):\n");
    for (int i = 0; i < 3; i++) {
        g_atomic_counter = 0;
        int t = thread_counts[i];
        double el = run_test(t, 1);
        printf("  Threads: %2d | Time: %6.4f s | Latency: %6.2f ns/op\n",
               t, el, (el / TOTAL_OPS) * 1e9);
    }

    printf("\n[2] POSIX Spinlocks (pthread_spinlock_t):\n");
    for (int i = 0; i < 3; i++) {
        g_spinlock_counter = 0;
        int t = thread_counts[i];
        double el = run_test(t, 2);
        printf("  Threads: %2d | Time: %6.4f s | Latency: %6.2f ns/op\n",
               t, el, (el / TOTAL_OPS) * 1e9);
    }

    printf("\n[3] POSIX Mutexes (pthread_mutex_t):\n");
    for (int i = 0; i < 3; i++) {
        g_mutex_counter = 0;
        int t = thread_counts[i];
        double el = run_test(t, 3);
        printf("  Threads: %2d | Time: %6.4f s | Latency: %6.2f ns/op\n",
               t, el, (el / TOTAL_OPS) * 1e9);
    }

    pthread_spin_destroy(&g_spinlock);
    return 0;
}
