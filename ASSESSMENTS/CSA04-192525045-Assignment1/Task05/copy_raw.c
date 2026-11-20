#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

/*
 * Task 5/6: Raw POSIX read()/write() File Copy Program
 * Course: CSA04 - Operating Systems
 * Student: S. Sachin (Reg: 192525045)
 */

int main(int argc, char *argv[])
{
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <buffer_size> <source_file> <dest_file>\n", argv[0]);
        return 1;
    }

    size_t bufsize = (size_t)atol(argv[1]);
    if (bufsize == 0) {
        fprintf(stderr, "Invalid buffer size\n");
        return 1;
    }

    char *buf = (char *)malloc(bufsize);
    if (!buf) {
        perror("malloc");
        return 1;
    }

    int in = open(argv[2], O_RDONLY);
    if (in < 0) {
        perror("open src");
        free(buf);
        return 1;
    }

    int out = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out < 0) {
        perror("open dst");
        close(in);
        free(buf);
        return 1;
    }

    ssize_t n;
    long read_calls = 0;
    long write_calls = 0;
    size_t total_bytes = 0;

    while ((n = read(in, buf, bufsize)) > 0) {
        read_calls++;
        total_bytes += (size_t)n;
        ssize_t written = 0;
        while (written < n) {
            ssize_t w = write(out, buf + written, (size_t)(n - written));
            if (w < 0) {
                perror("write");
                close(in);
                close(out);
                free(buf);
                return 1;
            }
            written += w;
            write_calls++;
        }
    }

    if (n < 0) {
        perror("read");
        close(in);
        close(out);
        free(buf);
        return 1;
    }

    read_calls++; // terminating read that returns 0 (EOF)

    close(in);
    close(out);
    free(buf);

    printf("Raw copy finished: %zu bytes copied, %ld read() calls, %ld write() calls (bufsize=%zu)\n",
           total_bytes, read_calls, write_calls, bufsize);
    return 0;
}
