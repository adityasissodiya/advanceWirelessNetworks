**Lab 0: Introduction to ns-3**
*Choose Your Language: C++ and/or Python Binding*

---

## **Objectives**

By the end of this lab, you will be able to:

1. **Install ns-3** in two modes:

   * **C++ mode** (classic workflow)
   * **Python-binding mode** (Python API)
2. **Navigate and leverage the ns-3 documentation** (both C++ & Python sections).
3. **Create a minimal “Hello, ns-3!” simulation** in C++ and in Python, incrementally:

   * Write, build/run, observe behavior, solve common issues.
4. **Enable visualization** (NetAnim or built-in animators) to watch packet exchanges.
5. **Develop basic troubleshooting skills**, so you never feel “lost” when following ns-3 tutorials.

Throughout this lab, **every task is broken into a small coding objective → likely issue → guided solution**. If you choose to work in C++ only, skip the “Python version” steps. If you prefer Python, you can still reference the C++ snippets for insight.

---

## **Prerequisites & Setup**

1. **Your Development Machine**

   * **Operating System**: Ubuntu 22.04 LTS (or later) / Fedora 35 + / macOS Sonoma (with Homebrew) / Windows Subsystem for Linux (WSL2).
   * **Disk Space**: ≥10 GB free.
   * **RAM**: ≥4 GB, although 8 GB is recommended for smoother builds and animations.

2. **Software Dependencies**
   Before starting, open a terminal and install the following (on Ubuntu):

   ```bash
   sudo apt-get update  
   sudo apt-get install -y build-essential gcc g++ python3 python3-dev python3-pip \
     autoconf automake libxmu-dev cvs git cmake p7zip-full \
     python3-matplotlib python3-tk qt5-qmake qt5-default \
     gnuplot-x11 wireshark net-tools\
     netanim
   ```

   * `python3-dev`, `python3-pip`: for building Python bindings.
   * `qt5-default`, `netanim`: for NetAnim visualization.
   * `wireshark`, `gnuplot`: optional but useful for later labs.
   * On **Fedora** or other RPM-based distros, use `dnf install` equivalents.
   * On **macOS**, install dependencies via Homebrew (`brew install python3 cmake qt wireshark`), then follow WSL instructions if needed.

---

**Windows Users: Installing Windows Subsystem for Linux (WSL2)**
If you are on Windows and want a native Linux environment for ns-3, follow these steps to install WSL2 and Ubuntu:

1. **Enable WSL and Virtual Machine Platform**
   Open PowerShell as Administrator and run:

   ```powershell
   dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart
   dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart
   ```

   Then **restart** your PC.

2. **Set WSL2 as the default version**
   After reboot, open PowerShell as Administrator again and run:

   ```powershell
   wsl --set-default-version 2
   ```

3. **Install Ubuntu from the Microsoft Store**

   * Open the Microsoft Store, search for **“Ubuntu 22.04 LTS”** (or later), and click **Install**.
   * Once installed, launch Ubuntu from the Start menu.
   * On first launch, you’ll be prompted to create a UNIX username and password.

4. **Update Ubuntu and install dependencies**
   In the Ubuntu terminal (now running under WSL2), run:

   ```bash
   sudo apt-get update  
   sudo apt-get upgrade -y  
   sudo apt-get install -y build-essential gcc g++ python3 python3-dev python3-pip \
     autoconf automake libxmu-dev cvs git cmake p7zip-full \
     python3-matplotlib python3-tk qt5-qmake qt5-default \
     gnuplot-x11 wireshark net-tools
   ```

   > **Note:**
   >
   > * You may need to run `sudo dpkg --add-architecture i386 && sudo apt-get update` if certain 32-bit libraries are required.
   > * To allow GUI apps (e.g., NetAnim), ensure you have a Windows X server (such as [VcXsrv](https://sourceforge.net/projects/vcxsrv/) or [X410](https://x410.dev/)) running, and set the DISPLAY environment variable in Ubuntu:
   >
   >   ```bash
   >   echo "export DISPLAY=$(grep -m 1 nameserver /etc/resolv.conf | awk '{print $2}'):0" >> ~/.bashrc  
   >   source ~/.bashrc
   >   ```

5. **Proceed with ns-3 download & build inside WSL**
   In your WSL Ubuntu shell, follow the existing instructions from **“Download ns-3 Release”** onward (e.g., `wget ns-3.40.tar.bz2`, `./build.py`, etc.).

---

Place these WSL instructions **right after** the bullet list of “Software Dependencies” under **Prerequisites & Setup**.

3. **Download ns-3 Release**
   At the time of writing (June 2025), the **latest stable** ns-3 release is **ns-3.40** (for example). Visit [https://www.nsnam.org/](https://www.nsnam.org/) and download `ns-allinone-3.40.tar.bz2`.

   ```bash
   cd ~  
   wget https://www.nsnam.org/releases/ns-allinone-3.40.tar.bz2  
   tar xjf ns-allinone-3.40.tar.bz2  
   cd ns-allinone-3.40  
   ```

   If a newer version exists (e.g., 3.41 in late 2025), replace “3.40” accordingly.

---

## **Part 1: Installing ns-3**

### **1.1. Building for C++**

> **Goal:** Compile ns-3 so you can run C++ examples.

1. **Run the build script**

   ```bash
   ./build.py --enable-examples --enable-tests
   ```

   * This will download and compile dependencies (e.g., pybindgen for Python, Boost if needed).
   * Expected outcome: many lines of compilation; finish with “\*\*\* Build Complete \*\*\*”.
   * **Likely Issue #1**:

     ```
     error: python3-dev not found
     ```

     **Solution:**

     * Ensure you installed `python3-dev` (or `python3-devel`) via your package manager.
     * Re-run `sudo apt-get install python3-dev` then rerun `./build.py`.

2. **Verify the build**
   Change into the ns-3 directory and run a small example:

   ```bash
   cd ns-3.40  
   ./waf --run hello-simulator
   ```

   You should see:

   ```
   Hello Simulator
   ```

   * **Likely Issue #2**:

     ```
     waf: command not found
     ```

     **Solution:**

     * You must be in the `ns-3.40/` directory (where `waf` lives).
     * If you cd’d elsewhere, `cd ~/ns-3.40` before running the command.

3. **Inspect the ns-3 folder structure** (guiding you through documentation)

   * `ns-3.40/`

     * `src/`: **C++ source code** for each module (core, network, wifi, lte, internet, etc.).
     * `build/`: compiled object files and binaries.
     * `scratch/`: your personal area to put **custom C++ or Python scripts**.
     * `examples/`: various C++ example scripts (organized by module).
     * `tutorial/`: tutorial C++ and Python code.
     * `bindings/`: Python binding generator scripts (pybindgen).
   * To see **all directories**, run:

     ```bash
     ls ­-1  
     ```
   * **Documentation Reference**:

     * Official ns-3 documentation:
       [https://www.nsnam.org/docs/release/3.40/](https://www.nsnam.org/docs/release/3.40/)
     * C++ API reference:
       [https://www.nsnam.org/doxygen/classns3\_1\_1Simulator.html](https://www.nsnam.org/doxygen/classns3_1_1Simulator.html)
     * Tutorial index (C++):
       [https://www.nsnam.org/docs/release/3.40/tutorial/html/index.html](https://www.nsnam.org/docs/release/3.40/tutorial/html/index.html)

### **1.2. Building for Python Binding**

> **Goal:** Enable ns-3’s Python API so you can write `*.py` simulations.

1. **Ensure Python binding prerequisite**

   * In `ns-3.40/bindings/` you’ll find `authorize.py`, `build.py`, and the generated `ns3module.cc`.
   * **Recommended**: create a **virtual environment** for Python:

     ```bash
     cd ~/ns-3.40  
     python3 -m venv venv_ns3  
     source venv_ns3/bin/activate  
     pip install pybindgen
     ```
   * **Likely Issue #3**:

     ```
     No module named 'pybindgen'
     ```

     **Solution:**

     * Activate the virtualenv (`source venv_ns3/bin/activate`).
     * Run `pip install pybindgen`.

2. **Generate and build the Python binding**

   ```bash
   cd ~/ns-3.40  
   ./waf configure --enable-python-bindings --with-python=$(which python3)  
   ./waf build
   ```

   * `--enable-python-bindings` triggers generation of `build/bindings/libns3python.so` (the shared library).
   * **Likely Issue #4**:

     ```
     ERROR: Could not find Python development headers
     ```

     **Solution:**

     * Install `sudo apt-get install python3-dev python3-pip`, then reconfigure.
     * Confirm `$(which python3)` points to the correct Python executable inside your virtualenv.

3. **Verify Python binding**
   Open a Python REPL in the same directory (`ns-3.40/`) and type:

   ```python
   >>> import ns.core  
   >>> ns.core.Simulator.Now()
   ```

   It should return `0s`. If you see no errors, Python binding works.

   * **Likely Issue #5**:

     ```
     ImportError: no module named ns3
     ```

     **Solution:**

     * Ensure your `PYTHONPATH` includes the `build/bindings/` directory.

       ```bash
       export PYTHONPATH=~/ns-3.40/build/bindings:$PYTHONPATH
       ```
     * Then retry `python3` and `import ns.core`.

4. **Documentation Guidance (Python)**

   * Python ns-3 tutorial:
     [https://www.nsnam.org/docs/release/3.40/python/tutorial/python\_bindings.html](https://www.nsnam.org/docs/release/3.40/python/tutorial/python_bindings.html)
   * Python API reference (auto-generated):
     [https://www.nsnam.org/doxygen/python/classns3\_1\_1Simulator.html](https://www.nsnam.org/doxygen/python/classns3_1_1Simulator.html)

---

**Creating Your First Simulation Script**

### **3.1. Task 3.1: “Hello, Simulator!” in C++**

> **Goal:** Copy a minimal ns-3 C++ program, compile it, run it, then modify it incrementally.

#### **Step 3.1.1: Copy and build the example**

1. **Locate `hello-simulator.cc`**
   In `ns-3.40/examples/tutorial/first.cc`, you’ll find a trivial simulation that prints “Hello Simulator”. If you want a dedicated file:

   ```bash
   cd ~/ns-3.40/scratch  
   cp ../examples/tutorial/first.cc hello-sim.cc  
   ```

2. **Compile only `hello-sim.cc`**

   ```bash
   cd ~/ns-3.40  
   ./waf --run hello-sim  
   ```

   You should see:

   ```
   Hello Simulator
   ```

3. **Likely Issue #6**:

   ```
   error: cannot find -lns3.40
   ```

   **Solution:**

   * Make sure you are running `./waf` from the **ns-3.40/** root directory (not from `scratch/`).
   * Delete any old build and re-build:

     ```bash
     ./waf clean  
     ./waf build  
     ./waf --run hello-sim  
     ```

#### **Step 3.1.2: Incremental Modification**

1. **Task 3.1.2a**: Change the **simulation start and stop times**.

   * Open `hello-sim.cc` in your editor. You’ll see something like:

     ```cpp
     #include "ns3/core-module.h"
     using namespace ns3;

     NS_LOG_COMPONENT_DEFINE ("HelloSimulator");

     int main (int argc, char *argv[])
     {
       Time::SetResolution (Time::NS);
       LogComponentEnable ("HelloSimulator", LOG_LEVEL_INFO);

       Simulator::Schedule (Seconds (1.0), &std::cout<<"Hello after 1 second\n");
       Simulator::Run ();
       Simulator::Destroy ();
       return 0;
     }
     ```
   * **Modify**: Schedule a second log at 2 s:

     ```diff
     + Simulator::Schedule (Seconds (2.0), &std::cout<<"Hello after 2 seconds\n");
     ```
   * **Build & Run** again:

     ```bash
     ./waf --run hello-sim
     ```

     **Expected Output:**

     ```
     Hello after 1 second
     Hello after 2 seconds
     ```
   * **Issue to Anticipate (#7)**:

     ```
     error: no matching function for call to 'std::cout'
     ```

     **Solution:**

     * Ensure you added: `#include <iostream>` at the top.
     * Confirm the syntax: `&std::cout<<"...";` should be changed to:

       ```cpp
       Simulator::Schedule (Seconds (2.0), [](){
         std::cout << "Hello after 2 seconds\n";
       });
       ```

2. **Task 3.1.2b**: **Add two nodes** and **print their creation**.

   * At the top of `main()`, add:

     ```cpp
     NodeContainer nodes;
     nodes.Create (2);
     std::cout << "Created " << nodes.GetN() << " nodes.\n";
     ```
   * Your main now prints:

     ```
     Created 2 nodes.
     Hello after 1 second
     Hello after 2 seconds
     ```
   * **Likely Issue (#8)**:

     ```
     error: ‘NodeContainer’ was not declared in this scope
     ```

     **Solution:**

     * Include the appropriate module at the top:

       ```cpp
       #include "ns3/core-module.h"
       #include "ns3/network-module.h"
       #include "ns3/node.h"
       ```
     * Or simply include the umbrella module:

       ```cpp
       #include "ns3/ns3-module.h"
       ```

       which brings in all standard ns-3 classes.

3. **Task 3.1.2c**: **Simulate for a fixed duration** (e.g., 5 s) and then exit.

   * After scheduling your “Hello” calls, add:

     ```cpp
     Simulator::Stop (Seconds (5.0));
     ```
   * Now, `./waf --run hello-sim` should end at 5 s.
   * **Issue (#9)**:

     ```
     runtime error: Segmentation fault in Simulator::Run()
     ```

     **Solution:**

     * Make sure you call `Simulator::Stop` *before* `Simulator::Run()`.
     * Ensure you do **not** call `Run()` twice. The order inside `main()` should be:

       ```cpp
       Time::SetResolution (Time::NS);
       // Schedule events
       Simulator::Stop (Seconds (5.0));
       Simulator::Run ();
       Simulator::Destroy ();
       ```

> **Checkpoint 3.1 (C++):**
>
> * You have created a minimal C++ ns-3 program, added two nodes, scheduled events, and stopped the simulator at a given time.
> * If you see “Created 2 nodes. Hello after 1 second… Hello after 2 seconds”, you’re ready for Part 4.

---

### **3.2. Task 3.2: “Hello, Simulator!” in Python**

> **Goal:** Replicate steps 3.1.1–3.1.2 using ns-3’s Python API.

#### **Step 3.2.1: Create a new Python script**

1. **File**: `~/ns-3.40/scratch/hello_py.py`
   Paste the following:

   ```python
   #!/usr/bin/env python3
   import ns.core

   def main():
       ns.core.Time.SetResolution(ns.core.Time.NS)
       # Print at time = 1.0 s
       ns.core.Simulator.Schedule(
           ns.core.Seconds(1.0),
           lambda: print("Hello from Python after 1 second")
       )
       # Print at time = 2.0 s
       ns.core.Simulator.Schedule(
           ns.core.Seconds(2.0),
           lambda: print("Hello from Python after 2 seconds")
       )
       # Create 2 nodes
       nodes = ns.network.NodeContainer()
       nodes.Create(2)
       print(f"Python: Created {nodes.GetN()} nodes")
       # Stop at 5 seconds
       ns.core.Simulator.Stop(ns.core.Seconds(5.0))
       ns.core.Simulator.Run()
       ns.core.Simulator.Destroy()

   if __name__ == "__main__":
       main()
   ```

2. **Make it executable**:

   ```bash
   chmod +x ~/ns-3.40/scratch/hello_py.py
   ```

3. **Run**:

   ```bash
   cd ~/ns-3.40
   ./waf --pyrun scratch/hello_py.py
   ```

   **Expected Output:**

   ```
   Python: Created 2 nodes
   Hello from Python after 1 second
   Hello from Python after 2 seconds
   ```

4. **Likely Issue (#10)**:

   ```
   ImportError: No module named ns
   ```

   **Solution:**

   * Confirm you set `PYTHONPATH` to `~/ns-3.40/build/bindings`:

     ```bash
     export PYTHONPATH=~/ns-3.40/build/bindings:$PYTHONPATH
     ```
   * If using a virtualenv, ensure it is activated (`source venv_ns3/bin/activate`).

5. **Likely Issue (#11)**:

   ```
   TypeError: Schedule() takes 2 positional arguments but 3 were given
   ```

   **Solution:**

   * The Python binding signature is `Schedule(delay, callback)`. Ensure you pass exactly two arguments: one `Time` and one no-arg function, e.g.:

     ```python
     ns.core.Simulator.Schedule(ns.core.Seconds(1.0), lambda: ...)
     ```

> **Checkpoint 3.2 (Python):**
>
> * You replicated the “Hello Simulator” in Python.
> * You understand how to create nodes, schedule events, and stop the simulation from Python.

---

## **Appendix A: Quick Reference Links**

* **ns-3 Main Site**: [https://www.nsnam.org](https://www.nsnam.org)
* **ns-3 Download & Release Notes**: [https://www.nsnam.org/releases](https://www.nsnam.org/releases)
* **C++ Tutorial (HTML)**: [https://www.nsnam.org/docs/release/3.40/tutorial/html/index.html](https://www.nsnam.org/docs/release/3.40/tutorial/html/index.html)
* **Python Tutorial**: [https://www.nsnam.org/docs/release/3.40/python/tutorial/python\_bindings.html](https://www.nsnam.org/docs/release/3.40/python/tutorial/python_bindings.html)
* **Doxygen (C++ API)**: [https://www.nsnam.org/docs/release/3.40/doxygen/index.html](https://www.nsnam.org/docs/release/3.40/doxygen/index.html)
* **Doxygen (Python API)**: [https://www.nsnam.org/docs/release/3.40/doxygen/python/index.html](https://www.nsnam.org/docs/release/3.40/doxygen/python/index.html)
* **NetAnim Documentation**: [https://www.nsnam.org/docs/release/3.40/netanim-guide.html](https://www.nsnam.org/docs/release/3.40/netanim-guide.html)
* **FAQ & Troubleshooting**: [https://www.nsnam.org/docs/release/3.40/faq/](https://www.nsnam.org/docs/release/3.40/faq/)
