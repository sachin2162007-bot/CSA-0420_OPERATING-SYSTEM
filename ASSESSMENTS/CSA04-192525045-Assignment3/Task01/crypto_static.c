/**
 * crypto_static.c - Banking Client with Static Linking
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 3 (CO3)
 *
 * Demonstrates compile-time / link-time binding. All library symbols
 * are statically resolved and embedded into the final binary.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "libcrypto_demo.h"

void inspect_memory_maps(void) {
    printf("\n--- Memory Mappings (/proc/self/maps) ---\n");
    FILE *f = fopen("/proc/self/maps", "r");
    if (!f) {
        perror("Unable to open /proc/self/maps");
        return;
    }
    char line[256];
    int count = 0;
    while (fgets(line, sizeof(line), f) && count < 15) {
        printf("%s", line);
        count++;
    }
    printf("... (truncated for display)\n");
    fclose(f);
}

int main(void) {
    printf("====================================================\n");
    printf(" BANKING APPLICATION - STATIC LINKING DEMONSTRATION\n");
    printf(" Student: S. Sachin | Reg: 192525045 | CSA04 CO3\n");
    printf("====================================================\n");
    
    printf("[STATIC] Crypto Library Version: %s\n", crypto_get_version());
    
    const char transaction[] = "TRANSFER $250,000 FROM ACC#1029 TO ACC#9812";
    int len = (int)strlen(transaction);
    char cipher[128];
    char plain[128];
    int key = 0x4B;

    printf("[STATIC] Original Transaction: %s\n", transaction);
    crypto_encrypt(transaction, cipher, len, key);
    printf("[STATIC] Encrypted Hex: ");
    for (int i = 0; i < len; i++) {
        printf("%02X ", (unsigned char)cipher[i]);
    }
    printf("\n");
    
    crypto_decrypt(cipher, plain, len, key);
    printf("[STATIC] Decrypted Payload:    %s\n", plain);
    printf("[STATIC] Integrity Checksum:   0x%08X\n", crypto_checksum(plain, len));
    
    inspect_memory_maps();
    
    printf("\n[STATIC] Execution complete. All symbols statically bound.\n");
    return 0;
}
