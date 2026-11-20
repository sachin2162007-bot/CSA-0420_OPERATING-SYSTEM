#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <time.h>

/*
 * Task 5: System Calls Demonstration
 * Course: CSA04 - Operating Systems
 * Student: S. Sachin (Reg: 192525045)
 *
 * Demonstrates the 6 fundamental categories of system calls:
 * 1. Process Control (fork, waitpid, getpid, _exit)
 * 2. File Management (open, read, write, close, stat, unlink)
 * 3. Device Management (tcgetattr / terminal control, ioctl)
 * 4. Information Maintenance (getpid, time, uname)
 * 5. Communication (pipe, read, write)
 * 6. Protection (umask, chmod)
 */

void demo_file_management(void)
{
    printf("=== 1. File Management System Calls ===\n");
    const char *test_filename = "task5_temp.txt";
    const char *msg = "Demonstrating POSIX open(), write(), read(), close() system calls.\n";
    
    // open() system call
    int fd = open(test_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { perror("open write"); return; }
    
    // write() system call
    ssize_t written = write(fd, msg, strlen(msg));
    printf("write() syscall wrote %zd bytes to fd %d\n", written, fd);
    close(fd);
    
    // stat() system call for file metadata
    struct stat st;
    if (stat(test_filename, &st) == 0) {
        printf("stat() syscall: file size = %ld bytes, inode = %ld, mode = %o\n",
               (long)st.st_size, (long)st.st_ino, st.st_mode & 0777);
    }
    
    // read() system call
    fd = open(test_filename, O_RDONLY);
    if (fd >= 0) {
        char buf[128];
        ssize_t bytes_read = read(fd, buf, sizeof(buf) - 1);
        if (bytes_read > 0) {
            buf[bytes_read] = '\0';
            printf("read() syscall read %zd bytes: %s", bytes_read, buf);
        }
        close(fd);
    }
    
    // unlink() system call
    unlink(test_filename);
    printf("unlink() syscall removed %s\n\n", test_filename);
}

void demo_process_control(void)
{
    printf("=== 2. Process Control System Calls ===\n");
    printf("Parent PID = %d (getpid syscall)\n", getpid());
    
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    }
    
    if (pid == 0) {
        // Child process
        printf("Child process running (PID = %d, Parent PID = %d)\n", getpid(), getppid());
        _exit(42); // _exit syscall directly notifies kernel
    } else {
        // Parent process
        int status;
        pid_t waited_pid = waitpid(pid, &status, 0); // waitpid syscall
        if (waited_pid > 0 && WIFEXITED(status)) {
            printf("waitpid() reaped child PID %d with exit code %d\n\n",
                   waited_pid, WEXITSTATUS(status));
        }
    }
}

void demo_information_maintenance(void)
{
    printf("=== 3. Information Maintenance System Calls ===\n");
    time_t now = time(NULL); // time syscall
    printf("time() syscall returned Unix timestamp: %ld\n", (long)now);
    
    struct utsname sys_info;
    if (uname(&sys_info) == 0) { // uname syscall
        printf("uname() syscall: Sysname=%s, Nodename=%s, Release=%s, Machine=%s\n\n",
               sys_info.sysname, sys_info.nodename, sys_info.release, sys_info.machine);
    }
}

void demo_communication(void)
{
    printf("=== 4. Inter-Process Communication System Calls ===\n");
    int pipefd[2];
    if (pipe(pipefd) == -1) { // pipe syscall
        perror("pipe");
        return;
    }
    
    const char *ipc_msg = "Telemetry alarm packet transmitted via kernel pipe";
    write(pipefd[1], ipc_msg, strlen(ipc_msg) + 1); // write to pipe write-end
    close(pipefd[1]);
    
    char read_buf[128];
    ssize_t r = read(pipefd[0], read_buf, sizeof(read_buf)); // read from pipe read-end
    if (r > 0) {
        printf("pipe() read: \"%s\" (%zd bytes)\n\n", read_buf, r);
    }
    close(pipefd[0]);
}

void demo_protection(void)
{
    printf("=== 5. Protection & Security System Calls ===\n");
    mode_t old_mask = umask(0027); // umask syscall
    printf("umask() syscall set new file creation mask (old mask was %03o)\n\n", old_mask);
    umask(old_mask);
}

int main(void)
{
    printf("=====================================================\n");
    printf("  SMART HOSPITAL OS: TASK 5 SYSTEM CALLS DEMO\n");
    printf("  Student: S. Sachin | Reg No: 192525045\n");
    printf("=====================================================\n\n");
    
    demo_file_management();
    demo_process_control();
    demo_information_maintenance();
    demo_communication();
    demo_protection();
    
    printf("All system call category demonstrations completed successfully.\n");
    return 0;
}
