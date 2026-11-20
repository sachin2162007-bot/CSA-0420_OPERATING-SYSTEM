#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

/*
 * Task 6: Buffer Size and System Call Performance
 * Complete the Code: TODO 1
 * Course: CSA04 - Operating Systems
 * Student: S. Sachin (Reg: 192525045)
 *
 * Copy src to dst with the read() and write() system calls, using a buffer of
 * bufsize bytes. Returns the number of read() calls made, or -1 on error.
 */
long copy_file(const char *src, const char *dst, size_t bufsize)
{
    char buf[8192];
    if (bufsize > sizeof(buf)) return -1;
    int in = open(src, O_RDONLY);
    if (in < 0) return -1;
    int out = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out < 0) { close(in); return -1; }
    long calls = 0;
    ssize_t n;

    // TODO 1: Loop: read up to bufsize bytes into buf (count every read call in calls);
    // stop when read returns 0 (end of file) or -1; write exactly the n bytes that were read
    while ((n = read(in, buf, bufsize)) > 0) {
        calls++;
        ssize_t written = 0;
        while (written < n) {
            ssize_t w = write(out, buf + written, (size_t)(n - written));
            if (w < 0) {
                close(in);
                close(out);
                return -1;
            }
            written += w;
        }
    }

    if (n < 0) {
        close(in);
        close(out);
        return -1;
    }

    calls++; // count the EOF read call that returned 0

    close(in);
    close(out);
    return calls;
}

int main(void)
{
    FILE *f = fopen("records.dat", "w"); /* a 10,000-byte test file */
    if (!f) return 1;
    for (int i = 0; i < 10000; i++) fputc('A' + i % 26, f);
    fclose(f);

    size_t sizes[] = {1, 512, 4096};
    for (int i = 0; i < 3; i++)
        printf("buffer %4zu bytes: %5ld read() calls\n", sizes[i],
               copy_file("records.dat", "copy.dat", sizes[i]));

    unlink("records.dat");
    unlink("copy.dat");
    return 0;
}
