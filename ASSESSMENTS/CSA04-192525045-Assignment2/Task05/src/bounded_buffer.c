/**
 * Task 05: Synchronisation - Bounded Buffer Producer-Consumer
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 2 (CO2)
 *
 * Implements the supplied starter code from the assignment PDF completing
 * TODO 1 and TODO 2 using semaphores and mutex while preserving the structure.
 * Supports configurable multi-producer/consumer mode and no-mutex race test.
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define SIZE 5
#define ITEMS 20

int buffer[SIZE], in = 0, out = 0;
sem_t empty, full;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
long consumed_sum = 0;
int g_use_mutex = 1;

void *producer(void *arg)
{
    (void)arg;
    for (int item = 1; item <= ITEMS; item++) {
        // TODO 1: Wait on empty, lock the mutex, put item at buffer[in], advance in
        // circularly, unlock, then post full
        sem_wait(&empty);
        if (g_use_mutex) pthread_mutex_lock(&lock);
        buffer[in] = item;
        in = (in + 1) % SIZE;
        if (g_use_mutex) pthread_mutex_unlock(&lock);
        sem_post(&full);
    }
    return NULL;
}

void *consumer(void *arg)
{
    (void)arg;
    for (int k = 0; k < ITEMS; k++) {
        // TODO 2: Wait on full, lock the mutex, take the item at buffer[out], advance
        // out circularly, unlock, post empty and add the item to consumed_sum
        sem_wait(&full);
        if (g_use_mutex) pthread_mutex_lock(&lock);
        int item = buffer[out];
        out = (out + 1) % SIZE;
        consumed_sum += item;
        if (g_use_mutex) pthread_mutex_unlock(&lock);
        sem_post(&empty);
    }
    return NULL;
}

// Multi-producer / multi-consumer support
#define MULTI_THREADS 4
#define ITEMS_PER_PROD (ITEMS / MULTI_THREADS)

void *multi_producer(void *arg) {
    int id = *(int *)arg;
    int start = id * ITEMS_PER_PROD + 1;
    int end = start + ITEMS_PER_PROD - 1;
    for (int item = start; item <= end; item++) {
        sem_wait(&empty);
        if (g_use_mutex) pthread_mutex_lock(&lock);
        buffer[in] = item;
        in = (in + 1) % SIZE;
        if (g_use_mutex) pthread_mutex_unlock(&lock);
        sem_post(&full);
    }
    return NULL;
}

void *multi_consumer(void *arg) {
    (void)arg;
    for (int k = 0; k < ITEMS_PER_PROD; k++) {
        sem_wait(&full);
        if (g_use_mutex) pthread_mutex_lock(&lock);
        int item = buffer[out];
        out = (out + 1) % SIZE;
        consumed_sum += item;
        if (g_use_mutex) pthread_mutex_unlock(&lock);
        sem_post(&empty);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    int multi_mode = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-multi") == 0) multi_mode = 1;
        if (strcmp(argv[i], "-nomutex") == 0) g_use_mutex = 0;
    }

    sem_init(&empty, 0, SIZE);
    sem_init(&full, 0, 0);

    if (!multi_mode) {
        pthread_t p, c;
        pthread_create(&p, NULL, producer, NULL);
        pthread_create(&c, NULL, consumer, NULL);
        pthread_join(p, NULL);
        pthread_join(c, NULL);
    } else {
        pthread_t prods[MULTI_THREADS], cons[MULTI_THREADS];
        int ids[MULTI_THREADS];
        for (int i = 0; i < MULTI_THREADS; i++) {
            ids[i] = i;
            pthread_create(&prods[i], NULL, multi_producer, &ids[i]);
            pthread_create(&cons[i], NULL, multi_consumer, NULL);
        }
        for (int i = 0; i < MULTI_THREADS; i++) {
            pthread_join(prods[i], NULL);
            pthread_join(cons[i], NULL);
        }
    }

    printf("items consumed: %d, sum = %ld (expected %d)\n", ITEMS, consumed_sum,
           ITEMS * (ITEMS + 1) / 2);
    sem_destroy(&empty);
    sem_destroy(&full);
    return 0;
}
