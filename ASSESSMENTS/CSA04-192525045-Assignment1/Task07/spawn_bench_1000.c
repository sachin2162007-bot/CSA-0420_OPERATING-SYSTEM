#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

/*
 * Task 7 Alternative Concept Benchmark:
 * Measures time to start and wait for 1,000 child processes.
 * Runs 3 iterations and calculates the median time.
 * Course: CSA04 - Operating Systems
 * Student: S. Sachin (Reg: 192525045)
 */

#define ITERATIONS 1000
#define RUNS 3

double run_benchmark(void)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < ITERATIONS; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        }
        if (pid == 0) {
            _exit(0);
        }
        int status;
        waitpid(pid, &status, 0);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int compare_doubles(const void *a, const void *b)
{
    double da = *(const double *)a;
    double db = *(const double *)b;
    if (da < db) return -1;
    if (da > db) return 1;
    return 0;
}

int main(void)
{
    printf("====================================================\n");
    printf("  PROCESS CREATION BENCHMARK (1,000 fork+waitpid)\n");
    printf("  Student: S. Sachin | Reg No: 192525045\n");
    printf("====================================================\n\n");

    double times[RUNS];
    for (int r = 0; r < RUNS; r++) {
        times[r] = run_benchmark();
        printf("Run %d: %.6f seconds (%.2f us per process)\n",
               r + 1, times[r], (times[r] / ITERATIONS) * 1e6);
    }

    qsort(times, RUNS, sizeof(double), compare_doubles);
    double median = times[RUNS / 2];

    printf("\nMedian Execution Time: %.6f seconds\n", median);
    printf("Median Latency per Process: %.2f us\n", (median / ITERATIONS) * 1e6);
    printf("Throughput: %.2f process spawns/second\n", ITERATIONS / median);

    return 0;
}
