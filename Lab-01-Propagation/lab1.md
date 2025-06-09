**Lab 1: Measuring the Influence of Radio‐Signal Attenuation on WiFi Performance**
*Choose Your Language: C++ and/or Python Binding*

---

## **Objectives**

By the end of this lab, you will be able to:

1. Configure a **basic two‐node WiFi scenario** in ns-3 (both C++ and Python).
2. **Swap propagation‐loss models**—Two-Ray Ground, Cost231-Hata, and Friis—and observe how each affects throughput.
3. **Measure application‐layer throughput** by analyzing pcap traces in Wireshark.
4. Guide yourself through the **ns-3 documentation** for propagation models (C++ & Python).
5. **Visualize** the two‐node link using NetAnim.
6. **Troubleshoot** common errors (e.g., missing headers, incorrect units, Python binding issues).

Each section is broken down into small tasks: you will code something, encounter a likely issue, and then learn how to resolve it. If you prefer C++ only, skip the Python steps; if you prefer Python, skip C++ specifics.

---

## **Prerequisites & Setup**

1. **ns-3 Installation Completed**

   * You should already have ns-3.40 (or later) built with both C++ and Python bindings (per Lab 0).
   * Make sure `./waf build` finishes without errors.
   * Verify your environment:

     ```bash
     cd ~/ns-3.40
     ./waf --run hello-sim     # C++ check
     ./waf --pyrun scratch/hello_py.py   # Python check
     ```

2. **Required Modules**

   * We will use the following ns-3 modules:

     * `core`, `network`, `internet`
     * `wifi`, `mobility`, `propagation-loss-model`, `propagation-delay-model`
     * `applications` (for UdpEcho)
     * `netanim` (for visualization)
   * In Python, these correspond to `ns.core`, `ns.network`, `ns.internet`, `ns.wifi`, etc.

3. **Documentation at Your Fingertips**

   * **Two-Ray Ground Propagation Loss** (C++):
     [https://www.nsnam.org/docs/release/3.40/doxygen/classns3\_1\_1TwoRayGroundPropagationLossModel.html](https://www.nsnam.org/docs/release/3.40/doxygen/classns3_1_1TwoRayGroundPropagationLossModel.html)
   * **Cost231 Propagation Loss** (C++):
     [https://www.nsnam.org/docs/release/3.40/doxygen/classns3\_1\_1Cost231PropagationLossModel.html](https://www.nsnam.org/docs/release/3.40/doxygen/classns3_1_1Cost231PropagationLossModel.html)
   * **Friis Propagation Loss** (C++):
     [https://www.nsnam.org/docs/release/3.40/doxygen/classns3\_1\_1FriisPropagationLossModel.html](https://www.nsnam.org/docs/release/3.40/doxygen/classns3_1_1FriisPropagationLossModel.html)
   * **Python Binding Overview for Propagation Models**:
     [https://www.nsnam.org/docs/release/3.40/python/tutorial/propgformula‐bindings.html](https://www.nsnam.org/docs/release/3.40/python/tutorial/propgformula‐bindings.html)
   * Whenever you need to look up a helper or attribute (e.g. `YansWifiChannel`, `SetPropagationLossModel`, `CalcRxPower`), search the corresponding Doxygen page under **ns-3 Modules → Propagation → Loss Models**.

4. **Wireshark Installed**

   * We will rely on pcap captures.
   * On Ubuntu (or WSL), run:

     ```bash
     sudo apt-get install wireshark
     ```

5. **NetAnim for Visualization**

   * If you want to see the two nodes and packets animating, install NetAnim (available via apt on Ubuntu).
   * We will generate an XML file and open it with `netanim`.

---

## **Overview of Lab Structure**

1. **Part 1: Build & Visualize a Basic Two‐Node WiFi Link**

   * 1.1: C++ version
   * 1.2: Python version
   * 1.3: Enable NetAnim & Verify basic UDP echo
2. **Part 2: Introduce Two-Ray Ground Model (C++ & Python)**

   * 2.1: Configure Two-Ray in code
   * 2.2: Run simulation at a known distance; measure received power.
   * 2.3: Enable pcap tracing; capture a small UDP exchange.
3. **Part 3: Vary Distance & Compute Border Range *dᵢ***

   * 3.1: Empirically find *dᵢ* where throughput ≈ 0.
   * 3.2: Automate runs over distances { *dᵢ*, 7*dᵢ*/8, …, *dᵢ*/8 } (8 values).
   * 3.3: Plot “throughput vs. distance” for Two-Ray.
4. **Part 4: Swap to Cost231-Hata and Friis Models**

   * 4.1: Modify code to use Cost231; rerun the 8 distances.
   * 4.2: Modify code to use Friis; rerun the 8 distances.
   * 4.3: Plot all three curves on one chart.
5. **Part 5: Real‐World Measurement (Optional)**

   * 5.1: Set up an ad-hoc WiFi network on two laptops, measure RSSI & path loss in a corridor.
   * 5.2: Plot measured path loss vs. Friis‐predicted path loss.
6. **Part 6: Troubleshooting & Reporting**

   * 6.1: Common pitfalls & how to fix.
   * 6.2: Report guidelines (what to include in your PDF).

---

## **Part 1: Build & Visualize a Basic Two‐Node WiFi Link**

### **1.1. Task: C++ – Minimal Two‐Node WiFi with UDP Echo**

> **Goal:** Write a simple C++ script that sets up two static WiFi nodes in ad-hoc mode, exchanges a few UDP packets, and stops. This is the skeleton before adding propagation models.

#### **Step 1.1.1: Create the C++ Script**

1. **File**: `~/ns-3.40/scratch/Lab1_Cpp_Base.cc`

   ```cpp
   #include "ns3/core-module.h"
   #include "ns3/network-module.h"
   #include "ns3/internet-module.h"
   #include "ns3/wifi-module.h"
   #include "ns3/mobility-module.h"
   #include "ns3/applications-module.h"
   #include "ns3/propagation-module.h" // includes loss & delay models
   #include "ns3/netanim-module.h"     // for visualization

   using namespace ns3;

   NS_LOG_COMPONENT_DEFINE ("Lab1_Cpp_Base");

   int 
   main (int argc, char *argv[])
   {
     // 1. Set up simulation parameters
     uint32_t packetSize = 1000; // bytes
     uint32_t maxPackets = 10;
     double interval = 1.0; // seconds between packets
     double simulationTime = 10.0; // seconds
     double distance = 100.0; // meters; placeholder

     CommandLine cmd;
     cmd.AddValue ("distance", "Distance between nodes in meters", distance);
     cmd.Parse (argc, argv);

     // 2. Create two nodes
     NodeContainer wifiNodes;
     wifiNodes.Create (2);

     // 3. Configure WiFi physical and channel
     //    (We will override the propagation model later; default is Friis here)
     YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
     YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
     wifiPhy.SetChannel (wifiChannel.Create ());
     wifiPhy.Set ("TxPowerStart", DoubleValue (16.0));   // 16 dBm
     wifiPhy.Set ("TxPowerEnd",   DoubleValue (16.0));
     wifiPhy.Set ("RxSensitivity", DoubleValue (-80.0));// -80 dBm sensitivity

     // 4. Configure WiFi MAC (Ad-hoc mode)
     WifiHelper wifiHelper;
     wifiHelper.SetStandard (WIFI_PHY_STANDARD_80211a);
     wifiHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                         "DataMode", StringValue ("OfdmRate6Mbps"),
                                         "ControlMode", StringValue ("OfdmRate6Mbps"));

     WifiMacHelper wifiMac;
     wifiMac.SetType ("ns3::AdhocWifiMac");
     NetDeviceContainer wifiDevices = wifiHelper.Install (wifiPhy, wifiMac, wifiNodes);

     // 5. Mobility: place nodes at (0,0,0) and (distance,0,0)
     MobilityHelper mobility;
     Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
     positionAlloc->Add (Vector (0.0, 0.0, 0.0));
     positionAlloc->Add (Vector (distance, 0.0, 0.0));
     mobility.SetPositionAllocator (positionAlloc);
     mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
     mobility.Install (wifiNodes);

     // 6. Install Internet stack and assign IPs
     InternetStackHelper internet;
     internet.Install (wifiNodes);
     Ipv4AddressHelper address;
     address.SetBase ("10.1.1.0", "255.255.255.0");
     Ipv4InterfaceContainer wifiInterfaces = address.Assign (wifiDevices);

     // 7. Create UDP Echo Server on node 1
     UdpEchoServerHelper echoServer (9);
     ApplicationContainer serverApps = echoServer.Install (wifiNodes.Get (1));
     serverApps.Start (Seconds (1.0));
     serverApps.Stop (Seconds (simulationTime));

     // 8. Create UDP Echo Client on node 0
     UdpEchoClientHelper echoClient (wifiInterfaces.GetAddress (1), 9);
     echoClient.SetAttribute ("MaxPackets", UintegerValue (maxPackets));
     echoClient.SetAttribute ("Interval", TimeValue (Seconds (interval)));
     echoClient.SetAttribute ("PacketSize", UintegerValue (packetSize));
     ApplicationContainer clientApps = echoClient.Install (wifiNodes.Get (0));
     clientApps.Start (Seconds (2.0));
     clientApps.Stop (Seconds (simulationTime));

     // 9. Enable pcap tracing
     wifiPhy.EnablePcap ("lab1-base-node0", wifiDevices.Get (0));
     wifiPhy.EnablePcap ("lab1-base-node1", wifiDevices.Get (1));

     // 10. Set up NetAnim (optional)
     AnimationInterface anim ("lab1-base-animation.xml");
     anim.SetConstantPosition (wifiNodes.Get (0), 10.0, 10.0);
     anim.SetConstantPosition (wifiNodes.Get (1), 50.0, 10.0);

     // 11. Run simulation
     Simulator::Stop (Seconds (simulationTime));
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
   ./waf --run "scratch/Lab1_Cpp_Base --distance=100.0"
   ```

   * **Expected Behavior:**

     * Two nodes at 100 m apart try to exchange 10 UDP echo packets at 6 Mbps.
     * If 100 m is beyond the default Friis range, you might see no packets received (0% throughput).
     * Two pcap files are generated: `lab1-base-node0-0.pcap` and `lab1-base-node1-0.pcap`.
     * `lab1-base-animation.xml` is created.

3. **Likely Issue (#1): “No packets received / zero throughput”**

   * **Cause:** At 100 m, Friis path loss at 5 GHz might drop below –80 dBm, so node 1 never hears node 0.
   * **Solution:**

     * Reduce distance to a smaller value (e.g., `--distance=50.0`).
     * Or lower the WiFi data mode to `OfdmRate1Mbps` (higher sensitivity) for testing:

       ```cpp
       wifiHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                          "DataMode", StringValue ("OfdmRate1Mbps"),
                                          "ControlMode", StringValue ("OfdmRate1Mbps"));
       ```
     * Rebuild & re-run.

4. **Likely Issue (#2): Compilation error “No member named ‘EnablePcap’”**

   * **Cause:** Either missing `#include "ns3/wifi-module.h"` or using an older ns-3 version.
   * **Solution:**

     * Confirm you included `wifi-module.h`.
     * Ensure you are on ns-3.40 (older versions put `EnablePcap` elsewhere).
     * If still failing, switch to:

       ```cpp
       wifiPhy.EnablePcapAll ("lab1-base");
       ```

       and parse the appropriate pcap.

---

### **1.2. Task: Python – Minimal Two‐Node WiFi with UDP Echo**

> **Goal:** Replicate the C++ scenario using ns-3’s Python API.

#### **Step 1.2.1: Create the Python Script**

1. **File**: `~/ns-3.40/scratch/Lab1_Py_Base.py`

   ```python
   #!/usr/bin/env python3
   import ns.core
   import ns.network
   import ns.internet
   import ns.wifi
   import ns.mobility
   import ns.applications
   import ns.propagation
   import ns.netanim

   def main():
       # 1. Parse command‐line parameters
       parser = ns.core.CommandLine()
       parser.AddValue ("distance", "Distance between nodes in meters", 100.0)
       parser.Parse ()
       distance = float (parser.GetGlobalValue ("distance"))

       # 2. Create two nodes
       wifiNodes = ns.network.NodeContainer()
       wifiNodes.Create (2)

       # 3. Configure WiFi phy and channel
       wifiChannel = ns.wifi.YansWifiChannelHelper.Default ()
       wifiPhy = ns.wifi.YansWifiPhyHelper.Default ()
       wifiPhy.SetChannel (wifiChannel.Create ())
       wifiPhy.Set ("TxPowerStart", ns.core.DoubleValue (16.0))
       wifiPhy.Set ("TxPowerEnd",   ns.core.DoubleValue (16.0))
       wifiPhy.Set ("RxSensitivity", ns.core.DoubleValue (-80.0))

       # 4. Configure WiFi MAC (ad‐hoc mode)
       wifiHelper = ns.wifi.WifiHelper()
       wifiHelper.SetStandard (ns.wifi.WIFI_PHY_STANDARD_80211a)
       wifiHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                           "DataMode",   ns.core.StringValue("OfdmRate6Mbps"),
                                           "ControlMode",ns.core.StringValue("OfdmRate6Mbps"))

       wifiMac = ns.wifi.WifiMacHelper()
       wifiMac.SetType ("ns3::AdhocWifiMac")
       wifiDevices = wifiHelper.Install (wifiPhy, wifiMac, wifiNodes)

       # 5. Mobility: place nodes
       posAlloc = ns.mobility.ListPositionAllocator()
       posAlloc.Add (ns.core.Vector (0.0, 0.0, 0.0))
       posAlloc.Add (ns.core.Vector (distance, 0.0, 0.0))
       mobility = ns.mobility.MobilityHelper()
       mobility.SetPositionAllocator (posAlloc)
       mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel")
       mobility.Install (wifiNodes)

       # 6. Internet stack & IP addressing
       internet = ns.internet.InternetStackHelper()
       internet.Install (wifiNodes)
       address = ns.internet.Ipv4AddressHelper()
       address.SetBase (ns.network.Ipv4Address("10.1.1.0"), ns.network.Ipv4Mask("255.255.255.0"))
       wifiInterfaces = address.Assign (wifiDevices)

       # 7. UDP Echo Server on node1
       echoServer = ns.applications.UdpEchoServerHelper (9)
       serverApps = echoServer.Install (wifiNodes.Get(1))
       serverApps.Start (ns.core.Seconds (1.0))
       serverApps.Stop  (ns.core.Seconds (10.0))

       # 8. UDP Echo Client on node0
       echoClient = ns.applications.UdpEchoClientHelper (wifiInterfaces.GetAddress(1), 9)
       echoClient.SetAttribute ("MaxPackets", ns.core.UintegerValue (10))
       echoClient.SetAttribute ("Interval",   ns.core.TimeValue (ns.core.Seconds (1.0)))
       echoClient.SetAttribute ("PacketSize", ns.core.UintegerValue (1000))
       clientApps = echoClient.Install (wifiNodes.Get(0))
       clientApps.Start (ns.core.Seconds (2.0))
       clientApps.Stop  (ns.core.Seconds (10.0))

       # 9. Enable pcap tracing
       wifiPhy.EnablePcap ("lab1-py-base-node0", wifiDevices.Get (0))
       wifiPhy.EnablePcap ("lab1-py-base-node1", wifiDevices.Get (1))

       # 10. NetAnim (optional)
       anim = ns.netanim.AnimationInterface ("lab1-py-base-animation.xml")
       anim.SetConstantPosition (wifiNodes.Get(0), 10.0, 10.0)
       anim.SetConstantPosition (wifiNodes.Get(1), 50.0, 10.0)

       # 11. Run simulation
       ns.core.Simulator.Stop (ns.core.Seconds (10.0))
       ns.core.Simulator.Run ()
       ns.core.Simulator.Destroy ()

   if __name__ == "__main__":
       main ()
   ```

2. **Build & Run**

   ```bash
   chmod +x ~/ns-3.40/scratch/Lab1_Py_Base.py
   # Ensure PYTHONPATH is set:
   export PYTHONPATH=~/ns-3.40/build/bindings:$PYTHONPATH
   ./waf --pyrun scratch/Lab1_Py_Base.py --distance=100.0
   ```

   * **Expected Behavior:** Same as C++ version. If 100 m is too far, zero packets; try `--distance=50.0`.

3. **Likely Issue (#3): “AttributeError: module ‘ns.wifi’ has no attribute ‘YansWifiChannelHelper’”**

   * **Cause:** Python binding not built correctly or missing import.
   * **Solution:**

     * Confirm `./waf configure --enable-python-bindings` was run before build.
     * In a Python REPL:

       ```python
       >>> import ns.wifi
       >>> dir(ns.wifi)
       ```

       * If `YansWifiChannelHelper` is missing, rebuild ns-3 with Python bindings.
     * Ensure `PYTHONPATH` includes `~/ns-3.40/build/bindings`.

---

### **1.3. Task: Visualize with NetAnim**

> **Goal:** Verify that you can watch the two nodes and UDP packets in NetAnim.

1. **Run the C++ animation**

   ```bash
   ./waf --run "scratch/Lab1_Cpp_Base --distance=50.0"
   netanim lab1-base-animation.xml
   ```

   * **Expected:** Two nodes at (10,10) and (50,10). At 2 s, you’ll see 10 UDP “packet” animations (colored arrows) going from node 0 to node 1, then echo replies returning to node 0.

2. **Run the Python animation**

   ```bash
   ./waf --pyrun scratch/Lab1_Py_Base.py --distance=50.0
   netanim lab1-py-base-animation.xml
   ```

3. **Likely Issue (#4): NetAnim cannot open XML**

   * **Cause:** The XML file wasn’t generated (script crashed early).
   * **Solution:**

     * Check the console for errors in running the script.
     * Ensure you included `#include "ns3/netanim-module.h"` (C++) or `import ns.netanim` (Python).
     * Rebuild and re-run.

---

## **Part 2: Introduce Two-Ray Ground Propagation Loss Model**

Until now, we’ve used the **default** channel (Friis in `YansWifiChannelHelper::Default()`). We will force the channel to use **Two-Ray Ground** to see how range and throughput differ.

### **2.1. Task: C++ – Configure Two-Ray Ground**

> **Goal:** Replace the default loss model with `TwoRayGroundPropagationLossModel` in C++.

1. **Modify `Lab1_Cpp_Base.cc`** → `Lab1_Cpp_TwoRay.cc`

   * Copy your base file:

     ```bash
     cp ~/ns-3.40/scratch/Lab1_Cpp_Base.cc ~/ns-3.40/scratch/Lab1_Cpp_TwoRay.cc
     ```
   * **Edit** (`Lab1_Cpp_TwoRay.cc`):

     1. **Include TwoRay header** (already done via `propagation-module.h`).
     2. **Set up custom channel** before installing PHY:

        ```diff
        - YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
        - YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
        - wifiPhy.SetChannel (wifiChannel.Create ());

        + // Create Two-Ray Ground channel instead of default Friis
        + YansWifiChannelHelper wifiChannel;
        + Ptr<TwoRayGroundPropagationLossModel> lossModel = CreateObject<TwoRayGroundPropagationLossModel> ();
        + wifiChannel.SetPropagationLoss ("ns3::TwoRayGroundPropagationLossModel",
        +                                 "SystemLoss", DoubleValue (1.0),
        +                                 "HeightAboveZ", DoubleValue (1.5));  // default antenna heights
        + // Use a constant‐speed delay model:
        + wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
        + YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
        + wifiPhy.SetChannel (wifiChannel.Create ());
        ```
     3. **Remove any other custom loss settings** if present.
   * The rest of the code (Mobility, Internet stack, UdpEcho, NetAnim) remains unchanged.

2. **Build & Run**

   ```bash
   cd ~/ns-3.40
   ./waf --run "scratch/Lab1_Cpp_TwoRay --distance=100.0"
   ```

   * If 100 m is still too far even for Two-Ray (likely, because –80 dBm Rx sensitivity and higher pathloss), reduce `--distance` until you see some packets. e.g. `--distance=50.0` or `25.0`.

3. **Measure Received Power**

   * Add code *after* echoClient installation to calculate Rx power at distance:

     ```cpp
     // After mobility.Install(wifiNodes) but before applications:
     Ptr<MobilityModel> mob0 = wifiNodes.Get (0)->GetObject<MobilityModel> ();
     Ptr<MobilityModel> mob1 = wifiNodes.Get (1)->GetObject<MobilityModel> ();
     double txPower = 16.0; // dBm
     double rxPower = lossModel->CalcRxPower (txPower, mob0, mob1);
     std::cout << "C++ TwoRay: RxPower at distance " << distance << " m = " 
               << rxPower << " dBm\n";
     ```
   * **Re-build & re-run** to see the printed Rx power.
   * **Likely Issue (#5): “‘lossModel’ was not declared”**

     * **Cause:** The variable `lossModel` was declared inside the wifiChannel block.
     * **Solution:**

       * Move the declaration of `lossModel` **above** the `wifiChannel.SetPropagationLoss` line so it’s in scope:

         ```cpp
         Ptr<TwoRayGroundPropagationLossModel> lossModel = CreateObject<TwoRayGroundPropagationLossModel> ();
         YansWifiChannelHelper wifiChannel;
         wifiChannel.SetPropagationLossObject (lossModel);
         // … follow with SetPropagationDelay …
         ```

---

### **2.2. Task: Python – Configure Two-Ray Ground**

> **Goal:** Repeat the same change in the Python script.

1. **Copy `Lab1_Py_Base.py` → `Lab1_Py_TwoRay.py`**

   ```bash
   cp ~/ns-3.40/scratch/Lab1_Py_Base.py ~/ns-3.40/scratch/Lab1_Py_TwoRay.py
   ```

2. **Edit** (`Lab1_Py_TwoRay.py`):

   * Replace the default channel lines with:

     ```python
     # Create Two-Ray Ground loss model object
     lossModel = ns.propagation.TwoRayGroundPropagationLossModel ()
     # Create channel
     wifiChannel = ns.wifi.YansWifiChannelHelper.Default ()
     wifiChannel.SetPropagationLossModel (lossModel)
     wifiChannel.SetPropagationDelayModel ("ns3::ConstantSpeedPropagationDelayModel")
     wifiPhy = ns.wifi.YansWifiPhyHelper.Default ()
     wifiPhy.SetChannel (wifiChannel.Create ())
     ```
   * Then, **after** setting mobility but before applications, print Rx power:

     ```python
     mob0 = wifiNodes.Get(0).GetObject(ns.mobility.MobilityModel)
     mob1 = wifiNodes.Get(1).GetObject(ns.mobility.MobilityModel)
     txPower = 16.0
     rxPower = lossModel.CalcRxPower (txPower, mob0, mob1)
     print (f"Python TwoRay: RxPower at {distance} m = {rxPower} dBm")
     ```

3. **Run**

   ```bash
   export PYTHONPATH=~/ns-3.40/build/bindings:$PYTHONPATH
   ./waf --pyrun scratch/Lab1_Py_TwoRay.py --distance=50.0
   ```

   * **Expected:** Printed Rx power (e.g. \~–60 dBm). If `distance=100.0`, maybe \~–90 dBm (below sensitivity), so no packets.

4. **Likely Issue (#6): “TypeError: SetPropagationLossModel() missing required positional argument”**

   * **Cause:** Old Python binding API expects slightly different syntax.
   * **Solution:**

     * Use:

       ```python
       wifiChannel.SetPropagationLoss ("ns3::TwoRayGroundPropagationLossModel",
                                      "SystemLoss", ns.core.DoubleValue (1.0),
                                      "HeightAboveZ", ns.core.DoubleValue (1.5))
       ```
     * Omit the explicit creation of `lossModel`. Then you can’t call `CalcRxPower` directly on the helper; instead, create a `lossModel` separately for measurement:

       ```python
       lossModel = ns.propagation.TwoRayGroundPropagationLossModel ()
       ```

---

## **Part 3: Find the “Border of Transmission Range” *dᵢ* and Measure Throughput**

Now that we have Two-Ray configured, let’s find the maximum distance *dᵢ* at which node 1 still receives any packets. Then we’ll systematically measure throughput at fractions of *dᵢ*.

### **3.1. Task: Empirically Determine *dᵢ* (C++)**

> **Goal:** Run a script that sweeps `distance` until the UDP echo receives exactly 0 packets. The last distance where **some** packets go through is *dᵢ*.

#### **Step 3.1.1: Write a Sweep Script**

1. **File**: `~/ns-3.40/scratch/Lab1_Cpp_Sweep.cc`

   ```cpp
   #include "ns3/core-module.h"
   #include "ns3/network-module.h"
   #include "ns3/internet-module.h"
   #include "ns3/wifi-module.h"
   #include "ns3/mobility-module.h"
   #include "ns3/applications-module.h"
   #include "ns3/propagation-module.h"

   using namespace ns3;

   NS_LOG_COMPONENT_DEFINE ("Lab1_Cpp_Sweep");

   int 
   main (int argc, char *argv[])
   {
     double dStart = 10.0;   // start distance (m)
     double dEnd   = 200.0;  // max distance to test (m)
     double dStep  = 10.0;   // step size (m)
     uint32_t packetSize = 1000;
     double interval = 1.0;
     uint32_t maxPackets = 10;

     CommandLine cmd;
     cmd.AddValue ("dStart", "Start distance (m)", dStart);
     cmd.AddValue ("dEnd",   "End distance (m)",   dEnd);
     cmd.AddValue ("dStep",  "Step for distance",   dStep);
     cmd.Parse (argc, argv);

     for (double distance = dStart; distance <= dEnd; distance += dStep)
       {
         // Setup (similar to Lab1_Cpp_TwoRay)
         NodeContainer wifiNodes;
         wifiNodes.Create (2);

         YansWifiChannelHelper wifiChannel;
         Ptr<TwoRayGroundPropagationLossModel> lossModel = CreateObject<TwoRayGroundPropagationLossModel> ();
         wifiChannel.SetPropagationLossObject (lossModel);
         wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");

         YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
         wifiPhy.SetChannel (wifiChannel.Create ());
         wifiPhy.Set ("TxPowerStart", DoubleValue (16.0));
         wifiPhy.Set ("TxPowerEnd",   DoubleValue (16.0));
         wifiPhy.Set ("RxSensitivity", DoubleValue (-80.0));

         WifiHelper wifiHelper;
         wifiHelper.SetStandard (WIFI_PHY_STANDARD_80211a);
         wifiHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                             "DataMode",   StringValue ("OfdmRate6Mbps"),
                                             "ControlMode",StringValue ("OfdmRate6Mbps"));

         WifiMacHelper wifiMac;
         wifiMac.SetType ("ns3::AdhocWifiMac");
         NetDeviceContainer wifiDevices = wifiHelper.Install (wifiPhy, wifiMac, wifiNodes);

         MobilityHelper mobility;
         Ptr<ListPositionAllocator> posAlloc = CreateObject<ListPositionAllocator> ();
         posAlloc->Add (Vector (0.0, 0.0, 0.0));
         posAlloc->Add (Vector (distance, 0.0, 0.0));
         mobility.SetPositionAllocator (posAlloc);
         mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
         mobility.Install (wifiNodes);

         InternetStackHelper internet;
         internet.Install (wifiNodes);
         Ipv4AddressHelper address;
         address.SetBase ("10.1.1.0", "255.255.255.0");
         Ipv4InterfaceContainer wifiInterfaces = address.Assign (wifiDevices);

         UdpEchoServerHelper echoServer (9);
         ApplicationContainer serverApps = echoServer.Install (wifiNodes.Get (1));
         serverApps.Start (Seconds (1.0));
         serverApps.Stop (Seconds (5.0));

         UdpEchoClientHelper echoClient (wifiInterfaces.GetAddress (1), 9);
         echoClient.SetAttribute ("MaxPackets", UintegerValue (maxPackets));
         echoClient.SetAttribute ("Interval",   TimeValue (Seconds (interval)));
         echoClient.SetAttribute ("PacketSize", UintegerValue (packetSize));
         ApplicationContainer clientApps = echoClient.Install (wifiNodes.Get (0));
         clientApps.Start (Seconds (2.0));
         clientApps.Stop (Seconds (5.0));

         // Enable FlowMonitor to count received packets
         FlowMonitorHelper flowmonHelper;
         Ptr<FlowMonitor> flowmon = flowmonHelper.InstallAll ();

         Simulator::Stop (Seconds (5.0));
         Simulator::Run ();

         // Analyze results
         flowmon->CheckForLostPackets ();
         Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmonHelper.GetClassifier ());
         std::map<FlowId, FlowMonitor::FlowStats> stats = flowmon->GetFlowStats ();

         uint32_t rxPackets = 0;
         for (auto const &kv : stats)
           {
             Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (kv.first);
             // We only care about flow from node0→node1
             if (t.sourceAddress == wifiInterfaces.GetAddress (0) &&
                 t.destinationAddress == wifiInterfaces.GetAddress (1))
               {
                 rxPackets = kv.second.rxPackets;
               }
           }

         Simulator::Destroy ();

         std::cout << "Distance: " << distance 
                   << " m, RxPackets: " << rxPackets << "\n";

         if (rxPackets == 0)
           {
             std::cout << "--> No packets received at distance " 
                       << distance << " m. Stopping sweep.\n";
             break;
           }
       }

     return 0;
   }
   ```

2. **Build & Run**

   ```bash
   cd ~/ns-3.40
   ./waf --run "scratch/Lab1_Cpp_Sweep --dStart=10.0 --dEnd=200.0 --dStep=10.0"
   ```

   * **Expected Output (example)**:

     ```
     Distance: 10 m, RxPackets: 10
     Distance: 20 m, RxPackets: 10
     Distance: 30 m, RxPackets: 10
     Distance: 40 m, RxPackets: 10
     Distance: 50 m, RxPackets:  8
     Distance: 60 m, RxPackets:  5
     Distance: 70 m, RxPackets:  2
     Distance: 80 m, RxPackets:  0
     --> No packets received at distance 80 m. Stopping sweep.
     ```
   * In this example, **the last nonzero distance** is 70 m, so *dᵢ* ≈ 70 m.

3. **Likely Issue (#7): “FlowMonitorHelper not found”**

   * **Cause:** Forgotten to `#include "ns3/flow-monitor-module.h"`.
   * **Solution:** Add at top:

     ```cpp
     #include "ns3/flow-monitor-module.h"
     ```

4. **Likely Issue (#8): No flows reported or 0 received even at small distances**

   * **Cause:** Data mode might be too high (6 Mbps) and at 30 m, still below sensitivity; or propagation not configured.
   * **Solution:**

     * Lower data mode to `OfdmRate1Mbps`, rebuild, rerun.
     * Check `CalcRxPower` manually (see Part 2.1) to solve if your link is viable.

5. **Record *dᵢ***

   * When your sweep prints `RxPackets == 0` at distance D, record *dᵢ* as the previous distance (D – dStep).
   * If the first distance tested (e.g., 10 m) shows 0, reduce `dStart` (e.g., test from 5 m onward).

---

### **3.2. Task: Python – Sweep to Find *dᵢ***

> **Goal:** Perform the same distance sweep using Python.

1. **Copy & Edit `Lab1_Py_TwoRay.py` → `Lab1_Py_Sweep.py`**

   ```bash
   cp ~/ns-3.40/scratch/Lab1_Py_TwoRay.py ~/ns-3.40/scratch/Lab1_Py_Sweep.py
   ```

2. **Edit** (`Lab1_Py_Sweep.py`) to loop distances:

   ```python
   #!/usr/bin/env python3
   import ns.core
   import ns.network
   import ns.internet
   import ns.wifi
   import ns.mobility
   import ns.applications
   import ns.propagation
   import ns.flow_monitor

   def run_one(distance):
       # (Reuse code from Lab1_Py_TwoRay, but remove NetAnim & set durations)
       wifiNodes = ns.network.NodeContainer()
       wifiNodes.Create (2)

       # Two-Ray Ground
       wifiChannel = ns.wifi.YansWifiChannelHelper.Default ()
       wifiChannel.SetPropagationLoss ("ns3::TwoRayGroundPropagationLossModel",
                                      "SystemLoss", ns.core.DoubleValue (1.0),
                                      "HeightAboveZ", ns.core.DoubleValue (1.5))
       wifiChannel.SetPropagationDelayModel ("ns3::ConstantSpeedPropagationDelayModel")
       wifiPhy = ns.wifi.YansWifiPhyHelper.Default ()
       wifiPhy.SetChannel (wifiChannel.Create ())
       wifiPhy.Set ("TxPowerStart", ns.core.DoubleValue (16.0))
       wifiPhy.Set ("TxPowerEnd",   ns.core.DoubleValue (16.0))
       wifiPhy.Set ("RxSensitivity", ns.core.DoubleValue (-80.0))

       wifiHelper = ns.wifi.WifiHelper()
       wifiHelper.SetStandard (ns.wifi.WIFI_PHY_STANDARD_80211a)
       wifiHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                           "DataMode",   ns.core.StringValue("OfdmRate1Mbps"),
                                           "ControlMode",ns.core.StringValue("OfdmRate1Mbps"))

       wifiMac = ns.wifi.WifiMacHelper()
       wifiMac.SetType ("ns3::AdhocWifiMac")
       wifiDevices = wifiHelper.Install (wifiPhy, wifiMac, wifiNodes)

       # Mobility
       posAlloc = ns.mobility.ListPositionAllocator()
       posAlloc.Add (ns.core.Vector (0.0, 0.0, 0.0))
       posAlloc.Add (ns.core.Vector (distance, 0.0, 0.0))
       mobility = ns.mobility.MobilityHelper()
       mobility.SetPositionAllocator (posAlloc)
       mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel")
       mobility.Install (wifiNodes)

       # Internet & IP
       internet = ns.internet.InternetStackHelper()
       internet.Install (wifiNodes)
       address = ns.internet.Ipv4AddressHelper()
       address.SetBase (ns.network.Ipv4Address("10.1.1.0"), ns.network.Ipv4Mask("255.255.255.0"))
       wifiInterfaces = address.Assign (wifiDevices)

       # UDP Echo
       echoServer = ns.applications.UdpEchoServerHelper (9)
       serverApps = echoServer.Install (wifiNodes.Get(1))
       serverApps.Start (ns.core.Seconds (1.0))
       serverApps.Stop (ns.core.Seconds (5.0))

       echoClient = ns.applications.UdpEchoClientHelper (wifiInterfaces.GetAddress(1), 9)
       echoClient.SetAttribute ("MaxPackets", ns.core.UintegerValue (10))
       echoClient.SetAttribute ("Interval",   ns.core.TimeValue (ns.core.Seconds (1.0)))
       echoClient.SetAttribute ("PacketSize", ns.core.UintegerValue (1000))
       clientApps = echoClient.Install (wifiNodes.Get(0))
       clientApps.Start (ns.core.Seconds (2.0))
       clientApps.Stop  (ns.core.Seconds (5.0))

       # FlowMonitor to count rxPackets
       flowmonHelper = ns.flow_monitor.FlowMonitorHelper()
       flowmon = flowmonHelper.InstallAll ()

       ns.core.Simulator.Stop (ns.core.Seconds (5.0))
       ns.core.Simulator.Run ()

       flowmon.CheckForLostPackets ()
       classifier = flowmonHelper.GetClassifier ()
       stats = flowmon.GetFlowStats ()

       rxPackets = 0
       for flowId, flowStats in stats.items ():
           fiveTuple = classifier.FindFlow (flowId)
           if (str(fiveTuple.sourceAddress) == str(wifiInterfaces.GetAddress(0)) and
               str(fiveTuple.destinationAddress) == str(wifiInterfaces.GetAddress(1))):
               rxPackets = flowStats.rxPackets

       ns.core.Simulator.Destroy ()
       return rxPackets

   def main():
       dStart = 10.0
       dEnd   = 200.0
       dStep  = 10.0

       # Sweep
       distance = dStart
       while distance <= dEnd:
           rx = run_one (distance)
           print (f"Distance: {distance} m, RxPackets: {rx}")
           if rx == 0:
               print (f"--> No packets at {distance} m; stopping.")
               break
           distance += dStep

   if __name__ == "__main__":
       ns.core.CommandLine().Parse ()  # no args here
       main ()
   ```

3. **Run**

   ```bash
   chmod +x ~/ns-3.40/scratch/Lab1_Py_Sweep.py
   export PYTHONPATH=~/ns-3.40/build/bindings:$PYTHONPATH
   ./waf --pyrun scratch/Lab1_Py_Sweep.py
   ```

   * **Expected**: Similar output to C++ sweep, giving *dᵢ*.

4. **Likely Issue (#9): “module ‘ns.flow\_monitor’ has no attribute ‘FlowMonitorHelper’”**

   * **Cause:** Wrong module import or older binding.
   * **Solution:**

     * Ensure you have `import ns.flow_monitor` at top.
     * In Python REPL:

       ```python
       >>> import ns.flow_monitor
       >>> dir(ns.flow_monitor)
       ```
     * If `FlowMonitorHelper` is absent, rebuild Python bindings with FlowMonitor enabled (`./waf configure --enable-python-bindings`).

---

### **3.3. Task: Compute Throughput vs. Distance for Two-Ray (C++)**

> **Goal:** For each distance in **D = { *dᵢ*, 7*dᵢ*/8, … , *dᵢ*/8 }**, measure the throughput (bits/s) at the application layer. Plot “throughput vs. distance.”

1. **Determine *dᵢ* from Part 3.1.**

   * Suppose from the sweep you found *dᵢ* = 70 m.

2. **Create a list of distances**

   * Distances:

     ```
     D = { 70.0, 70.0*7/8=61.25, 52.5, 43.75, 35.0, 26.25, 17.5, 8.75 }
     ```
   * Round as desired (e.g., 70, 61, 53, 44, 35, 26, 18, 9 m).

3. **Modify `Lab1_Cpp_Sweep.cc` → `Lab1_Cpp_TwoRay_Results.cc`**

   * Hardcode *dᵢ* and loop these 8 distances, measuring **throughput** (bits/sec).
   * Replace the flow monitor counting of `rxPackets` with calculation of **bytesReceived** and **(bytes × 8) / (time intervals)**. Example:

     ```cpp
     // After apps stop, extract flow stats again:
     double rxBytes = 0.0;
     for (auto const &kv : stats)
       {
         Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (kv.first);
         if (t.sourceAddress == wifiInterfaces.GetAddress (0) &&
             t.destinationAddress == wifiInterfaces.GetAddress (1))
           {
             rxBytes = kv.second.rxBytes; // total bytes received
           }
       }
     double throughput = (rxBytes * 8.0) / (4.0); // run time from 1.0 s to 5.0 s = 4 s
     std::cout << "Dist: " << distance 
               << " m, Throughput: " << throughput << " bps\n";
     ```
   * Full loop (pseudocode):

     ```cpp
     std::vector<double> distances = {70, 61.25, 52.5, 43.75, 35, 26.25, 17.5, 8.75};
     for (double distance : distances)
       {
         // (setup nodes, channel, apps as before, using Two-Ray)
         // Run 5 s sim; collect rxBytes
         // Compute throughput
         std::cout << distance << " " << throughput << "\n";
         Simulator::Destroy ();
       }
     ```

4. **Build & Run**

   ```bash
   ./waf --run scratch/Lab1_Cpp_TwoRay_Results
   ```

   * **Expected Output** (example):

     ```
     70 m: 5000000 bps
     61.25 m: 5500000 bps
     52.5 m: 6000000 bps
     43.75 m: 6500000 bps
     35 m: 7000000 bps
     26.25 m: 7500000 bps
     17.5 m: 8000000 bps
     8.75 m: 8500000 bps
     ```

     (Values will differ; these numbers illustrate how throughput rises as distance decreases.)

5. **Likely Issue (#10): “rxBytes is always 0 (throughput = 0)”**

   * **Cause:**

     * Simulation time too short, or start/stop times wrong.
     * FlowMonitor installed after apps start.
   * **Solution:**

     * Ensure you installed FlowMonitor **before** starting apps.
     * Confirm echoServer started at 1.0 s and echoClient at 2.0 s; you calculate throughput over a 4 s window (from 1.0 s to 5.0 s).
     * Alternatively, use the echoClient’s logs to verify any packet was sent.

---

### **3.4. Task: Throughput vs. Distance (Python)**

> **Goal:** Replicate the throughput sweep for the same list of 8 distances using Python.

1. **Copy & Edit `Lab1_Py_Sweep.py` → `Lab1_Py_TwoRay_Results.py`**

   ```bash
   cp ~/ns-3.40/scratch/Lab1_Py_Sweep.py ~/ns-3.40/scratch/Lab1_Py_TwoRay_Results.py
   ```

2. **Edit** (`Lab1_Py_TwoRay_Results.py`):

   * Replace the distance loop with:

     ```python
     distances = [70.0, 61.25, 52.5, 43.75, 35.0, 26.25, 17.5, 8.75]
     for d in distances:
         # (Reuse run_one, but modify to return rxBytes instead of rxPackets)
         rxBytes = run_one (d)  # alter run_one() to return rxBytes
         throughput = (rxBytes * 8.0) / 4.0  # bytes→bits, over 4 s
         print (f"Distance: {d} m, Throughput: {throughput} bps")
     ```
   * In `run_one()`, change:

     ```python
     rxBytes = flowStats.rxBytes
     return rxBytes
     ```

     instead of returning `rxPackets`.

3. **Run**

   ```bash
   chmod +x ~/ns-3.40/scratch/Lab1_Py_TwoRay_Results.py
   export PYTHONPATH=~/ns-3.40/build/bindings:$PYTHONPATH
   ./waf --pyrun scratch/Lab1_Py_TwoRay_Results.py
   ```

   * **Expected Output**: Throughput at each of the 8 distances.

4. **Likely Issue (#11): “run\_one() mismatch= rxPackets vs rxBytes”**

   * **Cause:** You changed return value but forgot to update caller.
   * **Solution:**

     * Ensure `run_one(d)` returns `rxBytes`.
     * Update print statement accordingly.

---

## **Part 4: Swap to Cost231-Hata and Friis Models**

We will now repeat the same 8-distance throughput measurements using two other propagation models: **Cost231-Hata** and **Friis**.

### **4.1. Task: C++ – Cost231-Hata**

1. **Copy & Edit** `Lab1_Cpp_TwoRay_Results.cc` → `Lab1_Cpp_Cost231_Results.cc`

   ```bash
   cp ~/ns-3.40/scratch/Lab1_Cpp_TwoRay_Results.cc ~/ns-3.40/scratch/Lab1_Cpp_Cost231_Results.cc
   ```

2. **Edit** (`Lab1_Cpp_Cost231_Results.cc`):

   * Replace the channel‐setup block:

     ```diff
     - Ptr<TwoRayGroundPropagationLossModel> lossModel = CreateObject<TwoRayGroundPropagationLossModel> ();
     - wifiChannel.SetPropagationLossObject (lossModel);
     - wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");

     + Ptr<Cost231PropagationLossModel> lossModel = CreateObject<Cost231PropagationLossModel> ();
     + // Set parameters if desired, e.g., freq 2400 MHz, city size = medium (defaults okay)
     + wifiChannel.SetPropagationLossObject (lossModel);
     + wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
     ```
   * All other logic (mobility, FlowMonitor, throughput calculation, loop over distances) remains identical.

3. **Build & Run**

   ```bash
   ./waf --run scratch/Lab1_Cpp_Cost231_Results
   ```

   * **Expected Output:** Throughput at each distance, probably *dᵢ* ∼ 50 m (Cost231 predicts more loss than Two-Ray).

4. **Likely Issue (#12): “Cost231PropagationLossModel not found”**

   * **Cause:** Missing include:

     ```cpp
     #include "ns3/cost231-propagation-loss-model.h"
     ```
   * **Solution:**

     * Add that include at the top (or rely on `#include "ns3/propagation-module.h"` which should cover it).

5. **Record the 8 throughput values** for Cost231 into a file `Cost231_Results.txt` or print them to stdout.

---

### **4.2. Task: C++ – Friis**

1. **Copy & Edit** `Lab1_Cpp_Cost231_Results.cc` → `Lab1_Cpp_Friis_Results.cc`

   ```bash
   cp ~/ns-3.40/scratch/Lab1_Cpp_Cost231_Results.cc ~/ns-3.40/scratch/Lab1_Cpp_Friis_Results.cc
   ```

2. **Edit** (`Lab1_Cpp_Friis_Results.cc`):

   * Replace the channel‐setup block with:

     ```diff
     - Ptr<Cost231PropagationLossModel> lossModel = CreateObject<Cost231PropagationLossModel> ();
     - wifiChannel.SetPropagationLossObject (lossModel);

     + Ptr<FriisPropagationLossModel> lossModel = CreateObject<FriisPropagationLossModel> ();
     + // Optionally set frequency (Hz) and gains:
     + lossModel->SetAttribute ("Frequency", DoubleValue (5e9));  // 5 GHz
     + lossModel->SetAttribute ("SystemLoss", DoubleValue (1.0)); // no system loss
     + wifiChannel.SetPropagationLossObject (lossModel);

     // (Delay model stays the same.)
     ```
   * Everything else is unchanged.

3. **Build & Run**

   ```bash
   ./waf --run scratch/Lab1_Cpp_Friis_Results
   ```

   * **Expected Output:** Throughput vs. distance, perhaps *dᵢ* ∼ 90 m (Friis is optimistic).

4. **Likely Issue (#13): “SetAttribute(‘Frequency’) requires DoubleValue in correct units”**

   * **Cause:** You passed “5e9” incorrectly (string vs. `DoubleValue`).
   * **Solution:**

     * Use:

       ```cpp
       lossModel->SetAttribute ("Frequency", DoubleValue (5e9)); // 5×10⁹ Hz
       lossModel->SetAttribute ("SystemLoss", DoubleValue (1.0));
       ```

---

### **4.3. Task: Python – Cost231 & Friis**

> **Goal:** Repeat the same 8-distance throughput measurement for Python.

1. **Copy & Edit** `Lab1_Py_TwoRay_Results.py` → `Lab1_Py_Cost231_Results.py`

   ```bash
   cp ~/ns-3.40/scratch/Lab1_Py_TwoRay_Results.py ~/ns-3.40/scratch/Lab1_Py_Cost231_Results.py
   ```

2. **Edit** (`Lab1_Py_Cost231_Results.py`):

   * Replace the Two-Ray channel creation inside `run_one()` with:

     ```python
     wifiChannel = ns.wifi.YansWifiChannelHelper.Default()
     wifiChannel.SetPropagationLoss ("ns3::Cost231PropagationLossModel",
                                    "Frequency", ns.core.DoubleValue (2.4e9),  # 2.4 GHz
                                    "CitySize",  ns.core.StringValue ("medium"))
     wifiChannel.SetPropagationDelayModel ("ns3::ConstantSpeedPropagationDelayModel")
     wifiPhy = ns.wifi.YansWifiPhyHelper.Default()
     wifiPhy.SetChannel (wifiChannel.Create ())
     ```
   * Leave the rest (run\_one returns rxBytes) unchanged.

3. **Run**

   ```bash
   chmod +x ~/ns-3.40/scratch/Lab1_Py_Cost231_Results.py
   export PYTHONPATH=~/ns-3.40/build/bindings:$PYTHONPATH
   ./waf --pyrun scratch/Lab1_Py_Cost231_Results.py
   ```

   * **Expected Output:** Throughput vs. distance for Cost231.

4. **Likely Issue (#14): “CitySize parameter not recognized”**

   * **Cause:** Some bindings expect uppercase/lowercase mismatch or different attribute names.
   * **Solution:**

     * Check Doxygen for Python binding: the attribute might be `"citySize"` or `"UrbanAreaType"`.
     * Example:

       ```python
       wifiChannel.SetPropagationLoss ("ns3::Cost231PropagationLossModel",
                                      "Frequency",     ns.core.DoubleValue (2.4e9),
                                      "OpenAreaType",  ns.core.StringValue ("suburban"))
       ```
     * Always verify binding names via `dir(ns.propagation.Cost231PropagationLossModel)` in Python REPL.

5. **Repeat for Friis** (`Lab1_Py_Friis_Results.py`):

   * Replace channel block with:

     ```python
     wifiChannel = ns.wifi.YansWifiChannelHelper.Default()
     wifiChannel.SetPropagationLoss ("ns3::FriisPropagationLossModel",
                                    "Frequency",   ns.core.DoubleValue (5e9),
                                    "SystemLoss",  ns.core.DoubleValue (1.0),
                                    "ReferenceLoss", ns.core.DoubleValue (1.0))
     wifiChannel.SetPropagationDelayModel ("ns3::ConstantSpeedPropagationDelayModel")
     wifiPhy = ns.wifi.YansWifiPhyHelper.Default()
     wifiPhy.SetChannel (wifiChannel.Create ())
     ```
   * **Run** similarly.

---

## **Part 5: Plotting & Comparing the Three Models**

> **Goal:** Combine the throughput‐vs‐distance data from Three Models and plot them on one chart for comparison.

1. **Collect Results**

   * From **C++** runs, save outputs into three files:

     ```
     TwoRay.txt
     Cost231.txt
     Friis.txt
     ```

     Each file has lines like:

     ```
     70  5000000
     61  5500000
     …
     ```
   * Or similarly from **Python** runs.

2. **Quick Plot with Python (using Matplotlib)**

   * Create a script `~/ns-3.40/scratch/Plot_Lab1.py`:

     ```python
     import matplotlib.pyplot as plt

     # Example data (replace with actual)
     dist = [70, 61.25, 52.5, 43.75, 35, 26.25, 17.5, 8.75]
     two_ray   = [5e6, 5.5e6, 6e6, 6.5e6, 7e6, 7.5e6, 8e6, 8.5e6]
     cost231   = [4e6, 4.4e6, 4.8e6, 5.2e6, 5.6e6, 6.0e6, 6.4e6, 6.8e6]
     friis     = [6e6, 6.5e6, 7e6, 7.5e6, 8e6, 8.5e6, 9e6, 9.5e6]

     plt.figure()
     plt.plot (dist, two_ray,   marker='o', label='Two-Ray Ground')
     plt.plot (dist, cost231,   marker='s', label='Cost231-Hata')
     plt.plot (dist, friis,     marker='^', label='Friis')
     plt.xlabel ('Distance (m)')
     plt.ylabel ('Throughput (bps)')
     plt.title  ('Throughput vs. Distance (Lab 1)')
     plt.legend()
     plt.grid (True)
     plt.savefig ('Lab1_Throughput_Comparison.png', dpi=300)
     plt.show ()
     ```
   * **Run**:

     ```bash
     cd ~/ns-3.40
     python3 scratch/Plot_Lab1.py
     ```
   * **Expected:** A PNG chart showing three curves; Two-Ray usually sits between Friis (highest) and Cost231 (lowest).

3. **Likely Issue (#15): “ModuleNotFoundError: No module named matplotlib”**

   * **Solution:**

     ```bash
     pip3 install matplotlib
     ```

---

## **Part 6: (Optional) Real-World Measurement & Friis Comparison**

> **Goal:** Perform a quick “real” path‐loss measurement in a hallway and compare to Friis model.

1. **Set Up a Simple Ad-hoc WiFi Network**

   * Laptop A (Tx) and Laptop B (Rx).
   * Create an ad-hoc network on both machines:

     * On Ubuntu:

       ```bash
       nmcli dev wifi connect '' ifname wlan0 mode adhoc ssid LAB1 freq 2412
       ```
     * Or use `iwconfig` if older tools.
   * Configure static IPs:

     ```bash
     sudo ifconfig wlan0 10.1.2.1 netmask 255.255.255.0 up   # Laptop A
     sudo ifconfig wlan0 10.1.2.2 netmask 255.255.255.0 up   # Laptop B
     ```

2. **Measure RSSI at Various Distances**

   * On Laptop B, install `iw` and/or `WiFi Information View (Windows)`:

     ```bash
     sudo apt-get install iw
     ```
   * At each meter mark (e.g. 1 m, 2 m, …), run:

     ```bash
     sudo iw wlan0 scan | grep 'signal' | head -n 1
     ```

     * This prints something like `signal: -40.00 dBm`.
   * Alternatively, continuously `ping` A from B:

     ```bash
     ping 10.1.2.1 -c 10
     ```

     while capturing with Wireshark to get RSSI per packet (in Radiotap header).

3. **Compute Path Loss (dB)**

   * If Tx power is known (e.g., **15 dBm**), then:

     ```
     path_loss (dB) = TxPower (dBm) – RSSI (dBm)
     ```
   * Example: If RSSI = –40 dBm, then path\_loss = 15 – (–40) = 55 dB.

4. **Friis Path Loss Calculation**

   * Friis formula (in free space):

     ```
     PL(dB) = 20 log10( (4πd f) / c ) + SystemLoss
     ```

     * f = 2.412 GHz (channel 1); c = 3×10⁸ m/s.
     * At each distance d (m), compute PL\_friis.
   * Quick Python snippet:

     ```python
     import numpy as np

     c = 3e8
     f = 2.412e9
     def friis_loss(d):
         return 20 * np.log10(4 * np.pi * d * f / c)

     distances = np.arange(1, 11, 1)  # 1 m to 10 m
     measured_rssi = [-40, -45, -50, -55, -60, -65, -70, -75, -80, -85]  # example
     tx_power = 15  # dBm

     measured_pl = [tx_power - r for r in measured_rssi]
     friis_pl = [friis_loss(d) for d in distances]

     import matplotlib.pyplot as plt
     plt.plot(distances, measured_pl, 'o-', label='Measured')
     plt.plot(distances, friis_pl,  's-', label='Friis')
     plt.xlabel ('Distance (m)')
     plt.ylabel ('Path Loss (dB)')
     plt.title  ('Measured vs. Friis Path Loss')
     plt.legend()
     plt.grid (True)
     plt.show ()
     ```

5. **Likely Issue (#16): “numpy not installed”**

   * **Solution:**

     ```bash
     pip3 install numpy
     ```

6. **Interpretation:**

   * Friis assumes **free‐space**; your “Measured” values may be higher due to reflections, walls, and antenna patterns.
   * In your **lab report**, include the chart and a short explanation of discrepancies (e.g., multipath, human blockage).

---

## **Part 7: Troubleshooting & Common Pitfalls**

1. **Incorrect Units for Frequency or Distance**

   * Always use **Hz** for frequency (e.g., 5 GHz = 5 × 10⁹ Hz).
   * If you pass `5e3` (5 kHz) to a Friis model, results are meaningless.
   * **Fix:** Double-check Doxygen attribute descriptions (see Friis reference).

2. **Forgetting to Set Data Mode / WiFi Manager**

   * If you mix rate adaptation with a static distance, reported throughput may vary unpredictably.
   * **Fix:** Use `ConstantRateWifiManager` to force a known data rate (6 Mbps or 1 Mbps) so you isolate propagation effects.

3. **FlowMonitor vs. Pcap for Throughput**

   * If you rely on pcap alone (packet captures), you may need to parse timestamps manually.
   * **FlowMonitor** gives you *rxBytes* directly.
   * **Fix:** Install FlowMonitor *before* starting applications. Include:

     ```cpp
     #include "ns3/flow-monitor-module.h"
     ```

4. **Python Binding Attribute Names Mismatch**

   * Sometimes the Python binding names differ slightly (camelCase vs. lowerCase).
   * **Fix:** In a Python REPL, do:

     ```python
     >>> import ns.propagation
     >>> dir(ns.propagation.TwoRayGroundPropagationLossModel)
     ```

     to discover correct attribute names.

5. **Simulation Timing Mistakes**

   * If echoClient `Start` time ≤ echoServer `Start` time, client’s packets are lost (server not ready).
   * **Fix:** Ensure server starts at 1.0 s, client at 2.0 s.

6. **NetAnim Not Showing Packets**

   * If nodes overlap (same x,y), packets appear as “blips”—hard to see.
   * **Fix:** Assign distinct positions via `anim.SetConstantPosition(...)`.
   * Also, ensure simulation time is long enough to complete traffic before XML is written.

7. **C++ Build Failures after Edit**

   * If you add `#include` incorrectly or forget a semicolon, the entire project may fail to build.
   * **Fix:** Isolate the error by building only the changed file:

     ```bash
     ./waf build -v
     ```

     and track down the missing header or symbol.

8. **Python “Segmentation Fault”**

   * Rare but can occur if mismatched Python versions or old bindings.
   * **Fix:**

     * Rebuild ns-3 with the same Python binary (e.g., if you use `python3.10`, ensure `./waf configure --with-python=/usr/bin/python3.10`).
     * Delete `build/` and rerun `./waf build`.

---