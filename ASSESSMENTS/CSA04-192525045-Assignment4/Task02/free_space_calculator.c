/**
 * free_space_calculator.c - 4 TB Disk Free-Space Management Comparison
 * Course: CSA04 - Operating Systems | Assessment 4 (CO4)
 * Student Name: S. Sachin | Register Number: 192525045 | SIMATS Engineering
 *
 * Description:
 *   Evaluates Bit Vector, Linked Free List, Grouping, and Counting algorithms
 *   for managing free space across a 4 TB modern storage drive.
 */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

void calculate_free_space(uint64_t disk_tb, uint64_t block_size) {
    uint64_t disk_bytes = disk_tb * 1024ULL * 1024ULL * 1024ULL * 1024ULL; // Binary TiB
    uint64_t total_blocks = disk_bytes / block_size;

    // Bit vector: 1 bit per disk block
    uint64_t bit_vector_bits = total_blocks;
    uint64_t bit_vector_bytes = bit_vector_bits / 8;
    double bit_vector_mb = (double)bit_vector_bytes / (1024.0 * 1024.0);

    // Linked Free List: 4-byte pointer per free block
    // Assuming 50% disk is free:
    uint64_t free_blocks_half = total_blocks / 2;
    uint64_t linked_list_bytes = free_blocks_half * 4;
    double linked_list_gb = (double)linked_list_bytes / (1024.0 * 1024.0 * 1024.0);

    // Grouping: 1 block holds 1023 free block pointers + 1 pointer to next block
    uint64_t grouping_blocks = (free_blocks_half + 1022) / 1023;
    double grouping_mb = (double)(grouping_blocks * block_size) / (1024.0 * 1024.0);

    // Counting: (start_block_num, count) pairs (8 bytes per contiguous run)
    // Assuming average contiguous run length of 64 blocks:
    uint64_t num_runs = free_blocks_half / 64;
    uint64_t counting_bytes = num_runs * 8;
    double counting_mb = (double)counting_bytes / (1024.0 * 1024.0);

    printf("====================================================================\n");
    printf("  4 TB DISK FREE-SPACE MANAGEMENT OVERHEAD EVALUATOR                \n");
    printf("  Student: S. Sachin | Reg: 192525045 | SIMATS Engineering          \n");
    printf("====================================================================\n");
    printf("Disk Capacity:                %" PRIu64 " TB (4,398,046,511,104 bytes)\n", disk_tb);
    printf("Allocation Unit (Block Size): %" PRIu64 " bytes (4 KB)\n", block_size);
    printf("Total Addressable Blocks:     %" PRIu64 " blocks (2^30 blocks)\n", total_blocks);
    printf("--------------------------------------------------------------------\n");
    printf("Method                    | Memory / Storage Overhead | Key Characteristics\n");
    printf("--------------------------+---------------------------+---------------------\n");
    printf("1. Bit Vector (Bitmap)    | %10.2f MB (%" PRIu64 " B)   | Fast contiguous scan (CPU bit-ops)\n",
           bit_vector_mb, bit_vector_bytes);
    printf("2. Linked Free List (50%%) | %10.2f GB                 | Inefficient traversal; no contiguous awareness\n",
           linked_list_gb);
    printf("3. Grouping (50%% free)   | %10.2f MB (Reserved blks) | Compact block chains; batch alloc\n",
           grouping_mb);
    printf("4. Counting (avg 64 run)  | %10.2f MB                 | Highly compact for video/contiguous workloads\n",
           counting_mb);
    printf("====================================================================\n\n");
}

int main(void) {
    calculate_free_space(4, 4096);
    return 0;
}
