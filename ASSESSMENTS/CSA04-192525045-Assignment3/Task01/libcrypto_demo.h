/**
 * libcrypto_demo.h - Banking Encryption Module Interface
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 3 (CO3)
 */

#ifndef LIBCRYPTO_DEMO_H
#define LIBCRYPTO_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

// Returns current encryption algorithm version
const char* crypto_get_version(void);

// Encrypts plain text buffer with simple XOR-mask stream cipher (simulation)
void crypto_encrypt(const char* input, char* output, int len, int key);

// Decrypts cipher text buffer
void crypto_decrypt(const char* input, char* output, int len, int key);

// Computes a simulated 32-bit checksum for integrity verification
unsigned int crypto_checksum(const char* data, int len);

#ifdef __cplusplus
}
#endif

#endif /* LIBCRYPTO_DEMO_H */
