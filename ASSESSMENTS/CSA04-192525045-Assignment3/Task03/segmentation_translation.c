/**
 * segmentation_translation.c - Segment Table Address Translation Engine
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 3 (CO3)
 *
 * Simulates hardware Memory Management Unit (MMU) segmentation translation:
 *   Logical Address = (Segment Number s, Offset d)
 *   Check: If d >= Limit -> Hardware Trap: Segmentation Fault (Limit Violation)
 *   Physical Address = Base + d
 */

#include <stdio.h>
#include <stdbool.h>

#define MAX_SEGMENTS 5

typedef struct {
    int segment_id;
    const char* name;
    unsigned int base;
    unsigned int limit;
    char permissions[4]; // rwx
} SegmentEntry;

SegmentEntry segment_table[MAX_SEGMENTS] = {
    {0, "Code Segment (.text)",         1400, 1000, "r-x"},
    {1, "Data Segment (.data/.bss)",    6300,  400, "rw-"},
    {2, "Heap Segment",                 4300,  500, "rw-"},
    {3, "Stack Segment",                3200, 1100, "rw-"},
    {4, "Shared Library Segment",       4700, 1000, "r-x"}
};

void print_segment_table(void) {
    printf("====================================================================\n");
    printf(" SEGMENT TABLE LAYOUT                                               \n");
    printf("====================================================================\n");
    printf("%-4s | %-26s | %-8s | %-8s | %-6s\n", "Seg#", "Segment Name", "Base", "Limit", "Perms");
    printf("--------------------------------------------------------------------\n");
    for (int i = 0; i < MAX_SEGMENTS; i++) {
        printf("%-4d | %-26s | %8u | %8u | %-6s\n",
               segment_table[i].segment_id, segment_table[i].name,
               segment_table[i].base, segment_table[i].limit, segment_table[i].permissions);
    }
    printf("====================================================================\n\n");
}

bool translate_segment_address(int s, unsigned int d, unsigned int* physical_out) {
    if (s < 0 || s >= MAX_SEGMENTS) {
        printf("Translation Failed: Segment %d does not exist! [INVALID SEGMENT ID]\n", s);
        return false;
    }

    SegmentEntry seg = segment_table[s];
    if (d >= seg.limit) {
        printf("Hardware Trap: Address (%d, %u) -> Offset %u exceeds Limit %u! [SEGMENTATION FAULT]\n",
               s, d, d, seg.limit);
        return false;
    }

    *physical_out = seg.base + d;
    printf("Success: (%d, %u) in %-18s -> Base %u + Offset %u = Physical Address %u\n",
           s, d, seg.name, seg.base, d, *physical_out);
    return true;
}

int main(void) {
    printf("====================================================================\n");
    printf(" SEGMENTATION ADDRESS TRANSLATION DEMONSTRATION                     \n");
    printf(" Student: S. Sachin | Reg: 192525045 | CSA04 CO3                    \n");
    printf("====================================================================\n\n");

    print_segment_table();

    struct { int s; unsigned int d; } queries[] = {
        {0, 450},   // Inside Code segment
        {0, 1050},  // Exceeds Code limit (1000)
        {1, 0},     // Base of Data segment
        {1, 399},   // Last valid byte of Data segment
        {1, 400},   // Exceeds Data limit (400)
        {2, 250},   // Inside Heap segment
        {3, 800},   // Inside Stack segment
        {5, 100}    // Invalid Segment ID
    };

    int n = sizeof(queries) / sizeof(queries[0]);
    unsigned int phys = 0;
    for (int i = 0; i < n; i++) {
        translate_segment_address(queries[i].s, queries[i].d, &phys);
    }

    return 0;
}
