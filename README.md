# advanceWirelessNetworks**Lab 0: Introduction to ns-3 (Revised for 2025)**
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

## **Part 2: Exploring ns-3 Documentation**

Before writing any code, become familiar with **where** to find information:

1. **ns-3 Tutorial (HTML)**

   * **Link:** [https://www.nsnam.org/docs/release/3.40/tutorial/html/index.html](https://www.nsnam.org/docs/release/3.40/tutorial/html/index.html)
   * **Key Sections for Lab 0:**

     * *Chapter 1: Introduction*
     * *Chapter 2: Installing ns-3* (overview)
     * *Chapter 3: C++ Tutorial*
     * *Chapter 11: Python Tutorial* (starts on binding basics)

2. **C++ API Reference (Doxygen)**

   * **Link:** [https://www.nsnam.org/doxygen/group\_\_core.html](https://www.nsnam.org/doxygen/group__core.html)
   * Useful classes:

     * `ns3::Simulator`
     * `ns3::NodeContainer`
     * `ns3::PointToPointHelper`
     * etc.

3. **Python API Reference**

   * **Link:** [https://www.nsnam.org/doxygen/python/index.html](https://www.nsnam.org/doxygen/python/index.html)

4. **Module-Specific Documentation**

   * For any new module (e.g. wifi, internet, lte, flow-monitor), open:

     ```
     https://www.nsnam.org/docs/release/3.40/doxygen/<module>_html/index.html  
     ```
   * Example: WiFi propagation loss model reference:
     [https://www.nsnam.org/docs/release/3.40/doxygen/classns3\_1\_1FriisPropagationLossModel.html](https://www.nsnam.org/docs/release/3.40/doxygen/classns3_1_1FriisPropagationLossModel.html)

5. **General Troubleshooting**

   * Consult the **ns-3 FAQ and Issues** page:
     [https://www.nsnam.org/docs/release/3.40/faq/](https://www.nsnam.org/docs/release/3.40/faq/)
   * If build or runtime errors occur, search the **ns-3 Users’ Mailing List archives**:
     [http://mailman.isi.edu/pipermail/ns-users/](http://mailman.isi.edu/pipermail/ns-users/)

> **Tip:** Bookmark these documentation pages now. You’ll refer to them in every subsequent lab.

---

## **Part 3: Creating Your First Simulation Script (Incremental Steps)**

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

## **Part 4: Enabling Visualization**

Seeing packets “move” in ns-3 helps build intuition. We will use **NetAnim** (XML-based animator) for C++ and Python.

### **4.1. Task 4.1: Instrument Your C++ Script for NetAnim**

1. **Amend `hello-sim.cc` to generate an animation file**
   Although “Hello, Simulator” has no network traffic, we can still demonstrate NetAnim by adding a simple point-to-point link between two nodes and sending a dummy packet.

   * **Updated `hello-sim-anim.cc`** (in `scratch/`):

     ```cpp
     #include "ns3/core-module.h"
     #include "ns3/network-module.h"
     #include "ns3/internet-module.h"
     #include "ns3/point-to-point-module.h"
     #include "ns3/applications-module.h"
     #include "ns3/netanim-module.h"
     using namespace ns3;

     int main (int argc, char *argv[])
     {
       Time::SetResolution (Time::NS);
       LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
       LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);

       // 1. Create 2 nodes
       NodeContainer nodes;
       nodes.Create (2);

       // 2. Configure point-to-point channel
       PointToPointHelper p2p;
       p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
       p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
       NetDeviceContainer devices = p2p.Install (nodes);

       // 3. Install Internet stack
       InternetStackHelper stack;
       stack.Install (nodes);

       // 4. Assign IP addresses
       Ipv4AddressHelper address;
       address.SetBase ("10.1.1.0", "255.255.255.0");
       Ipv4InterfaceContainer interfaces = address.Assign (devices);

       // 5. Create UdpEchoServer on node1
       UdpEchoServerHelper echoServer (9);
       ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
       serverApps.Start (Seconds (1.0));
       serverApps.Stop (Seconds (10.0));

       // 6. Create UdpEchoClient on node0; send to node1
       UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
       echoClient.SetAttribute ("MaxPackets", UintegerValue (5));
       echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
       echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
       ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
       clientApps.Start (Seconds (2.0));
       clientApps.Stop (Seconds (10.0));

       // 7. Set up NetAnim
       AnimationInterface anim ("hello-animation.xml");
       // Optionally set node colors or positions:
       anim.SetConstantPosition (nodes.Get (0), 10.0, 10.0);
       anim.SetConstantPosition (nodes.Get (1), 50.0, 10.0);

       // 8. Run simulation
       Simulator::Stop (Seconds (10.0));
       Simulator::Run ();
       Simulator::Destroy ();
       return 0;
     }
     ```

2. **Build & Run**

   ```bash
   cd ~/ns-3.40  
   ./waf configure --enable-examples --enable-tests  
   ./waf build  
   ./waf --run hello-sim-anim
   ```

   * This should produce `hello-animation.xml` in the ns-3 root directory.

3. **Launch NetAnim**

   ```bash
   netanim ~/ns-3.40/hello-animation.xml
   ```

   * **Expected Behavior**: A GUI window appears, showing two nodes at coordinates (10,10) and (50,10). You should see 5 UDP echo request/response “packets” animating across the link at 2 s, 3 s, 4 s…
   * **Likely Issue (#12)**:

     ```
     netanim: command not found
     ```

     **Solution:**

     * Ensure you installed NetAnim (`sudo apt-get install netanim`).
     * Or locate its binary: `which NetAnim` or `which netanim`.
     * If you built NetAnim manually earlier, run the GUI from its install path.

4. **Troubleshooting Checklist (Visualization)**

   * **Check**: Is `hello-animation.xml` created?

     * If not, confirm you included `#include "ns3/netanim-module.h"` and the `AnimationInterface` line.
   * **Check**: Are nodes placed at distinct coordinates?

     * If both nodes overlap, change `(10,10)` and `(50,10)` to other values.
   * **Check**: Do you see traffic events in the ns-3 console logs?

     * In the program, logging for `UdpEchoClientApplication` and `UdpEchoServerApplication` is enabled. You should see log lines printed at 2 s onward.
   * **Check**: Does NetAnim freeze?

     * Use a small number of packets and short simulation time (e.g. 10 s) to keep animation light.

---

### **4.2. Task 4.2: Instrument Your Python Script for NetAnim**

> **Goal:** Produce the same animation from Python.

1. **Create `hello_py_anim.py`** in `scratch/`:

   ```python
   #!/usr/bin/env python3
   import ns.core
   import ns.network
   import ns.internet
   import ns.point_to_point
   import ns.applications
   import ns.netanim

   def main():
       ns.core.Time.SetResolution(ns.core.Time.NS)
       # 1. Create 2 nodes
       nodes = ns.network.NodeContainer()
       nodes.Create(2)

       # 2. Configure point-to-point channel
       p2p = ns.point_to_point.PointToPointHelper()
       p2p.SetDeviceAttribute("DataRate", ns.core.StringValue("5Mbps"))
       p2p.SetChannelAttribute("Delay", ns.core.StringValue("2ms"))
       devices = p2p.Install(nodes)

       # 3. Install internet stack
       stack = ns.internet.InternetStackHelper()
       stack.Install(nodes)

       # 4. Assign IP addresses
       address = ns.internet.Ipv4AddressHelper()
       address.SetBase(ns.network.Ipv4Address("10.1.1.0"), ns.network.Ipv4Mask("255.255.255.0"))
       interfaces = address.Assign(devices)

       # 5. Create UdpEchoServer on node1
       echoServer = ns.applications.UdpEchoServerHelper(9)
       serverApps = echoServer.Install(nodes.Get(1))
       serverApps.Start(ns.core.Seconds(1.0))
       serverApps.Stop(ns.core.Seconds(10.0))

       # 6. Create UdpEchoClient on node0
       echoClient = ns.applications.UdpEchoClientHelper(interfaces.GetAddress(1), 9)
       echoClient.SetAttribute("MaxPackets", ns.core.UintegerValue(5))
       echoClient.SetAttribute("Interval", ns.core.TimeValue(ns.core.Seconds(1.0)))
       echoClient.SetAttribute("PacketSize", ns.core.UintegerValue(1024))
       clientApps = echoClient.Install(nodes.Get(0))
       clientApps.Start(ns.core.Seconds(2.0))
       clientApps.Stop(ns.core.Seconds(10.0))

       # 7. Set up NetAnim
       anim = ns.netanim.AnimationInterface("hello_py_animation.xml")
       anim.SetConstantPosition(nodes.Get(0), 10.0, 10.0)
       anim.SetConstantPosition(nodes.Get(1), 50.0, 10.0)

       # 8. Run simulation
       ns.core.Simulator.Stop(ns.core.Seconds(10.0))
       ns.core.Simulator.Run()
       ns.core.Simulator.Destroy()

   if __name__ == "__main__":
       main()
   ```

2. **Make It Executable & Run**

   ```bash
   chmod +x ~/ns-3.40/scratch/hello_py_anim.py  
   # Ensure PYTHONPATH is set as before:
   export PYTHONPATH=~/ns-3.40/build/bindings:$PYTHONPATH  
   ./waf --pyrun scratch/hello_py_anim.py
   ```

   * **Expected**: Creates `hello_py_animation.xml`.
   * **Likely Issue (#13)**:

     ```
     AttributeError: module 'ns.netanim' has no attribute 'AnimationInterface'
     ```

     **Solution:**

     * Confirm `--enable-python-bindings` during `waf configure`.
     * Check that you ran `./waf build` *after* enabling Python bindings.
     * If still failing, rebuild ns-3 from scratch.

3. **Launch the animation**

   ```bash
   netanim ~/ns-3.40/hello_py_animation.xml
   ```

   * **Expected Behavior**: Same as C++ version.

4. **Troubleshooting Checklist (Python Visualization)**

   * **Check**: Does `hello_py_animation.xml` exist?

     * If not, ensure `ns.netanim.AnimationInterface(...)` is present in your script.
   * **Check**: Does `PYTHONPATH` include `build/bindings`?
   * **Check**: Are all required Python ns-3 imports working?

     * Try in Python REPL:

       ```python
       >>> import ns.core, ns.network, ns.internet, ns.point_to_point, ns.applications, ns.netanim
       ```

---

## **Part 5: Navigating ns-3 Documentation (Step-by-Step Guide)**

Whenever you need to find out **how to do X** (e.g., “Set up a point-to-point link” or “Use WiFi in ad-hoc mode”), follow this incremental path:

1. **Identify the module you need** (e.g., `point-to-point`, `wifi`, `lte`, `internet`).

2. **Open the Doxygen index for that module**:

   ```
   https://www.nsnam.org/docs/release/3.40/doxygen/group__<module>.html
   ```

   Example:

   * Point-to-Point: [https://www.nsnam.org/docs/release/3.40/doxygen/group\_\_point\_\_to\_\_point.html](https://www.nsnam.org/docs/release/3.40/doxygen/group__point__to__point.html)
   * WiFi: [https://www.nsnam.org/docs/release/3.40/doxygen/group\_\_wifi.html](https://www.nsnam.org/docs/release/3.40/doxygen/group__wifi.html)

3. **Find the Helper class** you need (e.g., `PointToPointHelper`, `YansWifiPhyHelper`).

4. **Open the Helper’s documentation page** to see its methods and attributes.

5. **Search for a complete example** in `examples/` or `tutorial/`.

   * `examples/point-to-point/first.cc` or `tutorial/` for usage patterns.

6. **For Python**:

   * Check the Python tutorial (Chapter 11) to see how the same Helper is exposed in ns-3’s Python API.
   * Use autocompletion (`ns.core.` + `Tab`) in a Python REPL to explore available classes.

> **Example**:
> You want to install the Internet Stack on nodes.
>
> 1. Module: `internet`.
> 2. Visit: [https://www.nsnam.org/docs/release/3.40/doxygen/group\_\_internet.html](https://www.nsnam.org/docs/release/3.40/doxygen/group__internet.html)
> 3. Find `InternetStackHelper`: [https://www.nsnam.org/docs/release/3.40/doxygen/classns3\_1\_1InternetStackHelper.html](https://www.nsnam.org/docs/release/3.40/doxygen/classns3_1_1InternetStackHelper.html)
> 4. Read “Install( )” method signature.
> 5. Check the C++ tutorial or `examples/internet/tcp-example.cc`.
> 6. For Python, check:
>
>    ```python
>    import ns.internet
>    dir(ns.internet.InternetStackHelper)
>    ```

---

## **Part 6: Incremental Troubleshooting Strategies**

Whenever you hit an error, follow these steps:

1. **Read the Error Carefully**

   * Identify which file and line number triggered it.
   * Determine if it’s a **compile-time error** (e.g., missing header, undefined symbol) or **runtime error** (Python import, simulation crash).

2. **Consult ns-3 Documentation**

   * If a C++ class is unknown, search the **Doxygen** for that class.
   * If a Python import fails, check your `PYTHONPATH` or rebuild Python bindings.

3. **Verify Module Inclusions**

   * For C++:

     ```cpp
     #include "ns3/ns3-module.h"
     ```

     brings in everything. Otherwise, explicitly include submodules (e.g., `"ns3/point-to-point-module.h"`).

   * For Python:

     ```python
     import ns.core
     import ns.network
     # etc.
     ```

     Make sure each is spelled correctly (case-sensitive).

4. **Check Build Logs** (for C++)

   * Run:

     ```bash
     ./waf configure --enable-examples --enable-tests --check-cxx-compiler
     ./waf build -v
     ```

     The `-v` flag shows more detailed output.

5. **Validate Environment Variables**

   * **C++**: Usually none needed beyond being in the ns-3 root when using `waf`.
   * **Python**:

     ```
     export PYTHONPATH=~/ns-3.40/build/bindings:$PYTHONPATH
     ```

     * If you open a new shell, re-export.
     * Confirm with:

       ```bash
       echo $PYTHONPATH
       ```

6. **Check Correct Version**

   * If tutorials reference ns-3.30 but you’re on 3.40, class names may differ. Always cross-check your installed version’s Doxygen (e.g., replace “3.30” with “3.40” in URLs).

7. **Ask for Help Early**

   * Best practice: If you spend more than **15 minutes** on a single error, stop and ask the TA or post a question on ns-3’s user mailing list.
   * When asking, provide:

     * The **exact error message**.
     * The **code snippet** that triggered it.
     * What you have tried.

---

## **Part 7: Lab 0 Deliverables**

1. **A single PDF report** (named `Lab0_NS3_Report.pdf`) containing:

   * **Section 1: Installation Verification**

     * Screen captures (or pasted console logs) showing:

       * `./waf --run hello-simulator` output in C++.
       * `./waf --pyrun scratch/hello_py.py` output in Python.

   * **Section 2: Documentation Walkthrough**

     * A short paragraph (1–2 sentences each) describing:

       1. Where you found the C++ API pages for `Simulator`, `NodeContainer`, and `PointToPointHelper`. Provide links.
       2. Where you located the Python binding documentation (link to Python tutorial).

   * **Section 3: “Hello, Simulator!” Code Snippets**

     * Paste your final `hello-sim.cc` and `hello_py.py` code (with your modifications).
     * Under each snippet, bullet-point the key lines you added and why.

   * **Section 4: Visualization Screenshots**

     * Two screenshots (or embedded images) of **NetAnim** running:

       1. C++ version (`hello-animation.xml`) – show node positions and a packet animation frame.
       2. Python version (`hello_py_animation.xml`).
     * Briefly explain in 2–3 sentences how enabling `AnimationInterface` created that XML.

   * **Section 5: Troubleshooting Log**

     * For each “Likely Issue” from this lab (1–13), write:

       1. A one-sentence description of the cause.
       2. The exact command or code change you applied to fix it.

2. **Name your Python binding scripts** exactly:

   * `hello_py.py`
   * `hello_py_anim.py`

3. **Name your C++ scripts** exactly:

   * `hello-sim.cc`
   * `hello-sim-anim.cc`

4. **Submit all four scripts** along with the PDF in a single compressed archive named `Lab0_Submission.zip`.

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

---

### Congratulations!

You have successfully completed **Lab 0**:

* Installed ns-3 (both C++ & Python).
* Written and run your first minimal simulations.
* Learned how to visualize packet flows in NetAnim.
* Gained experience navigating the ns-3 documentation and troubleshooting common issues.

In **Lab 1**, we will build on this foundation by exploring wireless simulation, propagation models, and WiFi traffic—again providing both C++ and Python examples, incremental tasks, direct doc links, and more visualization.
