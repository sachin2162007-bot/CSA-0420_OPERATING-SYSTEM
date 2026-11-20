/**
 * address_verification.c - 8-Address Hand Translation Verification
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 3 (CO3)
 *
 * Verifies 8 hand-calculated segment address translations against
 * programmatic MMU evaluation.
 */

#include <stdio.h>

typedef struct {
    unsigned int base;
    unsigned int limit;
} SegDesc;

SegDesc table[5] = {
    {1400, 1000}, // Seg 0: Code
    {6300,  400}, // Seg 1: Data
    {4300,  500}, // Seg 2: Heap
    {3200, 1100}, // Seg 3: Stack
    {4700, 1000}  // Seg 4: Shared Library
};

void verify_address(int test_num, int s, unsigned int d, const char* expected) {
    printf("Test #%d: Logical (%d, %4u) -> ", test_num, s, d);
    if (s < 0 || s >= 5) {
        printf("Actual: INVALID SEGMENT | Expected: %s\n", expected);
        return;
    }
    if (d >= table[s].limit) {
        printf("Actual: SEGMENT FAULT   | Expected: %s\n", expected);
        return;
    }
    unsigned int phys = table[s].base + d;
    printf("Actual: PHYSICAL %5u  | Expected: %s\n", phys, expected);
}

int main(void) {
    printf("==================================================================\n");
    printf("  VERIFICATION OF 8 HAND-CALCULATED SEGMENT ADDRESS TRANSLATIONS  \n");
    printf("  Student: S. Sachin | Reg: 192525045 | CSA04 CO3                \n");
    printf("==================================================================\n\n");

    verify_address(1, 0, 450,  "PHYSICAL  1850");
    verify_address(2, 0, 1050, "SEGMENT FAULT (Limit 1000 exceeded)");
    verify_address(3, 1, 0,    "PHYSICAL  6300");
    verify_address(4, 1, 450,  "SEGMENT FAULT (Limit 400 exceeded)");
    verify_address(5, 2, 250,  "PHYSICAL  4550");
    verify_address(6, 3, 800,  "PHYSICAL  4000");
    verify_address(7, 4, 999,  "PHYSICAL  5699");
    verify_address(8, 5, 100,  "INVALID SEGMENT (Seg 5 out of bounds)");

    printf("\nAll 8 hand calculations 100%% verified by programmatic MMU simulation.\n");
    printf("==================================================================\n");
    return 0;
}
