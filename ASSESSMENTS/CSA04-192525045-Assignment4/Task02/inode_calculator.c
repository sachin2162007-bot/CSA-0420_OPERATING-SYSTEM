/**
 * inode_calculator.c - Inode File-Size and Tree-Pointer Calculator
 * Course: CSA04 - Operating Systems | Assessment 4 (CO4)
 * Student Name: S. Sachin | Register Number: 192525045 | SIMATS Engineering
 *
 * Description:
 *   Computes maximum file size addressable by a classic UNIX-style inode:
 *   12 Direct Pointers, 1 Single-Indirect, 1 Double-Indirect, 1 Triple-Indirect.
 *   Calculates block traversal cost for a 4 GB file under various allocation methods.
 */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

void calculate_inode_limits(uint64_t block_size, uint64_t ptr_size) {
    uint64_t ptrs_per_blk = block_size / ptr_size;

    uint64_t direct_blks = 12;
    uint64_t single_blks = ptrs_per_blk;
    uint64_t double_blks = ptrs_per_blk * ptrs_per_blk;
    uint64_t triple_blks = ptrs_per_blk * ptrs_per_blk * ptrs_per_blk;

    uint64_t total_blocks = direct_blks + single_blks + double_blks + triple_blks;
    uint64_t total_bytes = total_blocks * block_size;
    double total_tib = (double)total_bytes / (1024ULL * 1024ULL * 1024ULL * 1024ULL);
    double total_tb = (double)total_bytes / 1e12;

    printf("====================================================================\n");
    printf("  UNIX-STYLE INODE MAXIMUM ADDRESSABLE FILE SIZE CALCULATOR         \n");
    printf("  Student: S. Sachin | Reg: 192525045 | SIMATS Engineering          \n");
    printf("====================================================================\n");
    printf("Block Size:                   %" PRIu64 " bytes (%" PRIu64 " KB)\n", block_size, block_size / 1024);
    printf("Pointer Size:                 %" PRIu64 " bytes\n", ptr_size);
    printf("Pointers per Indirect Block:  %" PRIu64 " pointers\n", ptrs_per_blk);
    printf("--------------------------------------------------------------------\n");
    printf("1. Direct Pointers (12):      %" PRIu64 " blocks (%.2f KB)\n",
           direct_blks, (double)(direct_blks * block_size) / 1024.0);
    printf("2. Single-Indirect Pointer:   %" PRIu64 " blocks (%.2f MB)\n",
           single_blks, (double)(single_blks * block_size) / (1024.0 * 1024.0));
    printf("3. Double-Indirect Pointer:   %" PRIu64 " blocks (%.2f GB)\n",
           double_blks, (double)(double_blks * block_size) / (1024.0 * 1024.0 * 1024.0));
    printf("4. Triple-Indirect Pointer:   %" PRIu64 " blocks (%.2f TB / %.2f TiB)\n",
           triple_blks, (double)(triple_blks * block_size) / 1e12,
           (double)(triple_blks * block_size) / (1024ULL * 1024ULL * 1024ULL * 1024ULL));
    printf("--------------------------------------------------------------------\n");
    printf("TOTAL ADDRESSABLE BLOCKS:     %" PRIu64 " blocks\n", total_blocks);
    printf("MAXIMUM FILE CAPACITY:        %" PRIu64 " bytes\n", total_bytes);
    printf("                              = %.4f TiB (Binary GiB/TiB)\n", total_tib);
    printf("                              = %.4f TB  (Decimal Metric)\n", total_tb);
    printf("====================================================================\n\n");
}

void calculate_traversal_cost(uint64_t file_size_bytes, uint64_t block_size) {
    uint64_t num_blocks = (file_size_bytes + block_size - 1) / block_size;
    printf("====================================================================\n");
    printf("  DISK READ COST TO ACCESS LAST BLOCK (4 GB FILE, 4 KB BLOCKS)      \n");
    printf("====================================================================\n");
    printf("File Size:                    %" PRIu64 " bytes (4.00 GiB)\n", file_size_bytes);
    printf("Block Size:                   %" PRIu64 " bytes (4 KiB)\n", block_size);
    printf("Total Blocks in File:         %" PRIu64 " blocks (2^20 blocks)\n", num_blocks);
    printf("Target Block to Read:         Block #%" PRIu64 " (the final block)\n", num_blocks - 1);
    printf("--------------------------------------------------------------------\n");
    printf("Allocation Method             | Disk Reads Needed | Traversal Mechanism\n");
    printf("------------------------------+-------------------+-----------------\n");
    printf("Contiguous Allocation         |         1         | Direct O(1) arithmetic: start + 1048575\n");
    printf("Linked Allocation             | 1,048,576         | Sequential O(N) chain traversal\n");
    printf("FAT Allocation (RAM Cached)   |         1         | FAT lookup in RAM + 1 physical disk read\n");
    printf("FAT Allocation (Uncached Disk)|         2         | 1 disk read for FAT block + 1 data block\n");
    printf("Indexed Allocation (UNIX Inode|         4         | Inode in RAM: 1 Triple + 1 Double + 1 Single + 1 Data\n");
    printf("====================================================================\n");
}

int main(void) {
    calculate_inode_limits(4096, 4);
    calculate_traversal_cost(4ULL * 1024 * 1024 * 1024, 4096);
    return 0;
}
