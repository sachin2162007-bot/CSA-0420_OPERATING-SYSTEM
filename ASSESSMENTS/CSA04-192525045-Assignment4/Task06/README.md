# Task 06: High-Concurrency Video Streaming Storage Design Challenge

**Course**: CSA04 – Operating Systems  
**Student Name**: S. Sachin  
**Register Number**: 192525045  
**Course Outcome**: CO4 (Bloom's Level K5 – Evaluate)  
**Institution**: SIMATS Engineering  

---

## 1. Executive Summary
This design challenge develops a complete storage subsystem architecture for an educational video-streaming platform serving 200,000 university students with a peak concurrency of 25,000 simultaneous 1080p video streams (4.0 Mbps each), requiring a sustained read throughput of **100 Gbps (12.5 GB/s)** across a 15,000-video repository.

---

## 2. Directory Structure and Files
- `streaming_storage_design.md`: Executive design master report covering the complete 5-stage framework.
- `workload_analysis.md`: Stage 1 mathematical characterization of 25,000 streams and Zipfian popularity distribution ($\alpha=0.85$).
- `capacity_calculations.md`: Stage 2A storage sizing deriving $12\text{ TB}$ base capacity to $35.16\text{ TB}$ raw protected 3-year capacity.
- `throughput_calculations.md`: Stage 2B bus bandwidth ($12.5\text{ GB/s}$) and chunk IOPS calculations.
- `architecture_design.md`: Stage 3 tiered design specification (Tier 0 NVMe SSD + Tier 2 HDD RAID 6).
- `storage_simulator.c`: C program comparing Design A (All-HDD) vs Design B (Hybrid NVMe+HDD).
- `disk_workload_simulator.c`: C program simulating Zipfian video access across 100,000 requests.
- `alternative_design_comparison.md`: Stage 5 financial TCO comparing On-Premises (\$85k) vs Cloud Egress (\$648k/mo).
- `complexity_analysis.md`: Stage 5B Big-O algorithmic complexity of ARC cache and streaming operations.
- `expected_output.txt`: Verifiable output log of architecture simulations.
- `run_experiments.sh` & `Makefile`: Automated build and benchmark orchestration.
- `config/storage_architecture.yaml`: Complete production YAML deployment manifest.

---

## 3. Core Technical Conclusions
1. **Design A (All-HDD) Failure**: A traditional rotational HDD array suffers catastrophic head seek thrashing, dropping $>85\%$ of streams under peak load.
2. **Design B (Hybrid NVMe Cache + HDD) Superiority**: Offloads $83.7\%$ of requests to Tier 0 NVMe SSDs, delivering $100\text{ Gbps}$ read throughput with $0\%$ stream drop and sub-millisecond response latency.
3. **Economic Justification**: An on-premises hybrid private storage cluster saves over $\$20\text{ Million}$ in cloud data egress fees over 3 years.

<!-- Add Gemini screenshot here: Storage throughput simulation and cache hit ratio graphs -->
