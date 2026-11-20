/**
 * libcrypto_demo.c - Banking Encryption Module Implementation
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 3 (CO3)
 *
 * This source is compiled into:
 *  1. Static library: libcrypto_demo.a (via ar rcs)
 *  2. Shared library: libcrypto_demo.so (via gcc -fPIC -shared)
 */

#include "libcrypto_demo.h"
#include <stdio.h>
#include <string.h>

#ifndef CRYPTO_VERSION_STRING
#define CRYPTO_VERSION_STRING "v1.0.0-Standard-XOR"
#endif

const char* crypto_get_version(void) {
    return CRYPTO_VERSION_STRING;
}

void crypto_encrypt(const char* input, char* output, int len, int key) {
    if (!input || !output || len <= 0) return;
    for (int i = 0; i < len; i++) {
        output[i] = (char)(input[i] ^ ((key + i) & 0xFF));
    }
    output[len] = '\0';
}

void crypto_decrypt(const char* input, char* output, int len, int key) {
    if (!input || !output || len <= 0) return;
    for (int i = 0; i < len; i++) {
        output[i] = (char)(input[i] ^ ((key + i) & 0xFF));
    }
    output[len] = '\0';
}

unsigned int crypto_checksum(const char* data, int len) {
    unsigned int sum = 0x55AA55AA;
    for (int i = 0; i < len; i++) {
        sum = (sum << 5) + sum + (unsigned char)data[i];
    }
    return sum;
}
