/**
 * Task 06: Banker's Algorithm Scalability Benchmark (10, 100, 1,000 processes)
 * Student Name: S. Sachin (192525045) | CSA04 OS Assessment 2
 * Evaluates execution time growth against theoretical O(m * n^2) complexity.
 */

#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define R 3

int benchmark_bankers(int n) {
    int *avail = malloc(R * sizeof(int));
    int **maxm = malloc(n * sizeof(int *));
    int **alloc = malloc(n * sizeof(int *));
    int **need = malloc(n * sizeof(int *));
    int *work = malloc(R * sizeof(int));
    int *finish = malloc(n * sizeof(int));
    int *seq = malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        maxm[i] = malloc(R * sizeof(int));
        alloc[i] = malloc(R * sizeof(int));
        need[i] = malloc(R * sizeof(int));
    }

    avail[0] = 50; avail[1] = 50; avail[2] = 50;
    for (int i = 0; i < n; i++) {
        for (int r = 0; r < R; r++) {
            alloc[i][r] = 1;
            maxm[i][r] = 2;
            need[i][r] = maxm[i][r] - alloc[i][r];
        }
        finish[i] = 0;
    }

    memcpy(work, avail, R * sizeof(int));
    int count = 0;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    while (count < n) {
        int found = 0;
        for (int p = 0; p < n; p++) {
            if (finish[p]) continue;
            int can_finish = 1;
            for (int r = 0; r < R; r++) {
                if (need[p][r] > work[r]) {
                    can_finish = 0;
                    break;
                }
            }
            if (can_finish) {
                for (int r = 0; r < R; r++) work[r] += alloc[p][r];
                finish[p] = 1;
                seq[count++] = p;
                found = 1;
            }
        }
        if (!found) break;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed_us = ((end.tv_sec - start.tv_sec) * 1e6) + ((end.tv_nsec - start.tv_nsec) / 1e3);

    printf("  Processes: %4d | Time: %9.2f us | Safe: %s\n",
           n, elapsed_us, (count == n) ? "YES" : "NO");

    for (int i = 0; i < n; i++) {
        free(maxm[i]); free(alloc[i]); free(need[i]);
    }
    free(maxm); free(alloc); free(need); free(avail);
    free(work); free(finish); free(seq);
    return 0;
}

int main(void) {
    printf("=======================================================================\n");
    printf(" BANKER'S ALGORITHM SCALABILITY BENCHMARK - O(m * n^2)\n");
    printf(" Student: S. Sachin (192525045) | Assessment 2 (CO2)\n");
    printf(" Measuring Safety Algorithm for n = 10, 100, 1,000 processes (m = 3)\n");
    printf("=======================================================================\n\n");

    benchmark_bankers(10);
    benchmark_bankers(100);
    benchmark_bankers(1000);

    return 0;
}
