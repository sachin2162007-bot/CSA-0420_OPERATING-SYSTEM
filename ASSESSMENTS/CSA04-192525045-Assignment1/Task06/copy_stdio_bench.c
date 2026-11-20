#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CHUNK 4096

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <src> <dst>\n", argv[0]);
        return 1;
    }

    FILE *in = fopen(argv[1], "rb");
    if (!in) { perror("fopen in"); return 1; }

    FILE *out = fopen(argv[2], "wb");
    if (!out) { perror("fopen out"); fclose(in); return 1; }

    char buf[CHUNK];
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    size_t n;
    size_t total_bytes = 0;
    while ((n = fread(buf, 1, sizeof(buf), in)) > 0) {
        total_bytes += n;
        fwrite(buf, 1, n, out);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    fclose(in);
    fclose(out);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Stdio (fread/fwrite with 4096B stream buffer): %zu bytes copied in %.6f s\n",
           total_bytes, elapsed);
    return 0;
}
