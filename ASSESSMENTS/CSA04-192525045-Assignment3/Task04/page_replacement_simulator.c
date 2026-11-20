/**
 * page_replacement_simulator.c - Faults vs Number of Frames Simulator
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 3 (CO3)
 *
 * Simulates page fault rate as available frame allocation varies
 * from 2 to 10 frames for a realistic program reference string.
 */

#include <stdio.h>
#include <stdbool.h>

#define MAX_FRAMES 16

int simulate_fifo(const int ref[], int len, int frames) {
    int f[MAX_FRAMES];
    int count = 0, next = 0, faults = 0;

    for (int i = 0; i < len; i++) {
        int page = ref[i];
        bool found = false;
        for (int j = 0; j < count; j++) {
            if (f[j] == page) { found = true; break; }
        }
        if (found) continue;

        faults++;
        if (count < frames) {
            f[count++] = page;
        } else {
            f[next] = page;
            next = (next + 1) % frames;
        }
    }
    return faults;
}

int simulate_lru(const int ref[], int len, int frames) {
    int f[MAX_FRAMES];
    int last[MAX_FRAMES];
    int count = 0, faults = 0;

    for (int i = 0; i < len; i++) {
        int page = ref[i];
        int hit_idx = -1;
        for (int j = 0; j < count; j++) {
            if (f[j] == page) { hit_idx = j; break; }
        }
        if (hit_idx >= 0) {
            last[hit_idx] = i;
            continue;
        }

        faults++;
        if (count < frames) {
            f[count] = page;
            last[count] = i;
            count++;
        } else {
            int lru_idx = 0;
            int oldest = last[0];
            for (int j = 1; j < frames; j++) {
                if (last[j] < oldest) { oldest = last[j]; lru_idx = j; }
            }
            f[lru_idx] = page;
            last[lru_idx] = i;
        }
    }
    return faults;
}

int main(void) {
    printf("==================================================================\n");
    printf("  PAGE FAULTS VS NUMBER OF FRAMES SIMULATOR                      \n");
    printf("  Student: S. Sachin | Reg: 192525045 | CSA04 CO3                \n");
    printf("==================================================================\n\n");

    // Standard reference string exhibiting spatial locality
    int ref[] = {1, 2, 3, 4, 2, 1, 5, 6, 2, 1, 2, 3, 7, 6, 3, 2, 1, 2, 3, 6};
    int len = sizeof(ref) / sizeof(ref[0]);

    printf("Reference String Length: %d references\n", len);
    printf("%-8s | %-12s | %-12s | %s\n", "Frames", "FIFO Faults", "LRU Faults", "Fault Rate Reduction");
    printf("------------------------------------------------------------------\n");

    int prev_lru = 0;
    for (int fr = 2; fr <= 8; fr++) {
        int fifo_f = simulate_fifo(ref, len, fr);
        int lru_f = simulate_lru(ref, len, fr);
        double delta = (prev_lru == 0) ? 0.0 : ((double)(prev_lru - lru_f) / prev_lru) * 100.0;
        printf("%-8d | %-12d | %-12d | -%.1f%%\n", fr, fifo_f, lru_f, delta);
        prev_lru = lru_f;
    }

    printf("------------------------------------------------------------------\n");
    printf("Observation: As allocated frames approach the process's working set size,\n");
    printf("page faults plummet, preventing thrashing.\n");
    printf("==================================================================\n");
    return 0;
}
