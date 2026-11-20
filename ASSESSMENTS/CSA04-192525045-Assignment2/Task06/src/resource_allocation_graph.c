/**
 * Task 06: Resource Allocation Graph (RAG) and Cycle Detection
 * Student Name: S. Sachin (192525045) | CSA04 OS Assessment 2
 * Builds adjacency matrix representing processes and resources, then runs
 * Depth First Search (DFS) to detect deadlock cycles.
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX_NODES 10

// Graph representation:
// Nodes 0..2 = Processes (P1, P2, P3)
// Nodes 3..5 = Resources (R1: GPU, R2: License, R3: Storage)
int g_adj[MAX_NODES][MAX_NODES] = {0};
int g_visited[MAX_NODES] = {0};
int g_rec_stack[MAX_NODES] = {0};

const char *node_names[] = {"P1", "P2", "P3", "R1(GPU)", "R2(License)", "R3(Storage)"};

int is_cyclic(int v, int num_nodes) {
    if (!g_visited[v]) {
        g_visited[v] = 1;
        g_rec_stack[v] = 1;

        for (int i = 0; i < num_nodes; i++) {
            if (g_adj[v][i]) {
                if (!g_visited[i] && is_cyclic(i, num_nodes)) return 1;
                else if (g_rec_stack[i]) return 1;
            }
        }
    }
    g_rec_stack[v] = 0;
    return 0;
}

int main(void) {
    int num_nodes = 6;

    // Request Edges (Process -> Resource)
    g_adj[0][3] = 1; // P1 requests R1 (GPU)
    g_adj[1][4] = 1; // P2 requests R2 (License)
    g_adj[2][5] = 1; // P3 requests R3 (Storage)

    // Assignment Edges (Resource -> Process)
    g_adj[3][1] = 1; // R1 (GPU) allocated to P2
    g_adj[4][2] = 1; // R2 (License) allocated to P3
    g_adj[5][0] = 1; // R3 (Storage) allocated to P1

    printf("=======================================================================\n");
    printf(" RESOURCE ALLOCATION GRAPH (RAG) CYCLE DETECTION\n");
    printf(" Student: S. Sachin (192525045) | Assessment 2 (CO2)\n");
    printf("=======================================================================\n\n");

    printf("Graph Edges:\n");
    for (int i = 0; i < num_nodes; i++) {
        for (int j = 0; j < num_nodes; j++) {
            if (g_adj[i][j]) {
                printf("  %s ---> %s\n", node_names[i], node_names[j]);
            }
        }
    }

    int has_cycle = 0;
    for (int i = 0; i < num_nodes; i++) {
        if (is_cyclic(i, num_nodes)) {
            has_cycle = 1;
            break;
        }
    }

    printf("\nCycle Detection Result: %s\n", has_cycle ? "DEADLOCK CYCLE FOUND!" : "NO CYCLE (SAFE)");
    if (has_cycle) {
        printf("Detected Circular Wait: P1 -> R1(GPU) -> P2 -> R2(License) -> P3 -> R3(Storage) -> P1\n");
    }
    return 0;
}
