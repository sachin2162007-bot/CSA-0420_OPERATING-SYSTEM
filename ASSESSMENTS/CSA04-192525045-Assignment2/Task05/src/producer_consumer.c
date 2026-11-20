/**
 * Task 05: Classic Producer-Consumer Problem with Semaphores
 * Student Name: S. Sachin (192525045) | CSA04 OS Assessment 2
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUF_SIZE 8
#define TOTAL_OPS 32

int g_buffer[BUF_SIZE];
int g_in = 0, g_out = 0;
sem_t g_sem_empty, g_sem_full;
pthread_mutex_t g_mtx = PTHREAD_MUTEX_INITIALIZER;

void *producer_thread(void *arg) {
    (void)arg;
    for (int i = 1; i <= TOTAL_OPS; i++) {
        sem_wait(&g_sem_empty);
        pthread_mutex_lock(&g_mtx);
        g_buffer[g_in] = i;
        printf("[PRODUCER] Inserted ticket request #%2d at slot %d\n", i, g_in);
        g_in = (g_in + 1) % BUF_SIZE;
        pthread_mutex_unlock(&g_mtx);
        sem_post(&g_sem_full);
    }
    return NULL;
}

void *consumer_thread(void *arg) {
    (void)arg;
    for (int i = 1; i <= TOTAL_OPS; i++) {
        sem_wait(&g_sem_full);
        pthread_mutex_lock(&g_mtx);
        int item = g_buffer[g_out];
        printf("  [CONSUMER] Processed ticket request #%2d from slot %d\n", item, g_out);
        g_out = (g_out + 1) % BUF_SIZE;
        pthread_mutex_unlock(&g_mtx);
        sem_post(&g_sem_empty);
    }
    return NULL;
}

int main(void) {
    sem_init(&g_sem_empty, 0, BUF_SIZE);
    sem_init(&g_sem_full, 0, 0);
    pthread_t p, c;
    pthread_create(&p, NULL, producer_thread, NULL);
    pthread_create(&c, NULL, consumer_thread, NULL);
    pthread_join(p, NULL);
    pthread_join(c, NULL);
    sem_destroy(&g_sem_empty);
    sem_destroy(&g_sem_full);
    printf("Classic Producer-Consumer simulation complete (%d operations).\n", TOTAL_OPS);
    return 0;
}
