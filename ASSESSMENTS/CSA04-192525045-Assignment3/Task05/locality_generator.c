/**
 * locality_generator.c - Trace Generator for Locality & Random Access
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 3 (CO3)
 *
 * Generates synthetic reference traces:
 *  1. Strong Locality (80/20 Pareto rule with drifting working set)
 *  2. Uniform Random Access (Zero locality baseline)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define TRACE_LENGTH 100000
#define VIRTUAL_PAGES 100

void generate_locality_trace(const char* filename, int length) {
    FILE* f = fopen(filename, "w");
    if (!f) { perror("fopen locality"); return; }

    uint32_t seed = 192525045; // Deterministic student seed
    int hot_pages[10] = {5, 6, 7, 8, 9, 10, 11, 12, 13, 14};

    for (int i = 0; i < length; i++) {
        seed = seed * 1664525 + 1013904223;
        int r = (seed >> 16) % 100;
        int page;
        if (r < 80) {
            // 80% of accesses target the 10 hot working set pages
            page = hot_pages[r % 10];
        } else {
            // 20% scattered across the entire 100-page address space
            page = (seed >> 8) % VIRTUAL_PAGES;
        }
        fprintf(f, "%d\n", page);

        // Periodic working set drift every 10,000 references
        if (i % 10000 == 0) {
            int shift = (i / 10000) * 5;
            for (int k = 0; k < 10; k++) hot_pages[k] = (5 + shift + k) % VIRTUAL_PAGES;
        }
    }
    fclose(f);
    printf("Generated locality trace: %s (%d references)\n", filename, length);
}

void generate_random_trace(const char* filename, int length) {
    FILE* f = fopen(filename, "w");
    if (!f) { perror("fopen random"); return; }

    uint32_t seed = 987654321;
    for (int i = 0; i < length; i++) {
        seed = seed * 1664525 + 1013904223;
        int page = (seed >> 16) % VIRTUAL_PAGES;
        fprintf(f, "%d\n", page);
    }
    fclose(f);
    printf("Generated random trace:   %s (%d references)\n", filename, length);
}

int main(void) {
    printf("==================================================================\n");
    printf("  PAGE REFERENCE TRACE GENERATOR (100,000 REFERENCES)            \n");
    printf("  Student: S. Sachin | Reg: 192525045 | CSA04 CO3                \n");
    printf("==================================================================\n");
    generate_locality_trace("data/locality_trace.txt", TRACE_LENGTH);
    generate_random_trace("data/random_trace.txt", TRACE_LENGTH);
    return 0;
}
