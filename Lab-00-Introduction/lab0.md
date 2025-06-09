**Lab 0: Introduction to ns-3 (2025 Edition)**
*Choose Your Language: C++ and/or Python Binding*

---

## **Objectives**

By the end of this lab, you will be able to:

1. **Install ns-3** in two modes:

   * **C++ mode** (CMake-based workflow)
   * **Python-binding mode** (Python API)
2. **Navigate and leverage the ns-3 documentation** for both C++ and Python.
3. **Create a minimal “Hello, ns-3!” simulation** in C++ and in Python, incrementally:

   * Write, build/run, observe behavior, solve common issues.
4. **Enable visualization** (NetAnim) to watch packet exchanges.
5. **Develop basic troubleshooting skills**, so you never feel “lost” when following ns-3 tutorials.

Each task follows the pattern: **“small coding objective → likely issue → guided solution.”**
If you choose C++ only, skip the “Python version” steps. If you prefer Python, you can still refer to the C++ snippets for insight.

---

## **Prerequisites & Setup**

### 1. Your Development Machine

* **OS**: Ubuntu 22.04 LTS (or later) / Fedora 35+ / macOS Sonoma / Windows 10+ with WSL 2
* **Disk**: ≥ 10 GB free
* **RAM**: ≥ 4 GB (8 GB+ recommended)

### 2. Software Dependencies

#### On Ubuntu 22.04+ (or WSL 2 Ubuntu)

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential gcc g++ python3 python3-dev python3-pip \
    autoconf automake libxmu-dev cvs git cmake p7zip-full \
    python3-matplotlib python3-tk \
    qtbase5-dev qttools5-dev-tools \
    gnuplot-x11 wireshark net-tools
```

* **cmake** ≥ 3.10: required by ns-3 3.40+
* **qtbase5-dev** & **qttools5-dev-tools**: provide `qmake` & Qt headers for NetAnim
* **wireshark**, **gnuplot**: optional for trace analysis
* **python3-dev** & **python3-pip**: for Python bindings

> **Fedora**: replace `apt-get` with `dnf install` of the same packages.
> **macOS**:
>
> ```bash
> brew install python@3 cmake qt wireshark gnuplot
> pip3 install matplotlib
> ```
>
> then install any missing X11 libs via Brew as needed.

---

### **Windows Users: Installing WSL 2 + Ubuntu**

1. **Enable WSL & Virtual Machine Platform**

   ```powershell
   dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart
   dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart
   restart-computer
   ```
2. **Set WSL2 default**

   ```powershell
   wsl --set-default-version 2
   ```
3. **Install Ubuntu** from Microsoft Store (e.g. “Ubuntu 22.04 LTS”).
4. **In Ubuntu terminal**, update & install deps (same `apt-get` list above).
5. **(Optional) GUI support**: run an X server on Windows (VcXsrv/X410), then in Ubuntu:

   ```bash
   echo "export DISPLAY=$(grep -m1 nameserver /etc/resolv.conf|awk '{print $2}'):0" >>~/.bashrc
   source ~/.bashrc
   ```

---

## **Download & Build ns-3**

1. **Download ns-allinone-3.40** (or newer)

   ```bash
   cd ~
   wget https://www.nsnam.org/releases/ns-allinone-3.40.tar.bz2
   tar xjf ns-allinone-3.40.tar.bz2
   cd ns-allinone-3.40
   ```

2. **Build everything (C++ + examples/tests + Python bindings)**

   ```bash
   ./build.py --enable-examples --enable-tests --enable-python-bindings \
             --qmake-path /usr/lib/qt5/bin/qmake
   ```

   * The `--qmake-path` flag ensures NetAnim builds if `qmake` isn’t on your `$PATH`.
   * **Likely Issue #1**:

     ```
     Error: CMake not found
     ```

     **Fix:**
     `sudo apt-get install cmake`
   * **Likely Issue #2**:

     ```
     Could not find qmake
     ```

     **Fix:**
     install `qtbase5-dev qttools5-dev-tools`, then rerun with `--qmake-path`.

3. **Inspect build artifacts**

   ```bash
   ls ns-3.40/build
   ```

   You should see `src/`, `bindings/`, `utils/ns3.40-test-runner-default`, etc.


## **Part 1: Verifying the Build**

### **1.1. C++ Example**

1. **Locate `hello-simulator`**

   ```bash
   find ns-3.40/build/src -type f -executable | grep hello-simulator
   # e.g. build/src/core/examples/hello-simulator
   ```
2. **Run it**

   ```bash
   ./build/src/core/examples/hello-simulator
   ```

   **Expected**:

   ```
   Hello Simulator
   ```

> **Likely Issue #3**:
>
> ```
> ./hello-simulator: No such file or directory
> ```
>
> **Fix:**
> Ensure you’re invoking the correct path under `build/src/...`.

---

### **1.2. Python Example**

1. **Create & run** `scratch/hello_py.py`:

   ```bash
   cat > scratch/hello_py.py << 'EOF'
   #!/usr/bin/env python3
   import ns.core
   def main():
       ns.core.Time.SetResolution(ns.core.Time.NS)
       ns.core.Simulator.Schedule(ns.core.Seconds(1.0),
                                  lambda: print("Hello Python after 1s"))
       ns.core.Simulator.Stop(ns.core.Seconds(2.0))
       ns.core.Simulator.Run(); ns.core.Simulator.Destroy()
   if __name__=="__main__": main()
   EOF
   chmod +x scratch/hello_py.py
   export PYTHONPATH=~/ns-allinone-3.40/ns-3.40/build/bindings:$PYTHONPATH
   ./build/utils/ns3.40-test-runner-default --run scratch/hello_py.py
   ```

   *(Or simply)*

   ```bash
   python3 scratch/hello_py.py
   ```

   **Expected**:

   ```
   Hello Python after 1s
   ```

> **Likely Issue #4**:
>
> ```
> ImportError: No module named ns
> ```
>
> **Fix:**
> `export PYTHONPATH=~/ns-allinone-3.40/ns-3.40/build/bindings:$PYTHONPATH`

---

## **Part 2: Documentation & Troubleshooting**

### **2.1. Key ns-3 Docs**

* **C++ Tutorial (HTML)**
  [https://www.nsnam.org/docs/release/3.40/tutorial/html/index.html](https://www.nsnam.org/docs/release/3.40/tutorial/html/index.html)
* **Python Binding Tutorial**
  [https://www.nsnam.org/docs/release/3.40/python/tutorial/python\_bindings.html](https://www.nsnam.org/docs/release/3.40/python/tutorial/python_bindings.html)
* **C++ API Reference (Doxygen)**
  [https://www.nsnam.org/docs/release/3.40/doxygen/build/html/index.html](https://www.nsnam.org/docs/release/3.40/doxygen/build/html/index.html)

  * Modules → **core**, **network**, **wifi**, **propagation**, **applications**, **flow-monitor**, **netanim**
* **Python API Reference**
  [https://www.nsnam.org/docs/release/3.40/doxygen/python/index.html](https://www.nsnam.org/docs/release/3.40/doxygen/python/index.html)

### **2.2. Common Pitfalls**

| Symptom                                            | Cause                                        | Quick Fix                                             |
| -------------------------------------------------- | -------------------------------------------- | ----------------------------------------------------- |
| Missing CMake                                      | `cmake` not installed                        | `sudo apt-get install cmake`                          |
| “qmake not found” → NetAnim build fails            | Qt dev packages missing                      | `sudo apt-get install qtbase5-dev qttools5-dev-tools` |
| Example binary not in `build/src/applications/...` | wrong module—some examples live under `core` | `find build/src -type f -executable`                  |
| Python `ImportError: ns.core`                      | PYTHONPATH not set                           | `export PYTHONPATH=.../build/bindings:$PYTHONPATH`    |
| NetAnim XML never generated                        | missed `qmake` or forgot `--qmake-path`      | rebuild with `--qmake-path /usr/lib/qt5/bin/qmake`    |
| Test-runner lists no “hello\_sim” test             | test-runner is for tests, not examples       | invoke the example binary directly                    |

---

## **Part 3: Visualization with NetAnim**

1. **Create a minimal XML** from C++:

   ```cpp
   #include "ns3/netanim-module.h"
   // inside main:
   AnimationInterface anim("hello.xml");
   anim.SetConstantPosition(nodes.Get(0),10,10);
   anim.SetConstantPosition(nodes.Get(1),50,10);
   ```
2. **Run & view**:

   ```bash
   # your sim must output hello.xml
   netanim hello.xml
   ```

---

### **What’s Next?**

With Lab 0 complete, you have:

* A working ns-3 CMake build environment
* Python bindings ready
* The ability to run and modify minimal C++ and Python examples
* A mental map of ns-3’s documentation and troubleshooting steps
