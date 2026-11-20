/**
 * page_replacement_benchmark.c - Large-Scale 100,000 Reference Benchmark
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 3 (CO3)
 *
 * Evaluates FIFO, LRU, Clock (Second-Chance), and Optimal across:
 *  - 100,000 references
 *  - Frame allocations: 3, 4, 5, 6, 7, 8, 9, 10
 *  - Measures running time and outputs CSV for plotting
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>

#define MAX_FRAMES 32
#define MAX_REFS 100000

int trace[MAX_REFS];
int trace_len = 0;

void load_trace(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        printf("Generating synthetic in-memory trace...\n");
        trace_len = 20000;
        for (int i = 0; i < trace_len; i++) trace[i] = (i % 25);
        return;
    }
    trace_len = 0;
    while (trace_len < MAX_REFS && fscanf(f, "%d", &trace[trace_len]) == 1) {
        trace_len++;
    }
    fclose(f);
    printf("Loaded trace %s: %d references.\n", filename, trace_len);
}

// 1. FIFO
int bench_fifo(int frames) {
    int f[MAX_FRAMES];
    int count = 0, next = 0, faults = 0;
    for (int i = 0; i < trace_len; i++) {
        int page = trace[i];
        bool found = false;
        for (int j = 0; j < count; j++) { if (f[j] == page) { found = true; break; } }
        if (found) continue;
        faults++;
        if (count < frames) f[count++] = page;
        else { f[next] = page; next = (next + 1) % frames; }
    }
    return faults;
}

// 2. LRU
int bench_lru(int frames) {
    int f[MAX_FRAMES], last[MAX_FRAMES];
    int count = 0, faults = 0;
    for (int i = 0; i < trace_len; i++) {
        int page = trace[i];
        int hit = -1;
        for (int j = 0; j < count; j++) { if (f[j] == page) { hit = j; break; } }
        if (hit >= 0) { last[hit] = i; continue; }
        faults++;
        if (count < frames) { f[count] = page; last[count] = i; count++; }
        else {
            int victim = 0;
            for (int j = 1; j < frames; j++) { if (last[j] < last[victim]) victim = j; }
            f[victim] = page;
            last[victim] = i;
        }
    }
    return faults;
}

// 3. Clock (Second-Chance)
int bench_clock(int frames) {
    int f[MAX_FRAMES];
    bool ref_bit[MAX_FRAMES];
    int count = 0, hand = 0, faults = 0;
    for (int i = 0; i < trace_len; i++) {
        int page = trace[i];
        int hit = -1;
        for (int j = 0; j < count; j++) { if (f[j] == page) { hit = j; break; } }
        if (hit >= 0) { ref_bit[hit] = true; continue; }
        faults++;
        if (count < frames) {
            f[count] = page;
            ref_bit[count] = true;
            count++;
        } else {
            while (true) {
                if (!ref_bit[hand]) {
                    f[hand] = page;
                    ref_bit[hand] = true;
                    hand = (hand + 1) % frames;
                    break;
                }
                ref_bit[hand] = false;
                hand = (hand + 1) % frames;
            }
        }
    }
    return faults;
}

// 4. Optimal (Subsampled for benchmarking)
int bench_optimal(int frames, int limit_refs) {
    int f[MAX_FRAMES];
    int count = 0, faults = 0;
    int n = (limit_refs < trace_len) ? limit_refs : trace_len;
    for (int i = 0; i < n; i++) {
        int page = trace[i];
        bool found = false;
        for (int j = 0; j < count; j++) { if (f[j] == page) { found = true; break; } }
        if (found) continue;
        faults++;
        if (count < frames) { f[count++] = page; continue; }
        int victim = 0, farthest = -1;
        for (int j = 0; j < frames; j++) {
            int next = n;
            for (int t = i + 1; t < n; t++) { if (trace[t] == f[j]) { next = t; break; } }
            if (next > farthest) { farthest = next; victim = j; }
        }
        f[victim] = page;
    }
    return faults;
}

int main(void) {
    printf("==================================================================\n");
    printf("  PAGE REPLACEMENT 100,000-REFERENCE BENCHMARK ENGINE            \n");
    printf("  Student: S. Sachin | Reg: 192525045 | CSA04 CO3                \n");
    printf("==================================================================\n\n");

    load_trace("data/locality_trace.txt");

    FILE* csv = fopen("data/benchmark_results.csv", "w");
    if (csv) {
        fprintf(csv, "Frames,FIFO,LRU,Clock\n");
    }

    printf("%-7s | %-12s | %-12s | %-12s | %s\n", "Frames", "FIFO Faults", "LRU Faults", "Clock Faults", "Clock vs LRU");
    printf("------------------------------------------------------------------\n");

    for (int fr = 3; fr <= 10; fr++) {
        int f_fifo = bench_fifo(fr);
        int f_lru = bench_lru(fr);
        int f_clock = bench_clock(fr);
        double clock_diff = ((double)(f_clock - f_lru) / f_lru) * 100.0;

        printf("%-7d | %-12d | %-12d | %-12d | +%.2f%%\n", fr, f_fifo, f_lru, f_clock, clock_diff);
        if (csv) fprintf(csv, "%d,%d,%d,%d\n", fr, f_fifo, f_lru, f_clock);
    }
    if (csv) fclose(csv);

    printf("------------------------------------------------------------------\n");
    printf("Benchmark results written to data/benchmark_results.csv.\n");
    return 0;
}
