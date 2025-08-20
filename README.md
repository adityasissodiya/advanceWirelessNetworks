# Advanced Wireless Networks – ns-3 Lab Series (2025 Edition)

> **Course code:** D7030E  
> **University:** Luleå University of Technology  
> **Languages:** C++ **or** Python (choose one per lab)  
> **ns-3 version:** 3.40 (frozen & tested; later versions not guaranteed)

This repository contains **five incremental laboratories (Lab-00 through Lab-04)** for hands-on wireless networking using the ns-3 simulator.  
Each lab can be completed in **either C++ or Python**. You only need to choose one language path per lab (not both).

---

## Repository Structure

```

.
├── README.md                # This file
├── common/                  # Shared resources
│   ├── setup.md             # Native installation guide (Linux, macOS, WSL)
│   ├── troubleshooting.md   # Common pitfalls & fixes
│   ├── simulation\_background.md
│   ├── links.md             # ns-3 Doxygen, tutorials, references
│   └── scripts/             # Utilities
│       ├── setup\_env.sh     # Environment helper (PYTHONPATH etc.)
│       ├── ci\_smoke.sh      # Smoke test for Docker/CI
│       ├── plot\_helper.py   # Simple CSV→plot utility
│       └── seed\_manager.sh
│
├── Lab-00-Introduction/     # Hello simulator + NetAnim warm-up
├── Lab-01-Propagation/      # Propagation loss models
├── Lab-02-WiFiPerformance/  # Infrastructure Wi-Fi performance
├── Lab-03-Adhoc/            # Multi-hop ad hoc networks
└── Lab-04-LTE/              # LTE cellular performance

````

Each `Lab-XX` folder contains:
- `docs/` → instructions PDF + `deliverables.md`
- `code/` → starter programs (both C++ and Python)
- You will create a `submission/` subfolder with your results.

---

## Environment Setup

### Option A: Docker / VS Code Devcontainer (recommended)

The repo ships with a Dockerfile pinned to **ns-3.40**.

```bash
make docker-build   # Build the ns-3.40 image
make shell          # Open a shell inside the container
source scripts/setup_env.sh  # Set ns-3 environment
````

Check Python bindings:

```bash
python3 -c "from ns import ns; print(ns.core.Simulator.Now())"
```

You can also open the repo in VS Code → “Reopen in Container” (requires Dev Containers extension).

### Option B: Native Installation

1. Install dependencies (Ubuntu example):

   ```bash
   sudo apt-get update && sudo apt-get install -y \
     build-essential cmake g++ python3 python3-dev python3-pip \
     qtbase5-dev qttools5-dev-tools \
     libxml2 libxml2-dev git pkg-config \
     python3-matplotlib gnuplot-x11 wireshark
   ```
2. Download & build ns-3.40:

   ```bash
   wget https://www.nsnam.org/releases/ns-allinone-3.40.tar.bz2
   tar xjf ns-allinone-3.40.tar.bz2
   cd ns-allinone-3.40
   ./build.py --enable-examples --enable-tests --enable-python-bindings
   ```
3. Set env vars (add to `.bashrc`):

   ```bash
   export NS3_DIR=$HOME/ns-allinone-3.40/ns-3.40
   export PYTHONPATH=$NS3_DIR/build/bindings/python:$PYTHONPATH
   export LD_LIBRARY_PATH=$NS3_DIR/build/lib:$LD_LIBRARY_PATH
   export PATH=$NS3_DIR:$NS3_DIR/build:$PATH
   ```

Verify:

```bash
$NS3_DIR/build/src/core/examples/hello-simulator
python3 -c "from ns import ns; print(ns.core.Simulator.Now())"
```

---

## Running Labs

* **C++:** Copy the lab’s `.cc` file into `ns-3.40/scratch/`, then:

  ```bash
  cd $NS3_DIR
  ./ns3 build
  ./ns3 run scratch/Lab1_Cpp_Friis --distance=100
  ```
* **Python:** Run directly:

  ```bash
  python3 Lab-01-Propagation/code/Lab1_Py_Friis.py --distance=100
  ```

Each lab requires multiple runs (e.g., sweeping distance, payload size, data rate, seeds). Use command-line arguments (`--rate`, `--payload`, `--seed`, etc.) as specified in the lab instructions.

Outputs:

* **Console logs** → redirect to `.txt`
* **FlowMonitor CSVs** → throughput, PDR results
* **Plots (PNG)** → use `plot_helper.py` or external tools
* **NetAnim XML** → open in NetAnim GUI, capture screenshot

---

## Lab Overview

| Lab                            | Theme                                      | Key Tasks                                                              |
| ------------------------------ | ------------------------------------------ | ---------------------------------------------------------------------- |
| **Lab 00** – Introduction      | Install check, Hello world, NetAnim basics | Run hello sim (C++/Py), generate XML, screenshot                       |
| **Lab 01** – Propagation       | Friis vs Two-Ray vs COST231 models         | Throughput vs distance, compare with measured path-loss                |
| **Lab 02** – Wi-Fi Performance | Infrastructure Wi-Fi                       | Rate sweep, payload sweep, hidden terminal (RTS/CTS)                   |
| **Lab 03** – Ad hoc            | Multi-hop Wi-Fi                            | UDP chain vs hops, payload sweep, TCP vs UDP, hidden terminal          |
| **Lab 04** – LTE               | Cellular downlink                          | Throughput vs offered rate, throughput vs distance, LTE trace analysis |

See each lab’s `deliverables.md` for exact filenames and submission requirements.

---

## Submission Guidelines

* Place all deliverables inside `Lab-XX-.../submission/`.
* **Must include** `choice.txt` with exactly one line: `C++` or `Python`.
* **File names must match** exactly what’s listed in `deliverables.md`.
* **CSV files:** include header rows.
* **Plots:** PNG format, axis labels, units, legends required.
* **NetAnim:** submit both `.xml` and a screenshot `.png`.

---

## Troubleshooting (Quick)

* **`ImportError: No module named ns.core`**
  → PYTHONPATH not set. Run `source scripts/setup_env.sh`.

* **Wi-Fi mode error (`no matching DataMode`)**
  → Use correct string: `DsssRate5_5Mbps`.

* **RTS/CTS toggle has no effect**
  → Set threshold *before* installing devices:

  ```cpp
  Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue("0"));
  ```

* **FlowMonitor shows zero throughput**
  → Must install FlowMonitor before starting applications.

* **Multi-hop results all zero**
  → You forgot to enable routing (e.g., OLSR).

For more, see [common/troubleshooting.md](common/troubleshooting.md).

---

## Support

* Check lab markdowns and deliverables first.
* Consult `common/troubleshooting.md`.
* Post questions on the course discussion forum.
* Contact a TA if stuck (include lab, command, and error output).

---

Happy simulating! 🚀
