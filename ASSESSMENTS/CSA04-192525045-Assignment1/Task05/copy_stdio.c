#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>

/*
 * Task 5/6: C Standard I/O (fread/fwrite) File Copy Program
 * Course: CSA04 - Operating Systems
 * Student: S. Sachin (Reg: 192525045)
 */

#define CHUNK_SIZE 4096

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <source_file> <dest_file>\n", argv[0]);
        return 1;
    }

    FILE *in = fopen(argv[1], "rb");
    if (!in) {
        perror("fopen in");
        return 1;
    }

    FILE *out = fopen(argv[2], "wb");
    if (!out) {
        perror("fopen out");
        fclose(in);
        return 1;
    }

    char buf[CHUNK_SIZE];
    size_t n;
    size_t total_bytes = 0;
    long fread_calls = 0;
    long fwrite_calls = 0;

    while ((n = fread(buf, 1, sizeof(buf), in)) > 0) {
        fread_calls++;
        total_bytes += n;
        size_t written = fwrite(buf, 1, n, out);
        fwrite_calls++;
        if (written < n) {
            perror("fwrite");
            fclose(in);
            fclose(out);
            return 1;
        }
    }
    fread_calls++; // terminating fread

    fclose(in);
    fclose(out);

    printf("Stdio copy finished: %zu bytes copied, %ld fread() calls, %ld fwrite() calls\n",
           total_bytes, fread_calls, fwrite_calls);
    return 0;
}
