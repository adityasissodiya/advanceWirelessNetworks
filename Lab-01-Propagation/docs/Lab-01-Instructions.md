# Lab 01: Propagation Models

**Language Options:** Complete this lab in **C++ or Python** (choose one).  
**ns-3 version:** 3.40

---

## Objectives

By the end of this lab you will be able to:

1. Use Three ns-3 propagation‐loss models (Two-Ray, Cost231‐Hata, Friis).  
2. Measure application‐level throughput vs. distance in a two‐node WiFi link.  
3. Compare simulated path loss to real‐world measurements.  
4. Plot and analyze bit‐rate and path‐loss curves.  

---

## Prerequisites & Setup

See [common/setup.md](../common/setup.md) for installation and build instructions.  
Refer to [common/links.md](../common/links.md) for API and tutorial references.

---

## Part 1: Simulated Propagation Experiments

### Task (C++): Two-Ray Ground Model

1. **Copy starter code**  
   ```bash
   cp code/Lab1_Cpp_TwoRay.cc \
     ~/ns-allinone-3.40/ns-3.40/scratch/
    ```

2. **Rebuild**

   ```bash
   cd ~/ns-allinone-3.40/ns-3.40
   cmake --build build -j$(nproc)
   ```
3. **Calculate** the border distance `dᵢ` (maximum range) using Two-Ray formula.
4. **Define** distance set

   ```
   D = {dᵢ, 7dᵢ/8, 6dᵢ/8, …, dᵢ/8}
   ```
5. **For each** distance `d ∈ D`:

   * Edit `Lab1_Cpp_TwoRay.cc` to place nodes at `(0,0)` and `(d,0)`.
   * Run the simulation, capture UDP throughput.
   * Record bit-rate vs. distance.
6. **Plot** bit-rate (y-axis) vs. distance (x-axis).

**Likely Issues:**

* Mode name typo (`DsssRate5.5Mbps`): see [3.1 in common/troubleshooting.md](../common/troubleshooting.md#31-mode-name-typo).
* Zero throughput if no routing/mobility errors: see [2.3 in common/troubleshooting.md](../common/troubleshooting.md#23-zero-throughput-in-flowmonitor).

---

### Task (C++): Cost231-Hata Model

Repeat steps 1–6 using `Lab1_Cpp_Cost231.cc` and the Cost231‐Hata model.

---

### Task (C++): Friis Model

Repeat steps 1–6 using `Lab1_Cpp_Friis.cc` and the Friis model.

---

### Task (Python): Propagation Models

For each model above, repeat the experiments in Python:

1. **Copy** `code/Lab1_Py_TwoRay.py`, `Lab1_Py_Cost231.py`, `Lab1_Py_Friis.py` into `scratch/`.
2. **Ensure** `PYTHONPATH` is set:

   ```bash
   export PYTHONPATH=~/ns-allinone-3.40/ns-3.40/build/bindings:$PYTHONPATH
   ```
3. **Run** each script with distance parameter looping over `D`.
4. **Collect** and **plot** bit-rate vs. distance for all three models.

**Likely Issues:**

* Python binding import errors: see [1.3 in common/troubleshooting.md](../common/troubleshooting.md#13-importerror-no-module-named-nscore).
* Scheduler or File I/O errors: verify `scratch/` paths and permissions.

---

## Part 2: Real-World Propagation Measurements

> **Recommended**: work with a partner and laptops in corridor.

### Task: Corridor RSSI & Path Loss

1. **Set up** an ad-hoc Wi-Fi link between two laptops.
2. **Measure** RSSI at distances 1 m, 2 m, … until at least 20 m (ping + Wireshark or RSSI tool).
3. **Compute** path loss (dB) = Tx power (dBm) − RSSI (dBm).
4. **Calculate** Friis path loss for the same distances.
5. **Plot** measured vs. Friis path loss on a single graph.
6. **Explain** discrepancies between measured and theoretical curves.

**Likely Issues:**

* Missing RSSI fields in Wireshark: enable Radiotap headers.
* Units confusion (dBm vs. mW): double-check conversions.

---

## Deliverables

See [`deliverables.md`](deliverables.md).

---

## Cross-References

* Shared setup: [common/setup.md](../common/setup.md)
* Troubleshooting: [common/troubleshooting.md](../common/troubleshooting.md)
* API & tutorial links: [common/links.md](../common/links.md)

---