/**
 * sequential_memory.c - Sequential Memory Access Pattern Demonstration
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 3 (CO3)
 *
 * Demonstrates high spatial and temporal locality.
 * Allocates a safe, bounded buffer (32 MB) and accesses memory sequentially.
 * Measures minor and major page faults via getrusage().
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>

#define BUFFER_SIZE_MB 32
#define NUM_INTS ((BUFFER_SIZE_MB * 1024 * 1024) / sizeof(int))

void print_fault_stats(const char* label) {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("[%s] Minor (Soft) Faults: %ld | Major (Hard) Faults: %ld | Max RSS: %ld KB\n",
           label, usage.ru_minflt, usage.ru_majflt, usage.ru_maxrss);
}

int main(void) {
    printf("==================================================================\n");
    printf("  SEQUENTIAL MEMORY ACCESS PATTERN (32 MB BUFFER)                \n");
    printf("  Student: S. Sachin | Reg: 192525045 | CSA04 CO3                \n");
    printf("==================================================================\n");

    print_fault_stats("Initial State");

    printf("\nAllocating 32 MB virtual buffer via calloc()...\n");
    int *buffer = (int*)calloc(NUM_INTS, sizeof(int));
    if (!buffer) {
        perror("calloc failed");
        return 1;
    }
    print_fault_stats("After calloc (Virtual Allocated)");

    printf("\nAccessing memory sequentially (Writing linear values)...\n");
    struct timeval t1, t2;
    gettimeofday(&t1, NULL);

    for (size_t i = 0; i < NUM_INTS; i++) {
        buffer[i] = (int)(i ^ 0x55AA);
    }

    gettimeofday(&t2, NULL);
    double elapsed_ms = (t2.tv_sec - t1.tv_sec) * 1000.0 + (t2.tv_usec - t1.tv_usec) / 1000.0;

    print_fault_stats("After Sequential Access");
    printf("Sequential Pass Duration: %.3f ms\n", elapsed_ms);

    free(buffer);
    printf("Buffer safely freed. Sequential test completed successfully.\n");
    return 0;
}
