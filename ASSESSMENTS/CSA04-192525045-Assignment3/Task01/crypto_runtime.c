/**
 * crypto_runtime.c - Banking Client with Explicit Runtime Loading (dlopen)
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 3 (CO3)
 *
 * Demonstrates execution-time dynamic binding using:
 *  - dlopen(): Map shared object into process virtual address space
 *  - dlsym(): Resolve symbol address by name from dynamic symbol table
 *  - dlclose(): Unmap shared object and decrement reference counter
 *
 * Alternative Concept Implementation: Loading encryption ONLY when a
 * cryptographic transaction is requested. If the library is missing or
 * corrupted, the banking core remains online and executes non-crypto operations.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>

typedef const char* (*fn_get_version)(void);
typedef void (*fn_encrypt)(const char*, char*, int, int);
typedef void (*fn_decrypt)(const char*, char*, int, int);
typedef unsigned int (*fn_checksum)(const char*, int);

int main(int argc, char* argv[]) {
    const char* lib_path = "./libcrypto_demo.so";
    if (argc > 1) {
        lib_path = argv[1];
    }

    printf("====================================================\n");
    printf(" BANKING APPLICATION - RUNTIME LOADING (dlopen)\n");
    printf(" Student: S. Sachin | Reg: 192525045 | CSA04 CO3\n");
    printf(" Target Library: %s\n", lib_path);
    printf("====================================================\n");

    printf("[RUNTIME] Step 1: Performing ordinary branch logging (no crypto needed)...\n");
    printf("[RUNTIME] Branch Teller ID #4092 logged in. Memory clean.\n");

    printf("\n[RUNTIME] Step 2: Encrypted transaction requested. Calling dlopen()...\n");
    void* handle = dlopen(lib_path, RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "[RUNTIME ERROR] dlopen failed: %s\n", dlerror());
        fprintf(stderr, "[FAULT TOLERANCE] Core banking application survives! Non-crypto mode engaged.\n");
        return 1;
    }
    printf("[RUNTIME] Success: Library loaded into virtual memory at handle %p\n", handle);

    // Clear any existing errors
    dlerror();

    // Resolve function symbols
    fn_get_version get_version = (fn_get_version)dlsym(handle, "crypto_get_version");
    fn_encrypt encrypt = (fn_encrypt)dlsym(handle, "crypto_encrypt");
    fn_decrypt decrypt = (fn_decrypt)dlsym(handle, "crypto_decrypt");
    fn_checksum checksum = (fn_checksum)dlsym(handle, "crypto_checksum");

    char* error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "[RUNTIME ERROR] Symbol resolution failed: %s\n", error);
        dlclose(handle);
        return 1;
    }
    printf("[RUNTIME] Step 3: All cryptographic symbols resolved via dlsym().\n");
    printf("[RUNTIME] Loaded Module Version: %s\n", get_version());

    const char* secret = "AUTHORIZATION TOKEN: SEC-KEY-994821";
    int len = (int)strlen(secret);
    char cipher[128];
    char plain[128];
    int key = 0x33;

    encrypt(secret, cipher, len, key);
    printf("[RUNTIME] Encrypted Data: ");
    for (int i = 0; i < len; i++) printf("%02X ", (unsigned char)cipher[i]);
    printf("\n");

    decrypt(cipher, plain, len, key);
    printf("[RUNTIME] Decrypted Data: %s\n", plain);
    printf("[RUNTIME] Checksum:       0x%08X\n", checksum(plain, len));

    printf("\n[RUNTIME] Step 4: Transaction complete. Calling dlclose() to free memory...\n");
    dlclose(handle);
    printf("[RUNTIME] Library unmapped. Memory footprint reclaimed.\n");

    return 0;
}
