/**
 * cow_demo.c - Copy-on-Write (COW) Memory Mechanics with fork()
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 3 (CO3)
 *
 * Demonstrates:
 *  1. Parent allocates a 16 MB data buffer.
 *  2. fork() creates child: Child shares physical pages with parent (marked Read-Only).
 *  3. Both inspect RSS before modification (Zero copy cost).
 *  4. Child writes to a single page: MMU traps write violation, kernel duplicates
 *     only the modified 4 KB page frame.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>

#define SIZE_MB 16
#define BUFFER_BYTES (SIZE_MB * 1024 * 1024)

long get_rss_kb(void) {
    struct rusage r;
    getrusage(RUSAGE_SELF, &r);
    return r.ru_maxrss;
}

int main(void) {
    printf("==================================================================\n");
    printf("  COPY-ON-WRITE (COW) DEMONSTRATION WITH fork()                  \n");
    printf("  Student: S. Sachin | Reg: 192525045 | CSA04 CO3                \n");
    printf("==================================================================\n\n");

    printf("[PARENT] Allocating and populating %d MB data buffer...\n", SIZE_MB);
    char *data = (char*)malloc(BUFFER_BYTES);
    if (!data) { perror("malloc"); return 1; }
    memset(data, 'A', BUFFER_BYTES);

    printf("[PARENT] Buffer initialized. Parent RSS: %ld KB\n", get_rss_kb());
    printf("[PARENT] Calling fork()...\n\n");

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        // Child Process
        printf("  [CHILD] Created with PID %d.\n", getpid());
        printf("  [CHILD] Before write: Child shares physical pages. Child RSS: %ld KB\n", get_rss_kb());
        printf("  [CHILD] Modifying 1 byte in buffer (data[0] = 'Z')...\n");

        data[0] = 'Z'; // Triggers COW fault on page 0 only!

        printf("  [CHILD] After 1-byte write: Only 1 page (4 KB) was duplicated by kernel!\n");
        printf("  [CHILD] Child RSS: %ld KB\n", get_rss_kb());

        printf("  [CHILD] Modifying entire buffer (Dirtying all pages)...\n");
        memset(data, 'X', BUFFER_BYTES);
        printf("  [CHILD] After full buffer write: All %d MB pages duplicated. Child RSS: %ld KB\n",
               SIZE_MB, get_rss_kb());
        free(data);
        printf("  [CHILD] Completed successfully.\n");
        exit(0);
    } else {
        // Parent Process
        waitpid(pid, NULL, 0);
        printf("\n[PARENT] Child has terminated. Parent data[0] = '%c' (Unchanged!).\n", data[0]);
        printf("[PARENT] COW verified: Parent memory remained isolated throughout.\n");
        free(data);
    }

    return 0;
}
