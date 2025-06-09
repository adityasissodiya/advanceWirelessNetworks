# Advanced Wireless Networks – ns-3 Lab Series (2025 Edition)

> **Course code:** D7030E    
> **University:** Luleå University of Technology  
> **Target audience:** MSc / PhD students learning research-grade network simulation  
> **Languages:** C++ **and** Python bindings for ns-3  
> **Latest tested ns-3 release:** 3.40 (works with 3.41+)

This repository contains **five incremental laboratories (Lab 0 – Lab 4)** designed to teach hands-on network-simulation skills with the **ns-3** discrete-event simulator.  
Each lab comes in two flavours:

- **C++** — matches the canonical ns-3 workflow via CMake  
- **Python** — uses the official pybindgen bindings so Python-loving students aren’t left behind

You’re free to stick to one language or mix both as you progress.

---

## Folder Layout

```
.
├── lab0\_intro/
│   ├── code/         # C++ & Python starter scripts
│   └── docs/         # PDF instructions + sample report template
├── lab1\_propagation/
│   ├── code/
│   └── docs/
├── lab2\_wifi/
│   ├── code/
│   └── docs/
├── lab3\_adhoc/
│   ├── code/
│   └── docs/
├── lab4\_lte/
│   ├── code/
│   └── docs/
├── common/           # utility scripts (plotting, seed helper, etc.)
└── README.md         # you’re here
```



Build output (object files, NetAnim XML, pcaps) stays inside your local **ns-3 build tree**, not in this repo.

---

## Prerequisites

| Platform                 | Steps                                                                                                                                           |
|--------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------|
| **Native Linux**         | Follow Lab 0 PDF. Key packages: `build-essential`, `python3-dev`, `cmake`, `qtbase5-dev`, `qttools5-dev-tools`, `netanim`, `wireshark`, `matplotlib`. |
| **Windows 10/11 (WSL2)** | See Lab 0 “Windows Users” instructions: enable WSL2, install Ubuntu, then install the same Ubuntu packages above.                                |
| **macOS Sonoma**         | `brew install python@3 cmake qt wireshark gnuplot`; then install Python deps via `pip3 install matplotlib pybindgen`.                           |

_All labs assume:_

- ns-3 is extracted to `~/ns-allinone-3.40/` (or newer).
- You have run the top-level build script with examples, tests, and Python bindings.
- Your `PYTHONPATH` includes `~/ns-allinone-3.40/ns-3.40/build/bindings`.

---

## Building & Running ns-3

1. **Download & extract**  
   ```bash
   cd ~
   wget https://www.nsnam.org/releases/ns-allinone-3.40.tar.bz2
   tar xjf ns-allinone-3.40.tar.bz2
   cd ns-allinone-3.40

2. **Build everything**

   ```bash
   ./build.py --enable-examples --enable-tests --enable-python-bindings \
              --qmake-path /usr/lib/qt5/bin/qmake
   ```

3. **Configure & build via CMake**

   ```bash
   cd ns-3.40
   cmake -S . -B build \
         -DNS3_BUILD_EXAMPLES=ON \
         -DNS3_BUILD_TESTS=ON \
         -DNS3_BUILD_PYTHON_BINDINGS=ON
   cmake --build build -j$(nproc)
   ```

4. **Run an example**

   ```bash
   # Find the binary, e.g.:
   ./build/src/core/examples/hello-simulator
   ```

5. **Run Python script**

   ```bash
   export PYTHONPATH=~/ns-allinone-3.40/ns-3.40/build/bindings:$PYTHONPATH
   python3 scratch/hello_py.py
   ```

---

## Lab Overviews

| Lab                          | Core Theme                                                          | What You Build & Measure                              | Key Skills                                      |
| ---------------------------- | ------------------------------------------------------------------- | ----------------------------------------------------- | ----------------------------------------------- |
| **Lab 0 – Introduction**     | ns-3 install (C++/Python), WSL2 setup, minimal simulation & NetAnim | “Hello” in C++/Python; basic visualization            | Build system, Python bindings, troubleshooting  |
| **Lab 1 – Propagation**      | Two-node WiFi, compare loss models (Two-Ray/Cost231/Friis)          | Throughput vs. distance; find border range *dᵢ*       | Loss models, FlowMonitor, plotting              |
| **Lab 2 – WiFi Networks**    | Infrastructure WiFi, payload & rate sweeps, hidden-terminal         | Triangle topologies; RTS/CTS on/off; throughput plots | RngSeedManager, PCAP timing, Wireshark analysis |
| **Lab 3 – Ad-hoc Multi-hop** | UDP vs TCP over 3–6 hop chain                                       | Throughput vs hops & packet size; hidden-terminal     | Routing helpers, TCP MSS tuning                 |
| **Lab 4 – LTE Module**       | EPC + eNodeB + UE, antenna patterns                                 | LTE traces; DL/UL throughput vs rate & distance       | LENA API, RLC vs PDCP stats analysis            |

Each `lab*/docs/` PDF contains problem statements, deliverables, scoring rubrics, and troubleshooting checklists. Starter code is under `lab*/code/`.

---

## Troubleshooting Cheat-Sheet

| Problem                                  | Quick Fix                                                                                                                                                       |
| ---------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `ImportError: no module named ns.core`   | `export PYTHONPATH=~/ns-allinone-3.40/ns-3.40/build/bindings:$PYTHONPATH`                                                                                       |
| Loss-model name typo (`DsssRate5.5Mbps`) | Use exact Doxygen mode strings (e.g. `DsssRate5_5Mbps`)                                                                                                         |
| RTS/CTS has no effect                    | Set threshold **before** installing devices:<br>`Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold","0")`                                      |
| FlowMonitor reports zero bytes           | Install FlowMonitor **before** applications start; verify IP/port five-tuple matches your flow                                                                  |
| NetAnim XML not generated                | Ensure you included `#include "ns3/netanim-module.h"` (C++) or `import ns.netanim` (Python) and keep your AnimationInterface alive until `Simulator::Destroy()` |

Further fixes are embedded in each lab’s PDF.

---
