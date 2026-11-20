#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <time.h>

/*
 * Task 10: Process vs Thread Performance Benchmark
 * Creates 1,000 processes and 1,000 threads.
 * Measures wall-clock time, user CPU time, and system CPU time.
 * Course: CSA04 - Operating Systems
 * Student: S. Sachin (Reg: 192525045)
 */

#define COUNT 1000

void *thread_worker(void *arg)
{
    (void)arg;
    pthread_exit(NULL);
}

void bench_processes(double *wall_sec, double *user_sec, double *sys_sec)
{
    struct timespec start, end;
    struct rusage ru_start, ru_end;

    getrusage(RUSAGE_SELF, &ru_start);
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < COUNT; i++) {
        pid_t pid = fork();
        if (pid < 0) { perror("fork"); exit(1); }
        if (pid == 0) {
            _exit(0);
        }
        int status;
        waitpid(pid, &status, 0);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    getrusage(RUSAGE_SELF, &ru_end);

    *wall_sec = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    *user_sec = (ru_end.ru_utime.tv_sec - ru_start.ru_utime.tv_sec) +
                (ru_end.ru_utime.tv_usec - ru_start.ru_utime.tv_usec) / 1e6;
    *sys_sec  = (ru_end.ru_stime.tv_sec - ru_start.ru_stime.tv_sec) +
                (ru_end.ru_stime.tv_usec - ru_start.ru_stime.tv_usec) / 1e6;
}

void bench_threads(double *wall_sec, double *user_sec, double *sys_sec)
{
    struct timespec start, end;
    struct rusage ru_start, ru_end;

    getrusage(RUSAGE_SELF, &ru_start);
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < COUNT; i++) {
        pthread_t t;
        if (pthread_create(&t, NULL, thread_worker, NULL) != 0) {
            perror("pthread_create");
            exit(1);
        }
        pthread_join(t, NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    getrusage(RUSAGE_SELF, &ru_end);

    *wall_sec = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    *user_sec = (ru_end.ru_utime.tv_sec - ru_start.ru_utime.tv_sec) +
                (ru_end.ru_utime.tv_usec - ru_start.ru_utime.tv_usec) / 1e6;
    *sys_sec  = (ru_end.ru_stime.tv_sec - ru_start.ru_stime.tv_sec) +
                (ru_end.ru_stime.tv_usec - ru_start.ru_stime.tv_usec) / 1e6;
}

int main(int argc, char *argv[])
{
    int runs = 3;
    if (argc > 1) runs = atoi(argv[1]);
    if (runs < 1) runs = 1;

    printf("===================================================================\n");
    printf("  TASK 10: PROCESS VS THREAD CREATION BENCHMARK (N=%d)\n", COUNT);
    printf("  Student: S. Sachin | Reg: 192525045 | Course: CSA04\n");
    printf("===================================================================\n\n");

    double proc_wall[runs], proc_user[runs], proc_sys[runs];
    double thrd_wall[runs], thrd_user[runs], thrd_sys[runs];

    for (int r = 0; r < runs; r++) {
        printf("--- Benchmark Run %d of %d ---\n", r + 1, runs);
        bench_processes(&proc_wall[r], &proc_user[r], &proc_sys[r]);
        printf("[Process] 1,000 fork()+waitpid(): Wall: %.6fs | User: %.6fs | Sys: %.6fs (%.2fus/proc)\n",
               proc_wall[r], proc_user[r], proc_sys[r], (proc_wall[r]/COUNT)*1e6);

        bench_threads(&thrd_wall[r], &thrd_user[r], &thrd_sys[r]);
        printf("[Thread ] 1,000 pthread_create()+join(): Wall: %.6fs | User: %.6fs | Sys: %.6fs (%.2fus/thrd)\n\n",
               thrd_wall[r], thrd_user[r], thrd_sys[r], (thrd_wall[r]/COUNT)*1e6);
    }

    if (runs >= 3) {
        printf("===================================================================\n");
        printf("  SUMMARY ACROSS 3 RUNS\n");
        printf("===================================================================\n");
        printf("Workload       | Run 1 (s) | Run 2 (s) | Run 3 (s) | Median (s) | Per-Op Latency\n");
        printf("---------------+-----------+-----------+-----------+------------+----------------\n");
        // Sort for median
        double pw[3] = {proc_wall[0], proc_wall[1], proc_wall[2]};
        double tw[3] = {thrd_wall[0], thrd_wall[1], thrd_wall[2]};
        // Simple 3-element sort
        for(int i=0;i<2;i++) for(int j=i+1;j<3;j++) {
            if(pw[i]>pw[j]) { double t=pw[i]; pw[i]=pw[j]; pw[j]=t; }
            if(tw[i]>tw[j]) { double t=tw[i]; tw[i]=tw[j]; tw[j]=t; }
        }
        printf("1,000 Processes|  %.6f |  %.6f |  %.6f |  %.6f  | %.2f us/op\n",
               proc_wall[0], proc_wall[1], proc_wall[2], pw[1], (pw[1]/COUNT)*1e6);
        printf("1,000 Threads  |  %.6f |  %.6f |  %.6f |  %.6f  | %.2f us/op\n",
               thrd_wall[0], thrd_wall[1], thrd_wall[2], tw[1], (tw[1]/COUNT)*1e6);
        printf("Speedup factor (Process Wall Time / Thread Wall Time): %.2fx\n", pw[1] / tw[1]);
    }

    return 0;
}
