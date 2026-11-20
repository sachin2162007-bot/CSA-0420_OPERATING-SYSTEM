/**
 * Task 04: POSIX Message Queue IPC
 * Student Name: S. Sachin (192525045) | CSA04 OS Assessment 2
 * Implements structured message passing with priority.
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <mqueue.h>

#define QUEUE_NAME "/weather_mq"
#define MAX_MSG_SIZE 64

int main(void) {
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(int);
    attr.mq_curmsgs = 0;

    mqd_t mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0666, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    pid_t pid = fork();
    if (pid == 0) {
        // Child: Analyser
        int count = 0, sum = 0, max = 0, val;
        while (1) {
            unsigned int prio;
            ssize_t bytes = mq_receive(mq, (char *)&val, sizeof(val), &prio);
            if (bytes > 0) {
                if (val == -1) break; // Sentinel EOF
                if (count == 0 || val > max) max = val;
                sum += val;
                count++;
            }
        }
        printf("mq-analyser: %d readings, average %.1f, max %d (message queue IPC)\n",
               count, count ? (double)sum / count : 0.0, max);
        mq_close(mq);
        _exit(0);
    }

    // Parent: Sensor Reader
    int readings[] = {31, 33, 36, 34, 41, 38};
    for (int i = 0; i < 6; i++) {
        mq_send(mq, (const char *)&readings[i], sizeof(int), 1);
    }
    int stop_sentinel = -1;
    mq_send(mq, (const char *)&stop_sentinel, sizeof(int), 1); // Signal EOF

    waitpid(pid, NULL, 0);
    printf("mq-reader: done\n");
    mq_close(mq);
    mq_unlink(QUEUE_NAME);
    return 0;
}
