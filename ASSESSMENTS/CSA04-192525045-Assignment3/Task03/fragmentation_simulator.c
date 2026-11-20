/**
 * fragmentation_simulator.c - External Fragmentation in Segmentation
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 3 (CO3)
 *
 * Simulates dynamic contiguous segment allocation and deallocation across
 * 20 segment operations of random sizes (10 KB to 100 KB) in a 1 MB physical memory pool.
 * Demonstrates the formation of external fragmentation holes and compaction cost.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define TOTAL_MEMORY_KB 1024
#define NUM_OPS 20

typedef struct Block {
    int seg_id;
    int base_kb;
    int size_kb;
    bool is_free;
    struct Block* next;
} Block;

Block* memory_list = NULL;

void init_memory(void) {
    memory_list = (Block*)malloc(sizeof(Block));
    memory_list->seg_id = -1;
    memory_list->base_kb = 0;
    memory_list->size_kb = TOTAL_MEMORY_KB;
    memory_list->is_free = true;
    memory_list->next = NULL;
}

bool allocate_first_fit(int seg_id, int size_kb) {
    Block* curr = memory_list;
    while (curr) {
        if (curr->is_free && curr->size_kb >= size_kb) {
            if (curr->size_kb > size_kb) {
                // Split block
                Block* new_block = (Block*)malloc(sizeof(Block));
                new_block->seg_id = -1;
                new_block->base_kb = curr->base_kb + size_kb;
                new_block->size_kb = curr->size_kb - size_kb;
                new_block->is_free = true;
                new_block->next = curr->next;

                curr->next = new_block;
                curr->size_kb = size_kb;
            }
            curr->seg_id = seg_id;
            curr->is_free = false;
            return true;
        }
        curr = curr->next;
    }
    return false;
}

void free_segment(int seg_id) {
    Block* curr = memory_list;
    while (curr) {
        if (!curr->is_free && curr->seg_id == seg_id) {
            curr->is_free = true;
            curr->seg_id = -1;
            break;
        }
        curr = curr->next;
    }
    // Merge adjacent free blocks (Coalescing)
    curr = memory_list;
    while (curr && curr->next) {
        if (curr->is_free && curr->next->is_free) {
            Block* temp = curr->next;
            curr->size_kb += temp->size_kb;
            curr->next = temp->next;
            free(temp);
        } else {
            curr = curr->next;
        }
    }
}

void report_fragmentation(int step) {
    int total_free = 0;
    int max_contiguous_free = 0;
    int free_blocks = 0;

    Block* curr = memory_list;
    while (curr) {
        if (curr->is_free) {
            total_free += curr->size_kb;
            if (curr->size_kb > max_contiguous_free) max_contiguous_free = curr->size_kb;
            free_blocks++;
        }
        curr = curr->next;
    }

    int external_frag_kb = total_free - max_contiguous_free;
    printf("Step %2d | Total Free: %4d KB | Largest Contiguous: %4d KB | Free Holes: %2d | Ext Frag: %4d KB\n",
           step, total_free, max_contiguous_free, free_blocks, external_frag_kb);
}

int main(void) {
    printf("=======================================================================\n");
    printf("  EXTERNAL FRAGMENTATION SIMULATOR FOR 20 SEGMENT ALLOCATIONS         \n");
    printf("  Student: S. Sachin | Reg: 192525045 | CSA04 CO3                     \n");
    printf("  Total Memory: %d KB                                                 \n", TOTAL_MEMORY_KB);
    printf("=======================================================================\n\n");

    init_memory();

    // 20 interleaved allocation and deallocation operations
    int alloc_sizes[20] = {80, 120, 60, 90, 150, 70, 110, 85, 95, 130,
                           50,  75, 65, 80, 100, 90,  60, 85, 70,  95};

    for (int i = 0; i < 10; i++) {
        allocate_first_fit(i + 1, alloc_sizes[i]);
    }
    printf("[State After 10 Initial Allocations]:\n");
    report_fragmentation(10);

    // Deallocate alternating segments to create fragmented holes
    free_segment(2);
    free_segment(4);
    free_segment(7);
    free_segment(9);
    printf("\n[State After Freeing Segments 2, 4, 7, 9 (Creating Non-contiguous Holes)]:\n");
    report_fragmentation(14);

    // Attempting remaining allocations
    printf("\n[Attempting Allocations into Fragmented Memory]:\n");
    for (int i = 10; i < 20; i++) {
        bool ok = allocate_first_fit(i + 1, alloc_sizes[i]);
        if (!ok) {
            printf(">>> Segment %d (%d KB) FAILED to allocate due to External Fragmentation!\n",
                   i + 1, alloc_sizes[i]);
        }
        report_fragmentation(i + 5);
    }

    printf("\n-----------------------------------------------------------------------\n");
    printf("Observation: External fragmentation leaves sufficient aggregate memory,\n");
    printf("yet requests fail because memory is broken into non-contiguous holes.\n");
    printf("Compaction is required, which imposes massive memory copying overhead.\n");
    printf("=======================================================================\n");

    return 0;
}
