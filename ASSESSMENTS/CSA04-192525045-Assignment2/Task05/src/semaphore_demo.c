/**
 * Task 05: Binary vs Counting Semaphore Demonstration
 * Student Name: S. Sachin (192525045) | CSA04 OS Assessment 2
 * Shows seat reservation counter protected by binary semaphore,
 * and train coach access bounded by counting semaphore.
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define TOTAL_SEATS 50
#define NUM_AGENTS 10

int g_available_seats = TOTAL_SEATS;
sem_t g_binary_sem;   // Value = 1 (Mutual exclusion)
sem_t g_counting_sem; // Value = 3 (Max 3 concurrent booking transactions)

void *booking_agent(void *arg) {
    int agent_id = *(int *)arg;

    // Counting semaphore bounds concurrent connection to server
    sem_wait(&g_counting_sem);
    printf("[Agent %2d] Connected to booking server (counting sem slot acquired)\n", agent_id);

    // Binary semaphore guarantees mutual exclusion on seat allocation
    sem_wait(&g_binary_sem);
    if (g_available_seats > 0) {
        g_available_seats--;
        printf("  -> [Agent %2d] Booked seat #%d! Remaining seats: %d\n",
               agent_id, TOTAL_SEATS - g_available_seats, g_available_seats);
    }
    sem_post(&g_binary_sem);

    sem_post(&g_counting_sem);
    return NULL;
}

int main(void) {
    pthread_t agents[NUM_AGENTS];
    int agent_ids[NUM_AGENTS];

    sem_init(&g_binary_sem, 0, 1);
    sem_init(&g_counting_sem, 0, 3); // Maximum 3 concurrent server sessions

    printf("Railway Booking Semaphore Demonstration (10 Agents, 50 Seats)\n");
    for (int i = 0; i < NUM_AGENTS; i++) {
        agent_ids[i] = i + 1;
        pthread_create(&agents[i], NULL, booking_agent, &agent_ids[i]);
    }
    for (int i = 0; i < NUM_AGENTS; i++) {
        pthread_join(agents[i], NULL);
    }

    printf("All agents finished. Final Available Seats: %d (Expected: %d)\n",
           g_available_seats, TOTAL_SEATS - NUM_AGENTS);

    sem_destroy(&g_binary_sem);
    sem_destroy(&g_counting_sem);
    return 0;
}
