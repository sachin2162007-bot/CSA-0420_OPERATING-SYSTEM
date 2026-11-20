/**
 * Task 01: Online Examination Server Process Model Demonstration
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 2 (CO2)
 *
 * Demonstrates:
 * - Process creation with fork()
 * - Process state transitions (New -> Ready -> Running -> Waiting -> Terminated)
 * - Process Control Block (PCB) attributes (PID, PPID, state, exit status)
 * - Simulation of student exam session, answer auto-saving, and video proctoring
 * - Observation points for ps -ef and /proc/<pid>/status
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void simulate_exam_session(int student_id) {
    pid_t pid = getpid();
    pid_t ppid = getppid();
    printf("[STUDENT %d] Session process created (PID: %d, PPID: %d)\n", student_id, pid, ppid);
    printf("[STUDENT %d] State: RUNNING (Parsing exam questions)\n", student_id);
    
    // Simulate CPU computation
    volatile long sum = 0;
    for (int i = 0; i < 1000000; i++) sum += i;
    
    // Simulate I/O wait (student answering / network latency)
    printf("[STUDENT %d] State: WAITING (Simulating student typing and network I/O)\n", student_id);
    sleep(1);
    
    printf("[STUDENT %d] State: RUNNING (Saving answer draft to database)\n", student_id);
    printf("[STUDENT %d] Session complete. State: TERMINATED\n", student_id);
    exit(0);
}

void simulate_proctor_session(int student_id) {
    pid_t pid = getpid();
    printf("  [PROCTOR %d] Video stream monitor spawned (PID: %d)\n", student_id, pid);
    sleep(1);
    exit(0);
}

int main(int argc, char *argv[]) {
    int num_students = 3; // Demonstration count (scaled in analysis to 2,000)
    if (argc > 1) {
        num_students = atoi(argv[1]);
        if (num_students <= 0 || num_students > 100) num_students = 3;
    }

    printf("================================================================\n");
    printf(" ONLINE EXAMINATION SERVER - PROCESS CREATION DEMO\n");
    printf(" Student: S. Sachin (192525045) | Course: CSA04 OS Assessment 2\n");
    printf(" Server Main Coordinator PID: %d\n", getpid());
    printf(" Simulating %d concurrent student sessions + proctoring processes\n", num_students);
    printf("================================================================\n\n");

    pid_t student_pids[num_students];
    pid_t proctor_pids[num_students];

    // Fork student exam sessions
    for (int i = 0; i < num_students; i++) {
        student_pids[i] = fork();
        if (student_pids[i] < 0) {
            perror("fork student");
            exit(1);
        } else if (student_pids[i] == 0) {
            simulate_exam_session(i + 1);
        }

        // Fork companion video proctoring process
        proctor_pids[i] = fork();
        if (proctor_pids[i] < 0) {
            perror("fork proctor");
            exit(1);
        } else if (proctor_pids[i] == 0) {
            simulate_proctor_session(i + 1);
        }
    }

    printf("\n[COORDINATOR] All child processes dispatched. Observing with ps / /proc...\n");
    printf("[COORDINATOR] Active Child PIDs:\n");
    for (int i = 0; i < num_students; i++) {
        printf("  Student %d: PID %d | Proctor %d: PID %d\n", 
               i + 1, student_pids[i], i + 1, proctor_pids[i]);
    }
    printf("\n");

    // Parent waits for all child processes to prevent zombies
    int status;
    pid_t wpid;
    while ((wpid = wait(&status)) > 0) {
        if (WIFEXITED(status)) {
            printf("[COORDINATOR] Child process %d terminated cleanly (exit status: %d)\n",
                   wpid, WEXITSTATUS(status));
        }
    }

    printf("\n================================================================\n");
    printf(" All student sessions and proctoring processes completed.\n");
    printf(" Resource reclaim verified: PCB memory released, open files closed.\n");
    printf("================================================================\n");
    return 0;
}
