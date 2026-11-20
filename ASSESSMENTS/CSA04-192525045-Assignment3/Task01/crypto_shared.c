/**
 * crypto_shared.c - Banking Client with Dynamic Shared Library Linking
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 3 (CO3)
 *
 * Demonstrates load-time dynamic linking with libcrypto_demo.so.
 * The program relies on the OS dynamic linker (ld.so) at program launch.
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
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, "libcrypto_demo") || strstr(line, "crypto_shared") || strstr(line, "[stack]")) {
            printf("%s", line);
        }
    }
    fclose(f);
}

int main(void) {
    printf("====================================================\n");
    printf(" BANKING APPLICATION - SHARED LIBRARY DEMONSTRATION\n");
    printf(" Student: S. Sachin | Reg: 192525045 | CSA04 CO3\n");
    printf("====================================================\n");
    
    printf("[SHARED] Crypto Library Version: %s\n", crypto_get_version());
    
    const char transaction[] = "SECURITY AUDIT: UPDATE BRANCH CIPHER TO V2";
    int len = (int)strlen(transaction);
    char cipher[128];
    char plain[128];
    int key = 0x5C;

    printf("[SHARED] Plaintext Data:       %s\n", transaction);
    crypto_encrypt(transaction, cipher, len, key);
    printf("[SHARED] Encrypted Hex:        ");
    for (int i = 0; i < len; i++) {
        printf("%02X ", (unsigned char)cipher[i]);
    }
    printf("\n");
    
    crypto_decrypt(cipher, plain, len, key);
    printf("[SHARED] Decrypted Payload:    %s\n", plain);
    printf("[SHARED] Checksum:             0x%08X\n", crypto_checksum(plain, len));
    
    inspect_memory_maps();
    
    printf("\n[SHARED] Execution completed successfully.\n");
    return 0;
}
