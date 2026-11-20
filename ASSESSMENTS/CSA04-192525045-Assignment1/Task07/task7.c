#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/*
 * Task 7: Evaluate Process Control System Calls
 * Complete the Code: TODO 1
 * Course: CSA04 - Operating Systems
 * Student: S. Sachin (Reg: 192525045)
 */

int main(void)
{
    pid_t pid;
    int status;

    printf("shell: running job 1\n");
    fflush(stdout); /* empty the buffer before fork() */

    // TODO 1: Create a child with fork(). In the child, replace the program with
    // execlp("expr", "expr", "6", "*", "7", (char *)NULL) and exit(127) if exec
    // fails. In the parent, wait for the child with waitpid() and print "job 1 exit
    // status N" using WEXITSTATUS
    pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        execlp("expr", "expr", "6", "*", "7", (char *)NULL);
        perror("execlp");
        _exit(127);
    }

    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return 1;
    }

    if (WIFEXITED(status))
        printf("job 1 exit status %d\n", WEXITSTATUS(status));

    printf("shell: running job 2\n");
    fflush(stdout);

    pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        _exit(3); /* a child that fails with code 3 */
    }

    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return 1;
    }

    if (WIFEXITED(status))
        printf("job 2 exit status %d\n", WEXITSTATUS(status));

    return 0;
}
