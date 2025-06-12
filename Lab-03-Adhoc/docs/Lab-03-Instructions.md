# Lab 03: Ad-hoc Network Performance

**Language Options:** Complete this lab in **C++ or Python** (choose one).  
**ns-3 version:** 3.40

---

## Objectives

By the end of this lab you will:

1. Build an ad-hoc WiFi chain and measure UDP throughput over multiple hops.  
2. Analyze throughput vs. number of hops and packet size.  
3. Compare TCP vs. UDP performance in a 3-node chain.  
4. Revisit the hidden-terminal problem in an ad-hoc setup with RTS/CTS on/off.  
5. Automate multiple seeds and visualize all scenarios in NetAnim.  

---

## Prerequisites & Setup

- Installation & build: [common/setup.md](../common/setup.md)  
- API & tutorials: [common/links.md](../common/links.md)  

---

## Part 1: Multi-Hop UDP Throughput

### Task (C++): Chain Setup & Measurement

1. **Copy** starter:
   ```bash
   cp code/Lab3_Cpp_Adhoc.cc ~/ns-allinone-3.40/ns-3.40/scratch/
    ```

2. **Rebuild**:

   ```bash
   cd ~/ns-allinone-3.40/ns-3.40
   cmake --build build -j$(nproc)
   ```
3. **Configure** number of nodes (`--numNodes`), distance (`--distance`), packet size (`--pktSize`) as script arguments.
4. **Run** multiple seeds:

   ```bash
   RngSeedManager::SetSeed(1);
   RngSeedManager::SetRun(1);
   ./build/src/scratch/Lab3_Cpp_Adhoc --numNodes=4 --pktSize=500 --distance=200.0
   RngSeedManager::SetRun(2);
   ./build/src/scratch/Lab3_Cpp_Adhoc --numNodes=4 --pktSize=500 --distance=200.0
   ```
5. **Record** UDP throughput for each seed and compute average.
6. **Visualize** with NetAnim (`AnimationInterface("lab3-adhoc.xml")`).

**Likely issues:**

* No routing → throughput=0: add OLSR or static routing helper (see troubleshooting).
* Python import error if using wrong build folder (for Python tasks).

### Task (Python): Chain Setup & Measurement

Repeat Part 1 in Python:

1. **Copy** `code/Lab3_Py_Adhoc.py` to `scratch/`.
2. **Set** `PYTHONPATH`:

   ```bash
   export PYTHONPATH=~/ns-allinone-3.40/ns-3.40/build/bindings:$PYTHONPATH
   ```
3. **Run** with the same arguments:

   ```bash
   python3 scratch/Lab3_Py_Adhoc.py --numNodes=5 --pktSize=700 --distance=150.0
   ```
4. **Collect** and average throughput; **generate** `lab3-py-adhoc.xml` for NetAnim.

---

## Part 2: Throughput vs. Hops & Packet Size

### Task (C++): Payload Sweep over Hop Counts

1. **Modify** or reuse `Lab3_Cpp_Adhoc.cc` to loop:

   ```cpp
   uint32_t hops[] = {3,4,5,6};
   uint32_t pkts[] = {300,700,1200};
   for (auto h: hops)
     for (auto p: pkts) { /* set numNodes=h, pktSize=p */ }
   ```
2. **Run** two seeds per experiment; **record** results in a table.
3. **Plot**:

   * Throughput vs. packet size for each hop count.
   * Throughput vs. number of hops for packet size = 1200 B.

### Task (Python): Sweep

Repeat Part 2 in Python with `Lab3_Py_PayloadSweep.py`; produce the same plots.

---

## Part 3: TCP vs. UDP Comparison

### Task (C++): TCP 3-Node Chain

1. **Copy** `Lab3_Cpp_TCP.cc` to `scratch/`.
2. **Set** TCP segment size to `pktSize`:

   ```cpp
   Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(pktSize));
   ```
3. **Run** for `numNodes=3`, `pktSize=300` and `1200`; two seeds each.
4. **Measure** and compare application throughput (TCP vs. UDP).

**Likely issue:**

* Low TCP throughput → adjust segment size (see troubleshooting).

### Task (Python): TCP

Repeat Part 3 in Python with `Lab3_Py_TCP.py`.

---

## Part 4: Hidden-Terminal in Ad-Hoc

### Task (C++): RTS/CTS Off vs. On

1. **Copy** `Lab3_Cpp_Hidden.cc` to `scratch/`.
2. **Place** STA0 at (0,0), AP at (`dᵢ`,0), STA1 at (2×`dᵢ`,0).
3. **Set** payload=1000 B, rate=1 Mbps.
4. **Run** with RTS/CTS disabled:

   ```cpp
   Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold","2200");
   ```
5. **Measure** throughput & packet-delivery ratio (PDR).
6. **Enable** RTS/CTS:

   ```cpp
   Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold","0");
   ```
7. **Rerun**, **re-measure**, and compare.

### Task (Python): Hidden-Terminal

Repeat Part 4 in Python with `Lab3_Py_Hidden.py`.

---

## Deliverables

See [`deliverables.md`](deliverables.md).

---

## Cross-References

* Setup: [common/setup.md](../common/setup.md)
* Troubleshooting: [common/troubleshooting.md](../common/troubleshooting.md)
* API & tutorial links: [common/links.md](../common/links.md)

---
