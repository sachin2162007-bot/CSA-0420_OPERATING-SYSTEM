#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[])
{
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <bufsize> <src> <dst>\n", argv[0]);
        return 1;
    }

    size_t bufsize = (size_t)atol(argv[1]);
    char *buf = (char *)malloc(bufsize);
    if (!buf) { perror("malloc"); return 1; }

    int in = open(argv[2], O_RDONLY);
    if (in < 0) { perror("open in"); free(buf); return 1; }

    int out = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out < 0) { perror("open out"); close(in); free(buf); return 1; }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    ssize_t n;
    long read_count = 0;
    long write_count = 0;
    size_t total_bytes = 0;

    while ((n = read(in, buf, bufsize)) > 0) {
        read_count++;
        total_bytes += (size_t)n;
        ssize_t written = 0;
        while (written < n) {
            ssize_t w = write(out, buf + written, (size_t)(n - written));
            if (w < 0) { perror("write"); break; }
            written += w;
            write_count++;
        }
    }
    read_count++;

    clock_gettime(CLOCK_MONOTONIC, &end);

    close(in);
    close(out);
    free(buf);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Buffer: %7zu bytes | Total: %zu bytes | Reads: %7ld | Writes: %7ld | Time: %.6f s\n",
           bufsize, total_bytes, read_count, write_count, elapsed);
    return 0;
}
