/**
 * Task 05: Monitor Pattern Simulation with Condition Variables
 * Student Name: S. Sachin (192525045) | CSA04 OS Assessment 2
 * Encapsulates shared state, internal mutex, and condition variables (wait/signal).
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int available_seats;
    pthread_mutex_t lock;
    pthread_cond_t seats_available;
} BookingMonitor;

BookingMonitor g_mon;

void monitor_init(BookingMonitor *m, int initial_seats) {
    m->available_seats = initial_seats;
    pthread_mutex_init(&m->lock, NULL);
    pthread_cond_init(&m->seats_available, NULL);
}

void monitor_destroy(BookingMonitor *m) {
    pthread_mutex_destroy(&m->lock);
    pthread_cond_destroy(&m->seats_available);
}

int monitor_book_seat(BookingMonitor *m, int agent_id) {
    pthread_mutex_lock(&m->lock);
    while (m->available_seats <= 0) {
        printf("[Agent %d] No seats available! Waiting on condition variable...\n", agent_id);
        pthread_cond_wait(&m->seats_available, &m->lock);
    }
    m->available_seats--;
    int booked_id = m->available_seats;
    printf("  [Agent %d] Booked seat successfully! Remaining: %d\n", agent_id, m->available_seats);
    pthread_mutex_unlock(&m->lock);
    return booked_id;
}

void monitor_cancel_seat(BookingMonitor *m, int agent_id) {
    pthread_mutex_lock(&m->lock);
    m->available_seats++;
    printf("  ===> [Agent %d] Cancelled ticket. Signalling condition variable...\n", agent_id);
    pthread_cond_signal(&m->seats_available);
    pthread_mutex_unlock(&m->lock);
}

void *agent_worker(void *arg) {
    int id = *(int *)arg;
    monitor_book_seat(&g_mon, id);
    if (id == 2) {
        // Agent 2 cancels a seat to unblock another agent
        monitor_cancel_seat(&g_mon, id);
    }
    return NULL;
}

int main(void) {
    monitor_init(&g_mon, 3); // Start with only 3 seats
    pthread_t agents[5];
    int ids[5];

    printf("Monitor Pattern Demonstration (5 Agents competing for 3 seats)\n");
    for (int i = 0; i < 5; i++) {
        ids[i] = i + 1;
        pthread_create(&agents[i], NULL, agent_worker, &ids[i]);
    }
    for (int i = 0; i < 5; i++) pthread_join(agents[i], NULL);
    monitor_destroy(&g_mon);

    printf("Monitor simulation completed cleanly.\n");
    return 0;
}
