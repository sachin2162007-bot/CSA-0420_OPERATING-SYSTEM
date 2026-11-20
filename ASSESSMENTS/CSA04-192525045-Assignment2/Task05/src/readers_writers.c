/**
 * Task 05: Readers-Writers Problem (Seat Inquiry vs Seat Booking)
 * Student Name: S. Sachin (192525045) | CSA04 OS Assessment 2
 * Implements solution with reader count and write mutex to prevent race conditions.
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_READERS 5
#define NUM_WRITERS 2

int g_train_seats = 120;
int g_reader_count = 0;
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t g_wrt;

void *reader_inquiry(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < 3; i++) {
        pthread_mutex_lock(&g_mutex);
        g_reader_count++;
        if (g_reader_count == 1) {
            sem_wait(&g_wrt); // First reader locks writer out
        }
        pthread_mutex_unlock(&g_mutex);

        // Reading critical section
        printf("[READER %d] Inquired seat availability: %d seats remaining\n", id, g_train_seats);

        pthread_mutex_lock(&g_mutex);
        g_reader_count--;
        if (g_reader_count == 0) {
            sem_post(&g_wrt); // Last reader releases lock for writers
        }
        pthread_mutex_unlock(&g_mutex);
    }
    return NULL;
}

void *writer_booking(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < 2; i++) {
        sem_wait(&g_wrt); // Exclusive writer lock
        g_train_seats -= 2;
        printf("  ===> [WRITER %d] Booked 2 tickets! Remaining seats: %d\n", id, g_train_seats);
        sem_post(&g_wrt);
    }
    return NULL;
}

int main(void) {
    sem_init(&g_wrt, 0, 1);
    pthread_t r[NUM_READERS], w[NUM_WRITERS];
    int r_ids[NUM_READERS], w_ids[NUM_WRITERS];

    for (int i = 0; i < NUM_READERS; i++) {
        r_ids[i] = i + 1;
        pthread_create(&r[i], NULL, reader_inquiry, &r_ids[i]);
    }
    for (int i = 0; i < NUM_WRITERS; i++) {
        w_ids[i] = i + 1;
        pthread_create(&w[i], NULL, writer_booking, &w_ids[i]);
    }

    for (int i = 0; i < NUM_READERS; i++) pthread_join(r[i], NULL);
    for (int i = 0; i < NUM_WRITERS; i++) pthread_join(w[i], NULL);

    printf("Readers-Writers simulation complete. Final Seats: %d\n", g_train_seats);
    sem_destroy(&g_wrt);
    return 0;
}
