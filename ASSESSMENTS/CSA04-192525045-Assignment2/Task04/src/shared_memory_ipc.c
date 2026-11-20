/**
 * Task 04: POSIX Shared Memory IPC with Semaphores
 * Student Name: S. Sachin (192525045) | CSA04 OS Assessment 2
 * Implements zero-copy exchange of weather readings using shm_open, mmap, and semaphores.
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>

#define SHM_NAME "/weather_station_shm"
#define MAX_READINGS 16

typedef struct {
    int readings[MAX_READINGS];
    int count;
    sem_t sem_ready;
    sem_t sem_done;
} SharedData;

int main(void) {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) { perror("shm_open"); exit(1); }
    ftruncate(shm_fd, sizeof(SharedData));

    SharedData *data = (SharedData *)mmap(NULL, sizeof(SharedData),
                                          PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    sem_init(&data->sem_ready, 1, 0);
    sem_init(&data->sem_done, 1, 0);

    pid_t pid = fork();
    if (pid == 0) {
        // Child: Analyser
        sem_wait(&data->sem_ready);
        int sum = 0, max = 0;
        for (int i = 0; i < data->count; i++) {
            int val = data->readings[i];
            if (i == 0 || val > max) max = val;
            sum += val;
        }
        printf("shm-analyser: %d readings, average %.1f, max %d (zero-copy shared memory)\n",
               data->count, data->count ? (double)sum / data->count : 0.0, max);
        sem_post(&data->sem_done);
        munmap(data, sizeof(SharedData));
        _exit(0);
    }

    // Parent: Sensor Reader
    int readings[] = {31, 33, 36, 34, 41, 38};
    data->count = 6;
    for (int i = 0; i < 6; i++) {
        data->readings[i] = readings[i];
    }
    sem_post(&data->sem_ready); // Notify child data is in shared memory

    sem_wait(&data->sem_done);  // Wait for child to finish analysis
    waitpid(pid, NULL, 0);
    printf("shm-reader: done\n");

    sem_destroy(&data->sem_ready);
    sem_destroy(&data->sem_done);
    munmap(data, sizeof(SharedData));
    shm_unlink(SHM_NAME);
    return 0;
}
