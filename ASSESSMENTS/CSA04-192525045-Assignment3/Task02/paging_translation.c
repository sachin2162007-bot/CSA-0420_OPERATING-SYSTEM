/**
 * paging_translation.c - Official Page Table Address Translation
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 3 (CO3)
 *
 * Implements logical to physical address translation with page table lookup.
 * Completes TODO 1 from the official Assessment 3 question paper.
 */

#include <stdio.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE 1024
#endif

#define PAGES 8

/* Page table of a process: frame number, or -1 if the page is not in memory. */
int page_table[PAGES] = {5, 2, -1, 7, 0, -1, 3, -1};

int main(void)
{
    unsigned logical[] = {0, 1500, 3100, 6200, 9000, 2049};
    int n = sizeof(logical) / sizeof(logical[0]);

    printf("====================================================\n");
    printf(" PAGING ADDRESS TRANSLATION (PAGE_SIZE = %d)\n", PAGE_SIZE);
    printf(" Student: S. Sachin | Reg: 192525045 | CSA04 CO3\n");
    printf("====================================================\n");

    for (int i = 0; i < n; i++) {
        unsigned a = logical[i];
        // TODO 1: Split a into page number p = a / PAGE_SIZE and offset d = a %
        // PAGE_SIZE. If p is outside the table print "invalid address"; if the frame
        // is -1 print "page fault"; otherwise print the physical address frame *
        // PAGE_SIZE + d
        unsigned p = a / PAGE_SIZE;
        unsigned d = a % PAGE_SIZE;

        if (p >= PAGES) {
            printf("%u -> page %u offset %u : invalid address\n", a, p, d);
        } else if (page_table[p] == -1) {
            printf("%u -> page %u offset %u : page fault\n", a, p, d);
        } else {
            unsigned physical = (unsigned)page_table[p] * PAGE_SIZE + d;
            printf("%u -> page %u offset %u : physical %u\n", a, p, d, physical);
        }
    }

    return 0;
}
