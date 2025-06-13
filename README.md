# Advanced Wireless Networks – ns-3 Lab Series (2025 Edition)

> **Course code:** D7030E  
> **University:** Luleå University of Technology   
> **Languages:** C++ **or** Python bindings for ns-3 (choose one)  
> **ns-3 version:** 3.40 (tested; compatibility with 3.41+ unverified)

This repository comprises five incremental laboratories (Lab 0–Lab 4) that teach hands-on network-simulation with ns-3. Each lab may be completed in C++ or Python; you are not required to do both.

---

## Project Layout

```
.
├── README.md                     # Top-level overview & quick-start
├── .gitignore                    
├── common/                       
│   ├── simulation_background.md  # Overview of the simulation architecture used by ns-3
│   ├── setup.md                  # Install & Build instructions
│   ├── troubleshooting.md        # Consolidated troubleshooting & cheat-sheet
│   ├── links.md                  # Common Doxygen & tutorial links
│   └── scripts/                  # Utility scripts
│       ├── plot_helper.py
│       └── seed_manager.sh
│
├── Lab-00-Introduction/          
│   ├── docs/                     
│   │   ├── Lab-00-Instructions.pdf  
│   │   └── deliverables.md           # What to submit for Lab 0
│   └── code/                     
│       ├── Lab0_Cpp_Hello.cc     # C++ starter “Hello Simulator”
│       └── Lab0_Py_Hello.py      # Python starter “Hello Simulator”
│
│
├── Lab-01-Propagation/           
│   ├── docs/                     
│   │   ├── Lab-01-Instructions.pdf
│   │   └── deliverables.md           # What to submit for Lab 1
│   └── code/                     
│       ├── Lab1_Cpp_TwoRay.cc
│       ├── Lab1_Cpp_Cost231.cc
│       ├── Lab1_Cpp_Friis.cc
│       ├── Lab1_Py_TwoRay.py
│       ├── Lab1_Py_Cost231.py
│       └── Lab1_Py_Friis.py
│
│
├── Lab-02-WiFiPerformance/       
│   ├── docs/                     
│   │   ├── Lab-02-Instructions.pdf
│   │   └── deliverables.md           # What to submit for Lab 2
│   └── code/                     
│       ├── Lab2_Cpp_Scenario1.cc
│       ├── Lab2_Cpp_Scenario2.cc
│       ├── Lab2_Py_Scenario1.py
│       └── Lab2_Py_Scenario2.py
│   
│
├── Lab-03-Adhoc/                 
│   ├── docs/                     
│   │   ├── Lab-03-Instructions.pdf
│   │   └── deliverables.md           # What to submit for Lab 3
│   └── code/                     
│       ├── Lab3_Cpp_Adhoc.cc
│       ├── Lab3_Cpp_PayloadSweep.cc
│       ├── Lab3_Cpp_TCP.cc
│       ├── Lab3_Cpp_Hidden.cc
│       ├── Lab3_Py_Adhoc.py
│       ├── Lab3_Py_PayloadSweep.py
│       ├── Lab3_Py_TCP.py
│       └── Lab3_Py_Hidden.py
│
│
└── Lab-04-LTE/                    
    ├── docs/                     
    │   ├── Lab-04-Instructions.pdf
    │   └── deliverables.md           # What to submit for Lab 4
    └── code/                     
        ├── Lab4_Cpp_LTE.cc
        └── Lab4_Py_LTE.py
````

Build artifacts (object files, NetAnim XML, pcap traces) reside in your local ns-3 build tree, not in this repository.

---

## Prerequisites

- **Ubuntu 22.04+ / WSL 2 / Fedora 35+ / macOS Sonoma**  
- **Disk:** ≥ 10 GB free  
- **RAM:** ≥ 4 GB (8 GB+ recommended)  
- **Dependencies:** see [common/setup.md](common/setup.md)  

---

## Building & Running ns-3

1. **Download & extract**  
   ```bash
   cd ~
   wget https://www.nsnam.org/releases/ns-allinone-3.40.tar.bz2
   tar xjf ns-allinone-3.40.tar.bz2
   cd ns-allinone-3.40
   ````

2. **Build all components**

   ```bash
   ./build.py \
     --enable-examples \
     --enable-tests \
     --enable-python-bindings \
     --qmake-path /usr/lib/qt5/bin/qmake
   ```

3. **Configure & compile via CMake**

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
   ./build/src/core/examples/hello-simulator
   ```

5. **Run Python script**

   ```bash
   export PYTHONPATH=~/ns-allinone-3.40/ns-3.40/build/bindings:$PYTHONPATH
   python3 scratch/hello_py.py
   ```

For detailed steps, see [common/setup.md](common/setup.md).

---

## Lab Overviews

| Lab                      | Theme                                                  | Deliverables                  |
| ------------------------ | ------------------------------------------------------ | ----------------------------- |
| **Lab 0** – Introduction | Installation, “Hello ns-3!” demo, NetAnim basics       | see `Lab-00-Introduction/`    |
| **Lab 1** – Propagation  | Two-Ray vs Cost231 vs Friis loss models                | see `Lab-01-Propagation/`     |
| **Lab 2** – WiFi Perf.   | Payload/rate sweeps, hidden-terminal in infrastructure | see `Lab-02-WiFiPerformance/` |
| **Lab 3** – Ad-hoc       | Multi-hop UDP/TCP chains, hidden-terminal in ad-hoc    | see `Lab-03-Adhoc/`           |
| **Lab 4** – LTE Module   | EPC, eNodeB/UE, antenna patterns, DL/UL throughput     | see `Lab-04-LTE/`             |

Each lab’s `deliverables.md` specifies exactly what to submit.

---

## Troubleshooting Cheat Sheet

Common issues and fixes are consolidated in [common/troubleshooting.md](common/troubleshooting.md). Key examples:

* **ImportError: no module named ns.core**
  → `export PYTHONPATH=~/ns-allinone-3.40/ns-3.40/build/bindings:$PYTHONPATH`
* **DsssRate5.5Mbps mode typo**
  → Use exact Doxygen string: `DsssRate5_5Mbps`
* **RTS/CTS has no effect**
  → Set threshold *before* device installation:
  `Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold","0")`
* **FlowMonitor reports zero bytes**
  → Install FlowMonitor *before* starting applications

---
