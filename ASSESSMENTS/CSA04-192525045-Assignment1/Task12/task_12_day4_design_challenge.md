# Task 12 — Day-4 Design Challenge: Re-evaluation and Adaptation

**Course Code:** CSA04 — Operating Systems  
**Assignment:** Assignment 1 (CO1)  
**Student Name:** S. Sachin  
**Register Number:** 192525045  
**Scenario:** 500-Bed Smart Hospital Computing Infrastructure  
**Bloom's Level:** K5 – Evaluate  

---

## 1. Changed Requirements Summary

The Smart Hospital infrastructure faces two major architectural modifications:
1. **Remote Patient Monitoring at Home:** Patients recover at home equipped with continuous wearable telemetry devices streaming vital signs over public residential Internet connections.
2. **Cloud-Based Records Service Shared with Partner Hospitals:** Patient electronic health records are migrated from the on-premise datacenter to a federated cloud service shared dynamically with two partner healthcare institutions.

**Architectural Decision:** We formally decide to **MODIFY** our original operating system design. 

A complete redesign is unwarranted because our on-premise bedside RTOS nodes and local clinical workstations remain optimal. Retaining the design without changes is impossible because public Internet connections introduce unbounded latency that invalidates hard real-time guarantees, and cloud-shared records expand the attack surface exponentially.

---

## 2. Four-Part Evaluation

### Part A: Impact Analysis (Affected OS Types, Services, and Protection)

```
+-----------------------------------------------------------------------------+
|                          IMPACT ANALYSIS MATRIX                             |
+-----------------------------------------------------------------------------+
| System Tier        | Original Architecture      | Impacted Architecture     |
|--------------------+----------------------------+---------------------------|
| Home Remote Nodes  | Bedside Hard Real-Time     | Dual-Tier Edge RTOS +     |
|                    | Local Ethernet VLAN        | Cellular Gateway & Buffer |
| Records Database   | On-Premise Clustered Linux | Hybrid Cloud-Native       |
|                    | Local SAN Storage          | Multi-Tenant Database     |
| Network Security   | Internal Hospital Firewall | Zero-Trust mTLS &         |
|                    | Trusted Corporate VLAN     | End-to-End Enclave Encrypt|
+-----------------------------------------------------------------------------+
```

1. **Operating System Types Affected:**
   * Home devices operate on unstable residential WiFi and cellular networks where packet loss and jitter render millisecond network guarantees impossible. The OS architecture must transition to an **Edge-Computing Embedded OS** capable of local autonomous alerting.
2. **Operating System Services Affected:**
   * **Communication Services:** Must adopt store-and-forward asynchronous queuing (MQTT over TLS 1.3 / WebSocket) to survive Internet link dropouts.
   * **File System Services:** Home devices require encrypted local flash ring buffers to store telemetry during broadband outages.
3. **Protection Mechanisms Affected:**
   * The security perimeter expands to untrusted home environments. Requires hardware **Secure Enclaves (ARM TrustZone / TPM 2.0)** and device attestation.

---

### Part B: Evaluation of the Original Design Under Changed Requirements

#### 1. Response-Time Feasibility Over Public Internet
* The original hospital design mandates alarms within **$\le 10\,	ext{milliseconds}$**.
* Public residential Internet connections exhibit average round-trip times (RTT) of $25	ext{--}80\,	ext{ms}$, with mobile cellular latency spikes exceeding $300\,	ext{ms}$.
* **Evaluation:** Streaming raw telemetry to the cloud for alarm processing fails hard real-time deadlines. The alarm evaluation logic must execute **locally on the home device edge OS**, triggering an immediate physical alarm sound at the patient's bedside within $5\,	ext{ms}$, while asynchronously dispatching an alert packet to the hospital cloud.

#### 2. Reliability & Availability Under Link Degradation
* In the hospital, dual-redundant fiber links provide 99.999% availability. Residential broadband experiences frequent packet loss and downtime.
* **Evaluation:** The original synchronous streaming protocol stalls or crashes upon socket disconnection. The adapted design requires an asynchronous local write-ahead buffer sustaining up to 72 hours of offline telemetry.

#### 3. Security and Multi-Tenancy
* Centralized on-premise databases assume perimeter firewall security. Storing data in a multi-hospital shared cloud exposes records to cross-tenant data leaks and unauthorized API queries.
* **Evaluation:** Requires transitioning from standard Discretionary Access Control (DAC) to **Cryptographic Envelope Encryption** with tenant-specific Hardware Security Module (HSM) keys.

---

### Part C: Adaptation and Revised Architectural Plan

```
 HOME PATIENT ENVIRONMENT                       SHARED CLOUD & HOSPITAL CONSORTIUM
+----------------------------+                 +-----------------------------------+
| Wearable Telemetry Sensor  |                 |     SHARED CLOUD EHR SERVICE      |
+----------------------------+                 |  Distributed PostgreSQL / Cockroach|
              | SPI / I2C                      |  Hardware Security Module (HSM)   |
              v                                +-----------------------------------+
+----------------------------+                                   ^
| Home Edge Monitor          |                                   | TLS 1.3 Mutual Auth
| - Embedded RTOS (FreeRTOS) |                                   | (HL7 FHIR API)
| - Local Alarm Buzzer (<5ms)|                                   v
| - Local Flash Ring Buffer  |                 +-----------------------------------+
| - Cellular 5G / WiFi Link  | --------------> | Zero-Trust Inter-Hospital Gateway |
+----------------------------+  MQTT / TLS 1.3 | - WireGuard Encrypted Mesh Tunnel |
                                               | - Policy Enforcement Point (PEP)  |
                                               +-----------------------------------+
                                                                 |
                                       +-------------------------+-------------------------+
                                       |                                                   |
                                       v                                                   v
                         +---------------------------+                       +---------------------------+
                         | Main Smart Hospital (500) |                       | Partner Hospitals (2 Inst)|
                         +---------------------------+                       +---------------------------+
```

1. **Edge Architecture for Home Devices:**
   * **OS:** Embedded Real-Time Microkernel OS (FreeRTOS / Zephyr) running on a dual-core ARM Cortex-M33 with ARM TrustZone.
   * **Local Autonomy:** Threshold evaluation occurs locally in SRAM. If an arrhythmia is detected, the device sounds a high-decibel local alarm immediately ($<5\,	ext{ms}$) regardless of network connectivity.
   * **Connectivity:** Encrypted MQTT-SN over TLS 1.3 with automated fallback from home WiFi to an integrated eSIM 5G/LTE module.
2. **Cloud-Native Federated EHR Platform:**
   * Distributed multi-region cloud database (e.g., CockroachDB / Amazon Aurora Multi-Region) with Raft consensus across three cloud availability zones.
   * Tenant isolation enforced via column-level cryptographic field encryption (AES-256-GCM) where Partner Hospital A cannot decrypt records belonging to Partner Hospital B without patient-granted consent tokens.

---

### Part D: Evidence-Based Defense and Trade-Off Analysis

#### 1. Mathematical Reliability Proof for Dual-Network Path
Let the availability of home residential WiFi be $A_{wifi} = 0.98$ (down $pprox 7\,	ext{hours/month}$) and cellular backup be $A_{cell} = 0.995$.

$$	ext{Combined Edge Link Availability } A_{total} = 1 - (1 - A_{wifi})(1 - A_{cell}) = 1 - (0.02 	imes 0.005) = 1 - 0.0001 = \mathbf{99.99\%}$$

By deploying dual-homed edge operating system networking drivers, telemetry link reliability increases from an unviable $98\%$ to an enterprise-grade $99.99\%$.

#### 2. Summary of Architectural Trade-Offs

| System Attribute | Original Design | Revised Adapted Design | Justification for Trade-Off |
| :--- | :--- | :--- | :--- |
| **Alarm Decision Point** | Central Server / Ward Screen | **Edge Device at Patient Home** | Eliminates Internet latency ($25	ext{--}80\,	ext{ms}$) to guarantee $<10\,	ext{ms}$ alarm deadline. |
| **Network Protocol** | Raw TCP Streaming Sockets | **MQTT over TLS 1.3 + Store-and-Forward** | Buffers data during residential broadband dropouts. |
| **Database Architecture**| On-Premise LAN Failover Cluster| **Hybrid Multi-Region Cloud Database** | Enables seamless, federated record sharing across 3 partner hospitals. |
| **Security Enforcement** | Network Perimeter Firewall | **Zero-Trust Identity + Enclave Encryption** | Protects patient data across untrusted public Internet channels. |

### Conclusion
Modifying the original design into an **Edge-Computing + Cloud-Federated Architecture** maintains hard real-time patient safety at home while securely scaling multi-hospital collaborative healthcare.
