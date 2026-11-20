/*
 * Course: CSA04 - Operating Systems (CO4)
 * Student: S. Sachin (192525045)
 * Assignment: Assessment 4 - Task 06 Design Challenge
 * File: storage_simulator.c
 *
 * Description:
 *   Simulates video streaming storage performance under peak load of 25,000 concurrent
 *   1080p streams (4 Mbps each = 12.5 GB/s total read throughput).
 *   Compares:
 *     - Design A: All-HDD Enterprise RAID 60 Array (24x 16TB 7200 RPM drives)
 *     - Design B: Hybrid NVMe SSD Cache (2TB Tier 0) + Enterprise HDD Capacity Pool
 *
 * Compilation:
 *   gcc -std=c11 -Wall -Wextra -O2 storage_simulator.c -o storage_simulator -lm
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define TOTAL_STREAMS 25000
#define STREAM_BITRATE_MBPS 4.0
#define STREAM_READ_MB_PER_SEC 0.5   // 4 Mbps = 0.5 MB/s
#define SEGMENT_SIZE_MB 4.0          // HLS 6-second chunk
#define CHUNK_REQUEST_RATE_HZ 0.125  // 1 chunk every 8 seconds per client

// Hardware Characteristics
// Design A: 24x HDD RAID 60 (2x 12-drive RAID 6)
#define HDD_DRIVES 24
#define HDD_MAX_IOPS_PER_DRIVE 150
#define HDD_MAX_MBPS_PER_DRIVE 220.0
#define HDD_AVG_SEEK_MS 8.5
#define HDD_ROTATIONAL_LATENCY_MS 4.16

// Design B: 4x NVMe SSD RAID 10 (Tier 0) + 16x HDD (Tier 1)
#define NVME_MAX_IOPS 800000
#define NVME_MAX_MBPS 14000.0 // 14 GB/s read throughput
#define NVME_LATENCY_MS 0.05
#define CACHE_HIT_RATIO 0.95 // 95% hot/warm streams served from NVMe Tier

typedef struct {
    char name[64];
    double throughput_demanded_gbps;
    double throughput_delivered_gbps;
    long long total_iops_demanded;
    long long total_iops_delivered;
    double avg_latency_ms;
    int streams_admitted;
    int streams_dropped;
    double drop_rate_percent;
} SimulationResult;

SimulationResult simulate_design_a(void) {
    SimulationResult r;
    snprintf(r.name, sizeof(r.name), "Design A: All-HDD RAID 60 (24 Drives)");
    
    // Demands
    r.throughput_demanded_gbps = (TOTAL_STREAMS * STREAM_BITRATE_MBPS) / 1000.0; // 100 Gbps
    // Chunks requested per second = 25,000 * 0.125 = 3,125 chunk IOPS
    // Under random read head contention, random 4MB seek throughput drops drastically:
    // Sustained random 4MB read per HDD: ~60 MB/s due to seek/rotational delay
    double effective_hdd_mbps = HDD_DRIVES * 65.0; // ~1,560 MB/s = 1.56 GB/s = 12.48 Gbps
    double max_hdd_iops = HDD_DRIVES * HDD_MAX_IOPS_PER_DRIVE; // 3,600 IOPS
    
    r.total_iops_demanded = (long long)(TOTAL_STREAMS * CHUNK_REQUEST_RATE_HZ);
    r.total_iops_delivered = (r.total_iops_demanded > max_hdd_iops) ? max_hdd_iops : r.total_iops_demanded;
    
    double delivered_gbps = (effective_hdd_mbps * 8.0) / 1000.0;
    if (delivered_gbps > r.throughput_demanded_gbps) delivered_gbps = r.throughput_demanded_gbps;
    r.throughput_delivered_gbps = delivered_gbps;

    // Admission control based on bottleneck
    double capacity_fraction = delivered_gbps / r.throughput_demanded_gbps;
    if (capacity_fraction > 1.0) capacity_fraction = 1.0;

    r.streams_admitted = (int)(TOTAL_STREAMS * capacity_fraction);
    r.streams_dropped = TOTAL_STREAMS - r.streams_admitted;
    r.drop_rate_percent = ((double)r.streams_dropped / TOTAL_STREAMS) * 100.0;
    
    // Latency under severe head saturation
    r.avg_latency_ms = HDD_AVG_SEEK_MS + HDD_ROTATIONAL_LATENCY_MS + 45.0; // Queue queueing delay
    return r;
}

SimulationResult simulate_design_b(void) {
    SimulationResult r;
    snprintf(r.name, sizeof(r.name), "Design B: Hybrid NVMe Cache (Tier 0) + HDD (Tier 1)");

    r.throughput_demanded_gbps = (TOTAL_STREAMS * STREAM_BITRATE_MBPS) / 1000.0; // 100 Gbps (12.5 GB/s)
    r.total_iops_demanded = (long long)(TOTAL_STREAMS * CHUNK_REQUEST_RATE_HZ); // 3,125 IOPS

    // NVMe handles 95% of traffic
    double nvme_demanded_gbps = r.throughput_demanded_gbps * CACHE_HIT_RATIO; // 95 Gbps (11.875 GB/s)
    double hdd_demanded_gbps = r.throughput_demanded_gbps * (1.0 - CACHE_HIT_RATIO); // 5 Gbps (0.625 GB/s)

    // NVMe delivers up to 14 GB/s = 112 Gbps (easily covers 95 Gbps)
    // HDD array (16 drives) delivers 16 * 65 MB/s = 1,040 MB/s = 8.32 Gbps (easily covers 5 Gbps)
    r.throughput_delivered_gbps = r.throughput_demanded_gbps;
    r.total_iops_delivered = r.total_iops_demanded;

    r.streams_admitted = TOTAL_STREAMS;
    r.streams_dropped = 0;
    r.drop_rate_percent = 0.0;

    // Weighted average latency: 95% NVMe (0.05 ms) + 5% HDD (12 ms)
    r.avg_latency_ms = (CACHE_HIT_RATIO * NVME_LATENCY_MS) + ((1.0 - CACHE_HIT_RATIO) * (HDD_AVG_SEEK_MS + HDD_ROTATIONAL_LATENCY_MS));
    return r;
}

int main(void) {
    SimulationResult res_a = simulate_design_a();
    SimulationResult res_b = simulate_design_b();

    printf("===============================================================================\n");
    printf("TASK 06: VIDEO STREAMING STORAGE ARCHITECTURE SIMULATOR\n");
    printf("Workload: %d Concurrent Streams @ %.1f Mbps (Peak: %.1f Gbps / 12.5 GB/s)\n",
           TOTAL_STREAMS, STREAM_BITRATE_MBPS, (TOTAL_STREAMS * STREAM_BITRATE_MBPS) / 1000.0);
    printf("===============================================================================\n\n");

    printf("%-35s | %-18s | %-18s\n", "Metric", res_a.name, res_b.name);
    printf("-------------------------------------------------------------------------------\n");
    printf("%-35s | %18.2f | %18.2f\n", "Throughput Demanded (Gbps)", res_a.throughput_demanded_gbps, res_b.throughput_demanded_gbps);
    printf("%-35s | %18.2f | %18.2f\n", "Throughput Delivered (Gbps)", res_a.throughput_delivered_gbps, res_b.throughput_delivered_gbps);
    printf("%-35s | %18lld | %18lld\n", "Chunk IOPS Demanded", res_a.total_iops_demanded, res_b.total_iops_demanded);
    printf("%-35s | %18lld | %18lld\n", "Chunk IOPS Delivered", res_a.total_iops_delivered, res_b.total_iops_delivered);
    printf("%-35s | %18.2f | %18.2f\n", "Average Chunk Latency (ms)", res_a.avg_latency_ms, res_b.avg_latency_ms);
    printf("%-35s | %18d | %18d\n", "Streams Admitted", res_a.streams_admitted, res_b.streams_admitted);
    printf("%-35s | %18d | %18d\n", "Streams Dropped / Starved", res_a.streams_dropped, res_b.streams_dropped);
    printf("%-35s | %17.1f%% | %17.1f%%\n", "Stream Failure Rate (%)", res_a.drop_rate_percent, res_b.drop_rate_percent);
    printf("-------------------------------------------------------------------------------\n\n");

    printf("ARCHITECTURAL VERDICT:\n");
    printf("  Design A (All-HDD) fails catastrophically under 25,000 streams (%.1f%% stream drop)\n", res_a.drop_rate_percent);
    printf("  due to mechanical head seek thrashing and throughput bottleneck.\n");
    printf("  Design B (Hybrid NVMe + HDD) absorbs 95%% of requests in Tier 0, delivering 100 Gbps\n");
    printf("  with 0%% stream drop and sub-millisecond response latency.\n");

    return 0;
}
