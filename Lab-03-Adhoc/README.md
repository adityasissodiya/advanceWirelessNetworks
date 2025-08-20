````markdown
# Lab 03 – Ad Hoc Multi-hop Networks

In this lab you will investigate the behavior of Wi-Fi in **ad hoc mode** (no access point). You will study how throughput changes over multiple hops, how packet size impacts performance, compare TCP vs UDP, and revisit the hidden terminal problem in an ad-hoc context.

## Learning Goals
- Understand throughput degradation in multi-hop wireless chains.
- Analyze the effect of packet size on throughput in ad hoc topologies.
- Compare TCP and UDP performance over multi-hop links.
- Examine hidden terminal problems in ad hoc mode and the role of RTS/CTS.

## Provided Files
- **docs/**
  - `Lab-03-Instructions.pdf` – detailed lab description.
  - `deliverables.md` – required submission files.
- **code/**
  - Part 1: `Lab3_Cpp_Adhoc.cc`, `Lab3_Py_Adhoc.py`
  - Part 2: `Lab3_Cpp_PayloadSweep.cc`, `Lab3_Py_PayloadSweep.py`
  - Part 3: `Lab3_Cpp_TCP.cc`, `Lab3_Py_TCP.py`
  - Part 4: `Lab3_Cpp_Hidden.cc`, `Lab3_Py_Hidden.py`

Each file sets up an ad hoc Wi-Fi network scenario corresponding to one part of the lab.
````
## Running the Code

### Example (Part 1 – Multi-hop UDP Chain)

**C++**:
```bash
cp Lab-03-Adhoc/code/Lab3_Cpp_Adhoc.cc ~/ns-allinone-3.40/ns-3.40/scratch/
cd ~/ns-allinone-3.40/ns-3.40
./ns3 build
./ns3 run scratch/Lab3_Cpp_Adhoc --numNodes=5 --pktSize=500
```

**Python**:
```bash
python3 Lab-03-Adhoc/code/Lab3_Py_Adhoc.py --numNodes=5 --pktSize=500
```

Adjust `--numNodes`, `--pktSize`, and seeds as required. Similar commands apply for the other parts (payload sweep, TCP vs UDP, hidden terminals).

---

## Data Collection

* **Part 1 – Multi-hop UDP Chain**

  * Vary number of nodes (chain length).
  * Save throughput results: `udp_chain_results.csv`
  * Plot throughput vs number of hops: `udp_chain_plot.png`
  * Provide NetAnim files: `udp_chain_anim.xml`, `udp_chain_screenshot.png`

* **Part 2 – Payload Size Sweep**

  * Test multiple packet sizes at different hop counts.
  * Save results: `payload_sweep_results.csv`
  * Save per-hop plots: `payload_sweep_plots/hop3.png`, `hop4.png`, etc.

* **Part 3 – TCP vs UDP**

  * Compare throughput for TCP and UDP at two packet sizes (e.g. 300 B and 1200 B).
  * Save results: `tcp_udp_comparison.csv`
  * Plot: `tcp_udp_comparison_plot.png`

* **Part 4 – Hidden Terminal**

  * Run with RTS/CTS off and on.
  * Save results: `hidden_off.csv`, `hidden_on.csv`
  * Plot: `hidden_comparison_plot.png`
  * NetAnim files and screenshots: `hidden_off_anim.xml`, `hidden_on_anim.xml`, plus `hidden_off_screenshot.png`, `hidden_on_screenshot.png`

---

## Deliverables Checklist

(see `docs/deliverables.md` for full detail)

* `choice.txt` – language used (`C++` or `Python`)
* Part 1: CSV, plot, anim XML, screenshot
* Part 2: CSV, per-hop plots directory
* Part 3: CSV, comparison plot
* Part 4: CSVs, comparison plot, anim XMLs, screenshots

---

## Common Pitfalls

* **Routing is required:** Ad hoc mode needs a routing protocol (e.g., OLSR) for multi-hop to work. If throughput is zero beyond 1 hop, you forgot routing.
* **RTS/CTS toggle:** As in Lab 2, set `RtsCtsThreshold=0` *before* installing devices, otherwise “RTS enabled” runs will look identical to off.
* **TCP startup time:** Ensure simulation runs long enough (≥10s) for TCP flows to reach steady state.
* **Payload sweep plots:** The deliverables require one plot per hop count (named `hopX.png`). Don’t combine all hops in a single figure unless you also provide the per-hop plots.

---