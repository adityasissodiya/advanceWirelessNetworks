# Lab 02: WiFi Network Performance

**Language Options:** Complete this lab in **C++ or Python** (choose one).  
**ns-3 version:** 3.40

---

## Objectives

By the end of this lab you will:

1. Measure application-layer throughput in an infrastructure WiFi network for two topologies.  
2. Sweep physical-layer data rate and packet payload to study throughput vs. packet size.  
3. Explore the hidden-terminal problem with and without RTS/CTS.  
4. Automate multiple seeds via RngSeedManager.  
5. Visualize scenarios with NetAnim.  

---

## Prerequisites & Setup

- Installation & build: [common/setup.md](../common/setup.md)  
- API & tutorials: [common/links.md](../common/links.md)

---

## Part 1: Infrastructure WiFi Throughput

### Task (C++): Scenario 1 – Single AP, One Sender & Receiver

1. **Copy** `code/Lab2_Cpp_Scenario1.cc` to your `scratch/` directory.  
2. **Rebuild** ns-3:
   ```bash
   cd ~/ns-allinone-3.40/ns-3.40
   cmake --build build -j$(nproc)
    ```

3. **Edit** distances in the script to form an equilateral triangle of side 10 m.
4. **Set** MAC to IEEE 802.11b and traffic to UDP (payload=1000 B).
5. **Vary** PHY rate ∈ {1, 5.5, 11 Mbps}, running two seeds per rate:

   ```cpp
   RngSeedManager::SetSeed(1);
   RngSeedManager::SetRun(1);  // first trial
   // run sim
   RngSeedManager::SetRun(2);  // second trial
   // run sim
   ```
6. **Measure** per-trial and average application throughput.
7. **Visualize** with NetAnim: enable `AnimationInterface` and capture XML.

**Likely issues:**

* Mode-name typo: see [3.1 in common/troubleshooting.md](../common/troubleshooting.md#31-mode-name-typo).
* No routing in infra mode: install `InternetStackHelper` on all nodes.

---

### Task (C++): Scenario 1 – Two Triangles

1. **Copy** `code/Lab2_Cpp_Scenario2.cc` to `scratch/`.
2. **Configure** four STA nodes and one AP in two equilateral triangles.
3. **Install** two OnOff applications (ports 9 & 10) and sinks.
4. **Run** for each PHY rate and two seeds.
5. **Measure** per-flow and aggregate throughput.
6. **Visualize** with NetAnim.

**Likely issues:**

* FlowMonitor missing second flow: verify distinct port numbers.

---

### Task (Python): Scenario 1 (Parts 1 & 2)

Repeat the C++ steps above using `code/Lab2_Py_Scenario1.py` and `code/Lab2_Py_Scenario2.py`:

1. **Copy** scripts to `scratch/`.
2. **Set** `PYTHONPATH`:

   ```bash
   export PYTHONPATH=~/ns-allinone-3.40/ns-3.40/build/bindings:$PYTHONPATH
   ```
3. **Run** each scenario with the same parameter sweeps and two seeds.
4. **Collect** throughput data and visualize with NetAnim (scripts enable XML output).

**Likely issues:**

* Python import errors: see [1.3 in common/troubleshooting.md](../common/troubleshooting.md#13-importerror-no-module-named-nscore).

---

## Part 2: Packet Size & Hidden-Terminal

![Two-Ray Ground Reflection Model](/common/images/hiddenTerminalScenario.png)  
*Refer to jayasuriya2004-hidden.pdf*

### Task (C++): Scenario 2 – Payload Sweep

1. **Copy** `Lab2_Cpp_Scenario1.cc` as base into `scratch/` and rename to `Lab2_Cpp_PayloadSweep.cc`.
2. **Modify** for distances = *dᵢ*/2 from Lab 1.
3. **Sweep** PHY rates {1, 5.5, 11 Mbps} × payloads {400, 700, 1000 B} → 9 experiments.
4. **Measure** throughput for each combination.
5. **Plot** throughput vs. payload for each PHY rate.

**Likely issues:**

* Invalid DataMode string: check [common/links.md](../common/links.md) for exact mode names.

---

### Task (C++): Scenario 2 – Hidden Terminal

1. **Copy** `code/Lab2_Cpp_Scenario2.cc` (or `Lab2_Cpp_Hidden.cc`) into `scratch/`.
2. **Arrange** STA0 at (0,0), AP at (dᵢ,0), STA1 at (2dᵢ,0).
3. **Install** two OnOff apps (same payload=1000 B, rate=1 Mbps) on ports 9 & 10.
4. **Run** with RTS/CTS disabled (`RtsCtsThreshold=2200`), measure throughput & PDR.
5. **Enable** RTS/CTS (`RtsCtsThreshold=0`), rerun, re-measure.
6. **Compare** results and analyze.

**Likely issues:**

* RTS/CTS no effect: see [3.2 & 3.3 in common/troubleshooting.md](../common/troubleshooting.md#32-rtscts-has-no-effect).

---

### Task (Python): Packet Sweep & Hidden Terminal

Repeat Part 2 in Python using analogous scripts (`Lab2_Py_PayloadSweep.py` and a hidden-terminal script). Ensure binding imports and run with identical parameters.

---

## Deliverables

See [`deliverables.md`](deliverables.md).

---

## Cross-References

* Setup instructions: [common/setup.md](../common/setup.md)
* Troubleshooting guide: [common/troubleshooting.md](../common/troubleshooting.md)
* API & tutorial links: [common/links.md](../common/links.md)

---
