/**
 * random_memory.c - Random Stride Memory Access Pattern Demonstration
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 3 (CO3)
 *
 * Demonstrates zero spatial locality.
 * Allocates a safe, bounded buffer (32 MB) and touches pages using large pseudo-random
 * page-crossing strides.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>

#define BUFFER_SIZE_MB 32
#define PAGE_SIZE_BYTES 4096
#define NUM_PAGES ((BUFFER_SIZE_MB * 1024 * 1024) / PAGE_SIZE_BYTES)

void print_fault_stats(const char* label) {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("[%s] Minor (Soft) Faults: %ld | Major (Hard) Faults: %ld | Max RSS: %ld KB\n",
           label, usage.ru_minflt, usage.ru_majflt, usage.ru_maxrss);
}

int main(void) {
    printf("==================================================================\n");
    printf("  RANDOM MEMORY ACCESS PATTERN (32 MB BUFFER - PAGE STRIDING)     \n");
    printf("  Student: S. Sachin | Reg: 192525045 | CSA04 CO3                \n");
    printf("==================================================================\n");

    print_fault_stats("Initial State");

    printf("\nAllocating 32 MB virtual memory via malloc()...\n");
    char *buffer = (char*)malloc(BUFFER_SIZE_MB * 1024 * 1024);
    if (!buffer) {
        perror("malloc failed");
        return 1;
    }
    print_fault_stats("After malloc (Lazy Allocation)");

    printf("\nAccessing pages randomly across 8,192 pages...\n");
    struct timeval t1, t2;
    gettimeofday(&t1, NULL);

    // Large prime stride to scatter access across non-contiguous pages
    size_t stride = 4099;
    size_t idx = 0;
    for (size_t i = 0; i < NUM_PAGES; i++) {
        idx = (idx + stride) % NUM_PAGES;
        buffer[idx * PAGE_SIZE_BYTES] = (char)(i & 0xFF);
    }

    gettimeofday(&t2, NULL);
    double elapsed_ms = (t2.tv_sec - t1.tv_sec) * 1000.0 + (t2.tv_usec - t1.tv_usec) / 1000.0;

    print_fault_stats("After Random Access");
    printf("Random Access Pass Duration: %.3f ms\n", elapsed_ms);
    printf("Observation: Random page striding triggers immediate page allocations\n");
    printf("with high TLB miss rates compared to linear streaming.\n");

    free(buffer);
    return 0;
}
