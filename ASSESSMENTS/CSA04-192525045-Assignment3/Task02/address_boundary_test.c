/**
 * address_boundary_test.c - Boundary Condition and Edge Case Verification
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 3 (CO3)
 *
 * Verifies address translation behavior at:
 *  1. Start of page (offset 0)
 *  2. Middle of page
 *  3. End of page (offset = PAGE_SIZE - 1)
 *  4. Across page boundary transition (PAGE_SIZE - 1 to PAGE_SIZE)
 *  5. Highest valid address of highest valid page
 *  6. Out-of-bounds address (page >= PAGES)
 */

#include <stdio.h>

#define PAGE_SIZE 1024
#define PAGES 8

int page_table[PAGES] = {5, 2, -1, 7, 0, -1, 3, -1};

void translate(unsigned addr, const char* desc) {
    unsigned p = addr / PAGE_SIZE;
    unsigned d = addr % PAGE_SIZE;

    printf("Testing [%-26s]: Addr %5u -> Page %2u, Offset %4u : ", desc, addr, p, d);
    if (p >= PAGES) {
        printf("RESULT: [INVALID ADDRESS (Out of Bounds)]\n");
    } else if (page_table[p] == -1) {
        printf("RESULT: [PAGE FAULT (Page Not Present in Memory)]\n");
    } else {
        unsigned phys = (unsigned)page_table[p] * PAGE_SIZE + d;
        printf("RESULT: [PHYSICAL ADDRESS %5u (Frame %d)]\n", phys, page_table[p]);
    }
}

int main(void) {
    printf("====================================================================\n");
    printf("  PAGE ADDRESS BOUNDARY CONDITION AND FAULT VERIFICATION           \n");
    printf("  Student: S. Sachin | Reg: 192525045 | CSA04 CO3                  \n");
    printf("  Configuration: PAGE_SIZE = %d B | Total Pages = %d               \n", PAGE_SIZE, PAGES);
    printf("====================================================================\n");

    translate(0, "Start of Page 0");
    translate(1023, "End of Page 0 (Max Offset)");
    translate(1024, "Start of Page 1 (Transition)");
    translate(2047, "End of Page 1");
    translate(2048, "Start of Page 2 (Unmapped)");
    translate(3071, "End of Page 2 (Unmapped)");
    translate(3072, "Start of Page 3 (Present)");
    translate(7167, "End of Page 6 (Last Valid)");
    translate(7168, "Start of Page 7 (Unmapped)");
    translate(8191, "End of Virtual Space");
    translate(8192, "Beyond Virtual Space");
    translate(65535, "Wild Out-of-Range Offset");

    printf("====================================================================\n");
    return 0;
}
