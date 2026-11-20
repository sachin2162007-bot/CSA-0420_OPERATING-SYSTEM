/**
 * Task 05: Dining Philosophers Problem with Resource Ordering
 * Student Name: S. Sachin (192525045) | CSA04 OS Assessment 2
 * Prevents circular wait deadlock using Dijkstra's asymmetric resource ordering.
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define N 5

pthread_mutex_t forks[N];

void *philosopher(void *arg) {
    int id = *(int *)arg;
    int left = id;
    int right = (id + 1) % N;

    // Asymmetric pickup: Odd philosophers pick right then left; even pick left then right
    int first = (id % 2 == 0) ? left : right;
    int second = (id % 2 == 0) ? right : left;

    for (int i = 0; i < 2; i++) {
        printf("[Philosopher %d] Thinking...\n", id);
        
        pthread_mutex_lock(&forks[first]);
        pthread_mutex_lock(&forks[second]);

        printf("  [Philosopher %d] Eating with forks %d and %d!\n", id, first, second);

        pthread_mutex_unlock(&forks[second]);
        pthread_mutex_unlock(&forks[first]);
    }
    return NULL;
}

int main(void) {
    pthread_t phils[N];
    int ids[N];

    for (int i = 0; i < N; i++) pthread_mutex_init(&forks[i], NULL);
    for (int i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&phils[i], NULL, philosopher, &ids[i]);
    }
    for (int i = 0; i < N; i++) pthread_join(phils[i], NULL);
    for (int i = 0; i < N; i++) pthread_mutex_destroy(&forks[i]);

    printf("Dining Philosophers simulation complete without deadlock.\n");
    return 0;
}
