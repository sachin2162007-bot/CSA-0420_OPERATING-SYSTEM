/**
 * segmentation_fault_demo.c - Real Linux SIGSEGV Signal Analysis
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 3 (CO3)
 *
 * Demonstrates how the Linux kernel detects segmentation violations:
 *  1. Installs a signal handler for SIGSEGV with SA_SIGINFO to inspect
 *     the faulting virtual memory address (si_addr).
 *  2. Intentionally touches protected memory (NULL pointer / read-only text).
 *  3. Explains what the OS kernel does during a page/segment permission fault.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <setjmp.h>

static sigjmp_buf recovery_env;

void segfault_handler(int sig, siginfo_t *si, void *unused) {
    (void)unused;
    printf("\n>>> [KERNEL TRAP INTERCEPTED] <<<\n");
    printf("Signal Number:       %d (SIGSEGV - Segmentation Violation)\n", sig);
    printf("Faulting Address:    %p\n", si->si_addr);
    printf("Fault Reason Code:   %d ", si->si_code);
    if (si->si_code == SEGV_MAPERR) {
        printf("(SEGV_MAPERR: Address not mapped to object)\n");
    } else if (si->si_code == SEGV_ACCERR) {
        printf("(SEGV_ACCERR: Invalid permissions for mapped object)\n");
    } else {
        printf("(Other segmentation fault)\n");
    }
    printf("OS Action:           Kernel MMU page fault handler trapped violation,\n");
    printf("                     verified VMA limits in task_struct->mm, identified\n");
    printf("                     illegal access, and dispatched SIGSEGV to process.\n");
    printf("Recovery:            Recovering execution safely via siglongjmp()...\n\n");
    siglongjmp(recovery_env, 1);
}

void show_linux_proc_maps(void) {
    printf("--- Current Process Memory Regions (/proc/self/maps) ---\n");
    FILE *f = fopen("/proc/self/maps", "r");
    if (!f) {
        perror("Unable to open /proc/self/maps");
        return;
    }
    char buf[256];
    int lines = 0;
    while (fgets(buf, sizeof(buf), f) && lines < 10) {
        printf("%s", buf);
        lines++;
    }
    printf("... (truncated for display)\n");
    fclose(f);
}

int main(void) {
    printf("==================================================================\n");
    printf("  LINUX SEGMENTATION FAULT (SIGSEGV) INVESTIGATION               \n");
    printf("  Student: S. Sachin | Reg: 192525045 | CSA04 CO3                \n");
    printf("==================================================================\n\n");

    show_linux_proc_maps();

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = segfault_handler;
    sigaction(SIGSEGV, &sa, NULL);

    printf("\n[EXPERIMENT 1]: Dereferencing NULL Pointer (Unmapped Address 0x0)...\n");
    if (sigsetjmp(recovery_env, 1) == 0) {
        volatile int *bad_ptr = NULL;
        *bad_ptr = 42; // Triggers SIGSEGV
    } else {
        printf("[EXPERIMENT 1]: Successfully caught and survived NULL dereference.\n");
    }

    printf("\n[EXPERIMENT 2]: Writing to Read-Only String Literal (Code Segment)...\n");
    if (sigsetjmp(recovery_env, 1) == 0) {
        char *ro_str = "READ ONLY CONSTANT";
        ro_str[0] = 'W'; // Triggers SIGSEGV (SEGV_ACCERR)
    } else {
        printf("[EXPERIMENT 2]: Successfully caught and survived Write to Read-Only page.\n");
    }

    printf("\nConclusion: The OS treats segmentation faults as hardware protection traps,\n");
    printf("preventing errant processes from corrupting other memory or the kernel.\n");
    printf("==================================================================\n");
    return 0;
}
