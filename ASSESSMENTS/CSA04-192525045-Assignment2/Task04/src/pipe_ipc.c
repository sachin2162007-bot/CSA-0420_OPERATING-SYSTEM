/**
 * Task 04: Weather Station Data Pipeline - UNIX Pipe IPC
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 2 (CO2)
 *
 * Implements the supplied pipe code from the assignment PDF with TODO 1 completed.
 * Demonstrates:
 * - Parent process sending sensor readings to child process via anonymous pipe
 * - Detection of EOF on pipe read() when write descriptor is closed
 * - Demonstration mode showing hang behavior when close(fd[1]) is omitted
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

/* The parent (sensor reader) sends readings to the child (analyser) through a pipe. */
int main(int argc, char *argv[])
{
    int test_hang = 0;
    if (argc > 1 && argv[1][0] == 'h') test_hang = 1;

    int fd[2];
    if (pipe(fd) == -1) { perror("pipe"); return 1; }
    pid_t pid = fork();
    if (pid == 0) {
        close(fd[1]); /* child only reads */
        int r, count = 0, sum = 0, max = 0;

        // TODO 1: Read one int at a time from fd[0] until read returns 0 (the parent
        // closed the pipe); keep the count, the sum and the maximum
        int reading;
        while ((r = read(fd[0], &reading, sizeof(reading))) > 0) {
            if (count == 0 || reading > max) {
                max = reading;
            }
            sum += reading;
            count++;
        }

        printf("analyser: %d readings, average %.1f, max %d\n", count,
               count ? (double)sum / count : 0.0, max);
        close(fd[0]);
        fflush(stdout);
        _exit(0);
    }

    close(fd[0]); /* parent only writes */
    int readings[] = {31, 33, 36, 34, 41, 38};
    for (int i = 0; i < 6; i++)
        if (write(fd[1], &readings[i], sizeof readings[i]) < 0) perror("write");

    if (!test_hang) {
        close(fd[1]); /* signals end of data to the child */
    } else {
        printf("DEBUG: Intentionally omitting close(fd[1]) to demonstrate hang!\n");
        fflush(stdout);
        // Parent sleeps while child hangs in read()
        sleep(2);
        printf("DEBUG: Child is hanging because fd[1] reference count > 0!\n");
        close(fd[1]); // Cleanup to allow graceful finish
    }

    waitpid(pid, NULL, 0);
    printf("reader: done\n");
    return 0;
}
