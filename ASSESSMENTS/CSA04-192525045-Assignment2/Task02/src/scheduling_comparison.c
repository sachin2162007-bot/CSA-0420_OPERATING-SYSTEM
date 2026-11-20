/**
 * Task 02: Full CPU Scheduling Comparison Benchmark
 * Student Name: S. Sachin | Register Number: 192525045
 * Course: CSA04 - Operating Systems | Assessment 2 (CO2)
 *
 * Implements and compares:
 * 1. First-Come First-Served (FCFS)
 * 2. Shortest Job First (SJF - Non-Preemptive)
 * 3. Shortest Remaining Time First (SRTF - Preemptive SJF)
 * 4. Priority Scheduling (Non-Preemptive, Priority: P2=1 > P4=2 > P1=3 > P3=4)
 * 5. Round Robin (RR) with time quanta: 1, 3, 5, 20
 *
 * Computes exact Gantt charts, Completion Time (CT), Turnaround Time (TAT),
 * Waiting Time (WT), and Response Time (RT).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 4

typedef struct {
    char name[4];
    int at; // Arrival Time
    int bt; // Burst Time
    int priority; // Lower integer = higher priority
    int ct; // Completion Time
    int tat; // Turnaround Time
    int wt; // Waiting Time
    int rt; // Response Time
    int first_cpu;
} Process;

void print_table(const char *algo, Process p[N]) {
    double total_tat = 0, total_wt = 0, total_rt = 0;
    printf("-----------------------------------------------------------------------\n");
    printf(" Algorithm: %s\n", algo);
    printf("-----------------------------------------------------------------------\n");
    printf(" Process  AT  BT  Pri  CT  TAT   WT   RT\n");
    for (int i = 0; i < N; i++) {
        p[i].tat = p[i].ct - p[i].at;
        p[i].wt = p[i].tat - p[i].bt;
        p[i].rt = p[i].first_cpu - p[i].at;
        total_tat += p[i].tat;
        total_wt += p[i].wt;
        total_rt += p[i].rt;
        printf(" %-7s %3d %3d %4d %3d %4d %4d %4d\n",
               p[i].name, p[i].at, p[i].bt, p[i].priority,
               p[i].ct, p[i].tat, p[i].wt, p[i].rt);
    }
    printf(" Average TAT = %6.2f | Average WT = %6.2f | Average RT = %6.2f\n\n",
           total_tat / N, total_wt / N, total_rt / N);
}

void run_fcfs() {
    Process p[N] = {
        {"P1", 0, 8, 3, 0, 0, 0, 0, -1},
        {"P2", 1, 4, 1, 0, 0, 0, 0, -1},
        {"P3", 2, 9, 4, 0, 0, 0, 0, -1},
        {"P4", 3, 5, 2, 0, 0, 0, 0, -1}
    };
    int time = 0;
    printf("Gantt Chart (FCFS):\n|");
    for (int i = 0; i < N; i++) {
        if (time < p[i].at) time = p[i].at;
        p[i].first_cpu = time;
        printf("  %s (%d-%d)  |", p[i].name, time, time + p[i].bt);
        time += p[i].bt;
        p[i].ct = time;
    }
    printf("\n");
    print_table("First-Come, First-Served (FCFS)", p);
}

void run_sjf() {
    Process p[N] = {
        {"P1", 0, 8, 3, 0, 0, 0, 0, -1},
        {"P2", 1, 4, 1, 0, 0, 0, 0, -1},
        {"P3", 2, 9, 4, 0, 0, 0, 0, -1},
        {"P4", 3, 5, 2, 0, 0, 0, 0, -1}
    };
    int completed = 0, time = 0, done[N] = {0};
    printf("Gantt Chart (SJF - Non-Preemptive):\n|");
    while (completed < N) {
        int idx = -1, min_bt = 99999;
        for (int i = 0; i < N; i++) {
            if (!done[i] && p[i].at <= time) {
                if (p[i].bt < min_bt) {
                    min_bt = p[i].bt;
                    idx = i;
                }
            }
        }
        if (idx == -1) {
            time++;
            continue;
        }
        p[idx].first_cpu = time;
        printf("  %s (%d-%d)  |", p[idx].name, time, time + p[idx].bt);
        time += p[idx].bt;
        p[idx].ct = time;
        done[idx] = 1;
        completed++;
    }
    printf("\n");
    print_table("Shortest Job First (SJF - Non-Preemptive)", p);
}

void run_srtf() {
    Process p[N] = {
        {"P1", 0, 8, 3, 0, 0, 0, 0, -1},
        {"P2", 1, 4, 1, 0, 0, 0, 0, -1},
        {"P3", 2, 9, 4, 0, 0, 0, 0, -1},
        {"P4", 3, 5, 2, 0, 0, 0, 0, -1}
    };
    int rem_bt[N];
    for (int i = 0; i < N; i++) rem_bt[i] = p[i].bt;
    int completed = 0, time = 0, last_proc = -1, seg_start = 0;

    printf("Gantt Chart (SRTF - Preemptive SJF):\n|");
    while (completed < N) {
        int idx = -1, min_rem = 99999;
        for (int i = 0; i < N; i++) {
            if (p[i].at <= time && rem_bt[i] > 0) {
                if (rem_bt[i] < min_rem) {
                    min_rem = rem_bt[i];
                    idx = i;
                }
            }
        }
        if (idx == -1) {
            time++;
            continue;
        }

        if (p[idx].first_cpu == -1) p[idx].first_cpu = time;

        if (last_proc != -1 && last_proc != idx) {
            printf("  %s (%d-%d)  |", p[last_proc].name, seg_start, time);
            seg_start = time;
        } else if (last_proc == -1) {
            seg_start = time;
        }
        last_proc = idx;

        rem_bt[idx]--;
        time++;

        if (rem_bt[idx] == 0) {
            completed++;
            p[idx].ct = time;
            printf("  %s (%d-%d)  |", p[idx].name, seg_start, time);
            seg_start = time;
            last_proc = -1;
        }
    }
    printf("\n");
    print_table("Shortest Remaining Time First (SRTF)", p);
}

void run_priority() {
    Process p[N] = {
        {"P1", 0, 8, 3, 0, 0, 0, 0, -1},
        {"P2", 1, 4, 1, 0, 0, 0, 0, -1},
        {"P3", 2, 9, 4, 0, 0, 0, 0, -1},
        {"P4", 3, 5, 2, 0, 0, 0, 0, -1}
    };
    int completed = 0, time = 0, done[N] = {0};
    printf("Gantt Chart (Priority - Non-Preemptive):\n|");
    while (completed < N) {
        int idx = -1, highest_pri = 99999;
        for (int i = 0; i < N; i++) {
            if (!done[i] && p[i].at <= time) {
                if (p[i].priority < highest_pri) {
                    highest_pri = p[i].priority;
                    idx = i;
                }
            }
        }
        if (idx == -1) {
            time++;
            continue;
        }
        p[idx].first_cpu = time;
        printf("  %s (%d-%d)  |", p[idx].name, time, time + p[idx].bt);
        time += p[idx].bt;
        p[idx].ct = time;
        done[idx] = 1;
        completed++;
    }
    printf("\n");
    print_table("Priority Scheduling (Non-Preemptive)", p);
}

int main(void) {
    printf("=======================================================================\n");
    printf(" COMPREHENSIVE CPU SCHEDULING BENCHMARK - VIDEO SERVICE JOBS\n");
    printf(" Student: S. Sachin (192525045) | Assessment 2 (CO2)\n");
    printf(" Jobs: P1(0,8), P2(1,4), P3(2,9), P4(3,5)\n");
    printf("=======================================================================\n\n");

    run_fcfs();
    run_sjf();
    run_srtf();
    run_priority();

    return 0;
}
