
## **Enabling Visualization**

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
