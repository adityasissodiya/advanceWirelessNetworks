```markdown
# Advanced Wireless Networks – ns-3 Lab Series (2025 Edition)

> **Course code:** D7030E    
> **University:** Luleå University of Technology  
> **Target audience:** MSc / PhD students learning research-grade network simulation  
> **Languages:** C++ **and** Python bindings for ns-3  
> **Latest tested ns-3 release:** 3.40 (works with 3.41+)

This repository contains **five incremental laboratories (Lab 0 – Lab 4)** designed to teach hands-on network-simulation skills with the **ns-3** discrete-event simulator.  
Each lab comes in two flavours:

* **C++** — matches the canonical ns-3 workflow.  
* **Python** — uses the official pybindgen bindings so students who prefer Python aren’t left behind.

You’re free to stick to one language or mix both as you progress.
```
---

## Table of Contents
1. [Folder Layout](#folder-layout)  
2. [Prerequisites](#prerequisites)  
3. [Quick-start: Build ns-3 & Run a Lab](#quick-start-build-ns-3--run-a-lab)  
4. [Lab Overviews](#lab-overviews)  
5. [Troubleshooting Cheat-Sheet](#troubleshooting-cheat-sheet)  
6. [License](#license)

---

## Folder Layout

```

.
├── lab0\_intro/
│   ├── c++/
│   ├── python/
│   └── docs/          # PDF instructions + sample report template
├── lab1\_propagation/
├── lab2\_wifi/
├── lab3\_adhoc/
├── lab4\_lte/
├── common/            # utility scripts (plotting, seed helper, makefile snippets)
└── README.md          # you’re here

````

*Every lab folder contains*  
`c++/*.cc`  C++ source  
`python/*.py`  matching Python scripts  
`docs/*.pdf`  problem statement, grading rubric, and example figures

Build output (object files, NetAnim XML, pcaps) stays inside your local **ns-3 build tree**, not in this repo.

---

## Prerequisites

| Platform | Steps |
|----------|-------|
| **Native Linux** (Ubuntu 22.04 LTS or later) | Follow the package list in **Lab 0** PDF.<br>Key packages: `build-essential`, `python3-dev`, `cmake`, `qt5-default`, `netanim`, `wireshark`, `matplotlib`. |
| **Windows 10/11** | Install **WSL 2 + Ubuntu**: see Lab 0 *Prerequisites* (PowerShell commands → restart → `wsl --set-default-version 2` → install Ubuntu from Microsoft Store). |
| **macOS Sonoma** | `brew install python@3.12 cmake gcc qt wireshark` then follow the Linux dependency list (NetAnim builds fine via Qt). |

*All labs assume*  
* ns-3 cloned or extracted to `~/ns-3.40/` (or newer).  
* You built ns-3 with **both** examples/tests and **Python bindings**:  
  ```bash
  ./waf configure --enable-examples --enable-tests --enable-python-bindings
  ./waf build
````

* `PYTHONPATH=~/ns-3.40/build/bindings:$PYTHONPATH` is set for Python runs.

---

## Quick-start: Build ns-3 & Run a Lab

```bash
# 1. Clone this lab repo
git clone https://github.com/<your-org>/ltu-ns3-labs.git
cd ltu-ns3-labs

# 2. Build ns-3 (if you haven’t)
cd ~/ns-3.40
./waf configure --enable-examples --enable-tests --enable-python-bindings
./waf build

# 3. Copy a lab source file into the ns-3 scratch directory
cp ../ltu-ns3-labs/lab1_propagation/c++/Lab1_Cpp_TwoRay.cc scratch/

# 4. Run it
./waf --run scratch/Lab1_Cpp_TwoRay --command-template="%s --distance=50"

# 5. Visualise (optional)
netanim lab1-two-ray.xml
```

Python version:

```bash
export PYTHONPATH=~/ns-3.40/build/bindings:$PYTHONPATH
cp ../ltu-ns3-labs/lab1_propagation/python/Lab1_Py_TwoRay.py scratch/
./waf --pyrun scratch/Lab1_Py_TwoRay.py --distance=50
```

---

## Lab Overviews

| Lab                                  | Core Theme                                                              | What You Build & Measure                                            | Key Skills                                           |
| ------------------------------------ | ----------------------------------------------------------------------- | ------------------------------------------------------------------- | ---------------------------------------------------- |
| **Lab 0 – Introduction / Toolchain** | ns-3 install (C++ + Python), WSL 2 guide, NetAnim basics                | *Hello-Simulator* in both languages; first point-to-point animation | Compilation, binding setup, PCAP capture             |
| **Lab 1 – Propagation Models**       | Two-node WiFi link, compare **Two-Ray**, **Cost231-Hata**, **Friis**    | Throughput vs. distance; derive border range *dᵢ*                   | Switching loss models, FlowMonitor, plotting         |
| **Lab 2 – WiFi Networks**            | Infrastructure WiFi, **payload & rate sweeps**, and **hidden-terminal** | Triangle topologies (single & double), RTS/CTS on/off               | Seed randomization, RngSeedManager, Wireshark timing |
| **Lab 3 – Ad-hoc Multi-hop**         | UDP vs TCP over chains of 3-6 hops                                      | Throughput vs. hop-count & packet size; hidden-terminal revisit     | OLSR / static routing, TCP MSS tuning                |
| **Lab 4 – LTE Module**               | EPC + eNodeB + UE, different antenna patterns                           | LTE traces, DL/UL throughput vs. data-rate & distance               | LENA helper API, RLC vs PDCP stats                   |

Each PDF in `lab*/docs/` details **problem statements, deliverables, scoring rubric, and troubleshooting check-lists**.  Example report templates are included.

---

## Troubleshooting Cheat-Sheet

| Problem                                            | Quick Fix                                                                                                                                       |
| -------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------- |
| `ImportError: no module named ns.core`             | `export PYTHONPATH=~/ns-3.40/build/bindings:$PYTHONPATH`                                                                                        |
| WiFi **modus name typo** (`DsssRate5.5Mbps` error) | Use exact strings from Doxygen: `DsssRate5_5Mbps`                                                                                               |
| RTS/CTS has no effect                              | Set threshold **before** installing devices:<br>`Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold","0")`                      |
| FlowMonitor shows zero bytes                       | Install monitor *before* applications start; check 5-tuple addresses/ports                                                                      |
| NetAnim XML missing                                | Add `#include "ns3/netanim-module.h"` in C++ or `import ns.netanim` in Python, and keep `AnimationInterface` alive until `Simulator::Destroy()` |

More issues & fixes are embedded in every lab PDF.

---
