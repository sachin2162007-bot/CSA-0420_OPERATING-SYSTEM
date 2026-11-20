#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/*
 * Task 7 Experiment:
 * Remove second fflush(stdout), change _exit(3) to exit(3),
 * redirect output to a file and explain why "shell: running job 2" appears twice.
 *
 * Course: CSA04 - Operating Systems
 * Student: S. Sachin (Reg: 192525045)
 */

int main(void)
{
    pid_t pid;
    int status;

    printf("shell: running job 1\n");
    fflush(stdout);

    pid = fork();
    if (pid == 0) {
        execlp("expr", "expr", "6", "*", "7", (char *)NULL);
        _exit(127);
    }
    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
        printf("job 1 exit status %d\n", WEXITSTATUS(status));

    printf("shell: running job 2\n");
    /* SECOND fflush(stdout) DELIBERATELY REMOVED */

    pid = fork();
    if (pid == 0) {
        /* CHANGED _exit(3) TO exit(3) */
        exit(3);
    }

    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
        printf("job 2 exit status %d\n", WEXITSTATUS(status));

    return 0;
}
