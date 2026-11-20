/**
 * file_allocation.c - File Allocation Simulator (Contiguous First-Fit vs Indexed)
 * Course: CSA04 - Operating Systems | Assessment 4 (CO4)
 * Student Name: S. Sachin | Register Number: 192525045 | SIMATS Engineering
 *
 * Description:
 *   Demonstrates file allocation on a 32-block simulated storage volume.
 *   [RESOLVED] Completed assignment requirement implementing first-fit contiguous allocation.
 *   Followed by indexed allocation demonstrating block aggregation across non-contiguous holes.
 *
 * Compilation:
 *   gcc -std=c11 -Wall -Wextra file_allocation.c -o file_allocation
 *
 * Execution:
 *   ./file_allocation
 */

#include <stdio.h>
#include <string.h>

#define BLOCKS 32

char disk[BLOCKS]; /* '.' = free, otherwise the file letter */

/* Contiguous allocation, first fit: returns the start block or -1. */
int alloc_contiguous(char file, int size)
{
    // [RESOLVED] Scan the disk for the first run of size consecutive free blocks; mark
    // them with the file letter and return the start block, or return -1 if no run is
    // long enough
    int run = 0;
    int start = -1;

    for (int b = 0; b < BLOCKS; b++) {
        if (disk[b] == '.') {
            if (run == 0) {
                start = b;
            }
            run++;
            if (run == size) {
                for (int i = start; i < start + size; i++) {
                    disk[i] = file;
                }
                return start;
            }
        } else {
            run = 0;
            start = -1;
        }
    }
    return -1;
}

/* Indexed allocation: one index block plus any free data blocks. */
int alloc_indexed(char file, int size, int index[])
{
    int got = 0, idx = -1;
    for (int b = 0; b < BLOCKS && got <= size; b++) {
        if (disk[b] != '.') continue;
        if (idx < 0) idx = b; /* first free block holds the index */
        else index[got++] = b;
        if (got == size) break;
    }
    if (idx < 0 || got < size) return -1;
    disk[idx] = (char)(file + 32); /* lowercase letter = index block */
    for (int i = 0; i < size; i++) disk[index[i]] = file;
    return idx;
}

void release(char file)
{
    for (int b = 0; b < BLOCKS; b++) if (disk[b] == file) disk[b] = '.';
}

int main(void)
{
    memset(disk, '.', BLOCKS);
    alloc_contiguous('A', 6);
    alloc_contiguous('B', 8);
    alloc_contiguous('C', 5);
    alloc_contiguous('D', 7);
    release('B');
    release('D');

    printf("%.32s\n", disk);
    printf("E (10 blocks) contiguous: start %d\n", alloc_contiguous('E', 10));

    int index[16];
    int ib = alloc_indexed('E', 10, index);
    printf("E (10 blocks) indexed: index block %d, data blocks", ib);
    for (int i = 0; i < 10; i++) printf(" %d", index[i]);
    printf("\n%.32s\n", disk);

    return 0;
}
