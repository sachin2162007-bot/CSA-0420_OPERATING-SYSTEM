#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <src> <dst>\n", argv[0]);
        return 1;
    }

    int in = open(argv[1], O_RDONLY);
    if (in < 0) { perror("open in"); return 1; }

    struct stat st;
    if (fstat(in, &st) < 0) { perror("fstat"); close(in); return 1; }
    size_t size = (size_t)st.st_size;

    int out = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (out < 0) { perror("open out"); close(in); return 1; }

    if (ftruncate(out, (off_t)size) < 0) { perror("ftruncate"); close(in); close(out); return 1; }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    void *src_map = mmap(NULL, size, PROT_READ, MAP_SHARED, in, 0);
    if (src_map == MAP_FAILED) { perror("mmap src"); close(in); close(out); return 1; }

    void *dst_map = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, out, 0);
    if (dst_map == MAP_FAILED) { perror("mmap dst"); munmap(src_map, size); close(in); close(out); return 1; }

    memcpy(dst_map, src_map, size);
    msync(dst_map, size, MS_SYNC);

    clock_gettime(CLOCK_MONOTONIC, &end);

    munmap(src_map, size);
    munmap(dst_map, size);
    close(in);
    close(out);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("mmap (memory-mapped copy): %zu bytes copied in %.6f s\n", size, elapsed);
    return 0;
}
