/**
 * Task 03: Fixed Worker Thread Pool (4, 8, and 16 Threads)
 * Student Name: S. Sachin (192525045) | CSA04 OS Assessment 2
 * Uses a work-queue with condition variables and mutexes to bound concurrency.
 */

#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_CAMERAS 64
#define WORKLOAD_ITERS 15000000

typedef struct {
    int camera_ids[NUM_CAMERAS];
    int head;
    int count;
    pthread_mutex_t lock;
    pthread_cond_t notify;
    int shutdown;
} WorkQueue;

WorkQueue g_queue;

void process_frame(int cam_id) {
    (void)cam_id;
    volatile double hash = 1.0;
    for (int i = 0; i < WORKLOAD_ITERS; i++) {
        hash = (hash * 1.0000001) + 0.0000005;
    }
}

void *pool_worker(void *arg) {
    (void)arg;
    while (1) {
        int cam_id = -1;
        pthread_mutex_lock(&g_queue.lock);
        while (g_queue.count == 0 && !g_queue.shutdown) {
            pthread_cond_wait(&g_queue.notify, &g_queue.lock);
        }
        if (g_queue.shutdown && g_queue.count == 0) {
            pthread_mutex_unlock(&g_queue.lock);
            break;
        }
        cam_id = g_queue.camera_ids[g_queue.head++];
        g_queue.count--;
        pthread_mutex_unlock(&g_queue.lock);

        if (cam_id != -1) {
            process_frame(cam_id);
        }
    }
    return NULL;
}

double run_pool(int pool_size) {
    pthread_t workers[pool_size];
    g_queue.head = 0;
    g_queue.count = NUM_CAMERAS;
    g_queue.shutdown = 0;
    pthread_mutex_init(&g_queue.lock, NULL);
    pthread_cond_init(&g_queue.notify, NULL);

    for (int i = 0; i < NUM_CAMERAS; i++) {
        g_queue.camera_ids[i] = i + 1;
    }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < pool_size; i++) {
        pthread_create(&workers[i], NULL, pool_worker, NULL);
    }

    // Wait until queue is drained
    while (1) {
        pthread_mutex_lock(&g_queue.lock);
        if (g_queue.count == 0) {
            g_queue.shutdown = 1;
            pthread_cond_broadcast(&g_queue.notify);
            pthread_mutex_unlock(&g_queue.lock);
            break;
        }
        pthread_mutex_unlock(&g_queue.lock);
        struct timespec ts = {0, 5000000};
        nanosleep(&ts, NULL);
    }

    for (int i = 0; i < pool_size; i++) {
        pthread_join(workers[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    pthread_mutex_destroy(&g_queue.lock);
    pthread_cond_destroy(&g_queue.notify);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    return elapsed;
}

int main(int argc, char *argv[]) {
    int requested_pool = 0;
    if (argc > 1) requested_pool = atoi(argv[1]);

    printf("================================================================\n");
    printf(" TRAFFIC CAMERA THREAD POOL BENCHMARK (64 CAMERAS)\n");
    printf(" Student: S. Sachin (192525045) | Assessment 2 (CO2)\n");
    printf("================================================================\n\n");

    if (requested_pool > 0) {
        double t = run_pool(requested_pool);
        printf("Pool Size: %2d Threads | Time: %6.4f s\n", requested_pool, t);
        return 0;
    }

    int pools[] = {4, 8, 16};
    for (int i = 0; i < 3; i++) {
        double t = run_pool(pools[i]);
        printf("Thread Pool Size: %2d | Total Time: %6.4f s | Throughput: %5.2f cams/s\n",
               pools[i], t, NUM_CAMERAS / t);
    }
    return 0;
}
