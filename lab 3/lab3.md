**Lab 3: Measuring Performance of an Ad Hoc Network**
*Choose Your Language: C++ and/or Python Binding*

---

## 1 Objectives

By the end of this lab you will be able to:

1. Set up an **ad-hoc WiFi chain** in ns-3, varying number of hops and packet sizes.
2. Measure **UDP throughput** end-to-end and understand how it degrades with hops.
3. Switch to **TCP traffic**, configure segment size, and compare TCP vs UDP performance.
4. Create and analyze a **hidden-terminal** scenario with **RTS/CTS** on vs off.
5. Use **ns-3 Python bindings** as an alternative to C++.
6. Visualize all topologies in **NetAnim**.
7. Troubleshoot common pitfalls (missing routing, wrong attributes, binding issues).
8. Navigate the ns-3 documentation for relevant classes and helpers.

Every section is broken into **small tasks → likely issue → guided solution**.

---

## 2 Prerequisites & Setup

* **ns-3.40** (or later) built with both C++ and Python bindings (see Lab 0).

* **Wireshark**, **NetAnim**, **Matplotlib** installed for pcap analysis, visualization, and plotting.

* **Documentation** base index:

  ```
  https://www.nsnam.org/docs/release/3.40/doxygen/build/html/index.html
  ```

  From there, navigate **Modules → wifi**, **flow-monitor**, **internet**, **mobility**, etc.

* **Key Doxygen links** (replace `index.html` with these):

  * Ad-hoc MAC: …/classns3\_1\_1AdhocWifiMac.html
  * ConstantRateWifiManager: …/classns3\_1\_1ConstantRateWifiManager.html
  * RngSeedManager: …/classns3\_1\_1RngSeedManager.html
  * OnOffHelper: …/classns3\_1\_1OnOffHelper.html
  * FlowMonitorHelper: …/classns3\_1\_1FlowMonitorHelper.html
  * AnimationInterface: …/classns3\_1\_1AnimationInterface.html

---

## 3 Part 1: Ad-Hoc Multi-Hop UDP Scenario

### 3.1 Task (C++): Single-Flow Chain

**Goal:** Create a chain of $N$ nodes (adjacent nodes within WiFi range only), send UDP from node 0 to node $N\!-\!1$, measure throughput.

#### Step 3.1.1: Skeleton C++ Code

```cpp
// File: scratch/Lab3_Cpp_Adhoc.cc
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

int main(int argc,char*argv[]){
  uint32_t numNodes=3,    // default 3 nodes = 2 hops
           pktSize=500;   // default UDP payload
  uint32_t tcpSegmentSize=1000;
  double simTime=10.0, distance=200.0;

  CommandLine cmd;
  cmd.AddValue("numNodes","Number of ad-hoc nodes",numNodes);
  cmd.AddValue("pktSize","UDP payload size (B)",pktSize);
  cmd.AddValue("distance","Distance between adjacent nodes (m)",distance);
  cmd.Parse(argc,argv);

  // 1. Create N nodes
  NodeContainer nodes; nodes.Create(numNodes);

  // 2. WiFi PHY & Channel (ad-hoc)
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
  YansWifiPhyHelper    phy     = YansWifiPhyHelper::Default();
  phy.SetChannel(channel.Create());
  phy.Set("TxPowerStart",DoubleValue(16.0))
     .Set("TxPowerEnd",  DoubleValue(16.0))
     .Set("RxSensitivity",DoubleValue(-80.0));

  WifiHelper wifi;  
  wifi.SetStandard(WIFI_PHY_STANDARD_80211b)
      .SetRemoteStationManager(
        "ns3::ConstantRateWifiManager",
        "DataMode",StringValue("DsssRate1Mbps"),
        "ControlMode",StringValue("DsssRate1Mbps")
      );
  WifiMacHelper mac; mac.SetType("ns3::AdhocWifiMac");
  NetDeviceContainer devices = wifi.Install(phy,mac,nodes);

  // 3. Mobility: linear chain
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> pa = CreateObject<ListPositionAllocator>();
  for(uint32_t i=0;i<numNodes;i++)
    pa->Add(Vector(distance*i,0,0));
  mobility.SetPositionAllocator(pa);
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(nodes);

  // 4. Internet & IP
  InternetStackHelper internet; internet.Install(nodes);
  Ipv4AddressHelper address;
  address.SetBase("10.1.1.0","255.255.255.0");
  Ipv4InterfaceContainer ips = address.Assign(devices);

  // 5. UDP OnOff from node0 → last node
  OnOffHelper onoff("ns3::UdpSocketFactory",
    InetSocketAddress(ips.GetAddress(numNodes-1),9));
  onoff.SetAttribute("PacketSize",UintegerValue(pktSize));
  onoff.SetAttribute("OnTime",   StringValue("ns3::ConstantRandomVariable[Constant=1]"));
  onoff.SetAttribute("OffTime",  StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  ApplicationContainer apps = onoff.Install(nodes.Get(0));
  apps.Start(Seconds(1.0)); apps.Stop(Seconds(simTime));

  PacketSinkHelper sink("ns3::UdpSocketFactory",
    InetSocketAddress(Ipv4Address::GetAny(),9));
  apps = sink.Install(nodes.Get(numNodes-1));
  apps.Start(Seconds(0.0)); apps.Stop(Seconds(simTime));

  // 6. FlowMonitor to capture rxBytes
  FlowMonitorHelper fmHelper;  
  Ptr<FlowMonitor> fm = fmHelper.InstallAll();

  // 7. NetAnim visualization
  AnimationInterface anim("lab3-adhoc-anim.xml");
  for(uint32_t i=0;i<numNodes;i++)
    anim.SetConstantPosition(nodes.Get(i),10+i*20,20);

  Simulator::Stop(Seconds(simTime));
  Simulator::Run();

  // 8. Compute throughput
  fm->CheckForLostPackets();
  Ptr<Ipv4FlowClassifier> classifier = 
    DynamicCast<Ipv4FlowClassifier>(fmHelper.GetClassifier());
  auto stats = fm->GetFlowStats();
  double rxBytes=0;
  for(auto& kv: stats){
    auto t=classifier->FindFlow(kv.first);
    if(t.sourceAddress==ips.GetAddress(0)
      && t.destinationAddress==ips.GetAddress(numNodes-1)){
      rxBytes=kv.second.rxBytes;
    }
  }
  double throughput = rxBytes*8.0/(simTime-1.0);
  std::cout<<"UDP Throughput: "<<throughput<<" bps\n";

  Simulator::Destroy();
  return 0;
}
```

#### **Step 3.1.2: Build & Run**

```bash
cd ~/ns-3.40
./waf configure --enable-examples --enable-tests
./waf build
./waf --run "scratch/Lab3_Cpp_Adhoc \
    --numNodes=4 --pktSize=700 --distance=200.0"
netanim lab3-adhoc-anim.xml
```

* **Expected:**

  * A chain of 4 nodes, UDP OnOff floods from node 0 to node 3.
  * NetAnim shows packet animation along the line.
  * Console prints “UDP Throughput: XXXX bps”.

#### **Likely Issues & Fixes**

| Symptom                                 | Cause                                                   | Fix                                                                                                                                                |
| :-------------------------------------- | :------------------------------------------------------ | :------------------------------------------------------------------------------------------------------------------------------------------------- |
| No routing between nodes ⇒ throughput=0 | Forgot routing protocol (ad‐hoc needs a routing helper) | Add e.g.:<br>`OlsrHelper olsr;`<br>`Ipv4StaticRoutingHelper r;`<br>`Ipv4ListRoutingHelper lr; lr.Add(olsr,1);`<br>`internet.SetRoutingHelper(lr);` |
| `OnOffHelper` not found                 | Missing `#include "ns3/applications-module.h"`          | Add that include                                                                                                                                   |
| FlowMonitorHelper undefined             | Forgot `#include "ns3/flow-monitor-module.h"`           | Include it                                                                                                                                         |
| XML not generated                       | Forgot `#include "ns3/netanim-module.h"`                | Include it                                                                                                                                         |

---

### 3.2 Task (Python): Ad-Hoc Chain

> **Goal:** Replicate 3.1 in Python.

```python
#!/usr/bin/env python3
import ns.core, ns.network, ns.wifi, ns.mobility
import ns.internet, ns.applications, ns.flow_monitor, ns.netanim

def main():
    # Parse args
    cmd = ns.core.CommandLine()
    cmd.AddValue("numNodes","Number of nodes",3)
    cmd.AddValue("pktSize","Packet size (B)",500)
    cmd.AddValue("distance","Inter-node distance (m)",200.0)
    cmd.Parse()
    numNodes = int(cmd.GetGlobalValue("numNodes"))
    pktSize  = int(cmd.GetGlobalValue("pktSize"))
    distance = float(cmd.GetGlobalValue("distance"))
    simTime  = 10.0

    # 1) Nodes
    nodes = ns.network.NodeContainer(); nodes.Create(numNodes)

    # 2) WiFi ad-hoc
    channel = ns.wifi.YansWifiChannelHelper.Default()
    phy     = ns.wifi.YansWifiPhyHelper.Default()
    phy.SetChannel(channel.Create())
    phy.Set("TxPowerStart", ns.core.DoubleValue(16))
    phy.Set("RxSensitivity",ns.core.DoubleValue(-80))

    wifi = ns.wifi.WifiHelper()
    wifi.SetStandard(ns.wifi.WIFI_PHY_STANDARD_80211b)
    wifi.SetRemoteStationManager(
      "ns3::ConstantRateWifiManager",
      "DataMode", ns.core.StringValue("DsssRate1Mbps"),
      "ControlMode",ns.core.StringValue("DsssRate1Mbps")
    )
    mac = ns.wifi.WifiMacHelper()
    mac.SetType ("ns3::AdhocWifiMac")
    devices = wifi.Install(phy,mac,nodes)

    # 3) Mobility
    pa = ns.mobility.ListPositionAllocator()
    for i in range(numNodes):
        pa.Add(ns.core.Vector(distance*i,0,0))
    mob = ns.mobility.MobilityHelper()
    mob.SetPositionAllocator(pa)
    mob.SetMobilityModel("ns3::ConstantPositionMobilityModel")
    mob.Install(nodes)

    # 4) Internet & IP
    inet = ns.internet.InternetStackHelper(); inet.Install(nodes)
    addr = ns.internet.Ipv4AddressHelper()
    addr.SetBase(ns.network.Ipv4Address("10.1.1.0"), 
                 ns.network.Ipv4Mask("255.255.255.0"))
    ips = addr.Assign(devices)

    # 5) Applications
    onoff = ns.applications.OnOffHelper(
      "ns3::UdpSocketFactory",
      ns.network.InetSocketAddress(ips.GetAddress(numNodes-1),9)
    )
    onoff.SetAttribute("PacketSize", ns.core.UintegerValue(pktSize))
    onoff.SetAttribute("OnTime", ns.core.StringValue("ns3::ConstantRandomVariable[Constant=1]"))
    onoff.SetAttribute("OffTime",ns.core.StringValue("ns3::ConstantRandomVariable[Constant=0]"))
    apps = onoff.Install(nodes.Get(0))
    apps.Start(ns.core.Seconds(1.0)); apps.Stop(ns.core.Seconds(simTime))

    sink = ns.applications.PacketSinkHelper(
      "ns3::UdpSocketFactory",
      ns.network.InetSocketAddress(ns.network.Ipv4Address.GetAny(),9)
    )
    apps = sink.Install(nodes.Get(numNodes-1))
    apps.Start(ns.core.Seconds(0.0)); apps.Stop(ns.core.Seconds(simTime))

    # 6) FlowMonitor
    fmHelper = ns.flow_monitor.FlowMonitorHelper()
    fm = fmHelper.InstallAll()

    # 7) NetAnim
    anim = ns.netanim.AnimationInterface("lab3-py-adhoc.xml")
    for i in range(numNodes):
        anim.SetConstantPosition(nodes.Get(i),10+i*20,20)

    # 8) Run
    ns.core.Simulator.Stop(ns.core.Seconds(simTime))
    ns.core.Simulator.Run()

    # 9) Throughput calc
    fm.CheckForLostPackets()
    classifier = fmHelper.GetClassifier()
    stats = fm.GetFlowStats()
    rxBytes = 0
    for flowId, flowStats in stats.items():
        t = classifier.FindFlow(flowId)
        if (str(t.sourceAddress)==str(ips.GetAddress(0))
            and str(t.destinationAddress)==str(ips.GetAddress(numNodes-1))):
            rxBytes = flowStats.rxBytes
    throughput = rxBytes*8.0/(simTime-1.0)
    print(f"Python UDP throughput = {throughput} bps")

    ns.core.Simulator.Destroy()

if __name__=="__main__":
    main()
```

**Build & Run:**

```bash
chmod +x scratch/Lab3_Py_Adhoc.py
export PYTHONPATH=~/ns-3.40/build/bindings:$PYTHONPATH
./waf --pyrun scratch/Lab3_Py_Adhoc.py --numNodes=5 --pktSize=700 --distance=100.0
netanim lab3-py-adhoc.xml
```

**Likely Issue (#1-Py):**
`ImportError: No module named ns.mobility`
→ **Fix:** ensure `--enable-python-bindings`, rebuild, and `export PYTHONPATH=…/build/bindings`.

---

## 4 Part 2: Varying Hops & Packet Sizes

### 4.1 Task: Throughput vs. Packet Size (for each hop count)

**Goal:** For each node-count $N∈\{3,4,5,6\}$ and each payload $P∈\{300,700,1200\}$, measure UDP throughput.

#### 4.1.1 Incremental Steps

1. **Loop over hop counts**

   ```cpp
   for (uint32_t N : {3u,4u,5u,6u}) { … }
   ```
2. **Inside**, loop over packet sizes:

   ```cpp
   for (uint32_t P : {300u,700u,1200u}) { … }
   ```
3. **Reuse** the chain setup from Part 1, replacing `pktSize` with `P` and `numNodes` with `N`.
4. **Collect** each throughput result into a 2D table: rows=N, columns=P.
5. **Print** in tabular form:

   ```
   N=3: P=300→X bps, 700→Y bps, 1200→Z bps
   …
   ```

#### 4.1.2 Likely Issues & Fixes

| Symptom                                                      | Cause                                     | Fix                                                                 |
| :----------------------------------------------------------- | :---------------------------------------- | :------------------------------------------------------------------ |
| Compile error: `cannot convert initializer list to uint32_t` | Braced-init lists unsupported             | Use an array:<br>`uint32_t Ns[] = {3,4,5,6};` then `for(auto N:Ns)` |
| Throughput same for all P                                    | Forgot to set `PacketSize` each iteration | Ensure `onoff.SetAttribute("PacketSize",…P)` inside the inner loop  |

### 4.2 Task (Python): Packet-Size Sweep

Replicate 4.1 in `Lab3_Py_PayloadSweep.py`, looping over `numNodes` and `pktSize` arrays. Return `rxBytes` from a helper, compute throughput, and store in a nested dictionary for plotting.

---

## 5 Part 3: TCP vs UDP in a 3-Node Chain

### 5.1 Task: Configure TCP

1. **In C++** (modify `Lab3_Cpp_Adhoc.cc`):

   * After creating nodes, insert:

     ```cpp
     // Set TCP segment size to match payload
     Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(pktSize));
     ```
   * Replace `OnOffHelper` with `BulkSendHelper("ns3::TcpSocketFactory", …)` and use `PacketSinkHelper("ns3::TcpSocketFactory",…)` at sink.
2. **Run** for $N=3$, $P∈\{300,1200\}$.
3. **Measure** throughput same way via FlowMonitor.

### 5.2 Likely Issues

| Symptom                    | Cause                               | Fix                                            |
| :------------------------- | :---------------------------------- | :--------------------------------------------- |
| `BulkSendHelper` not found | Include `ns3/applications-module.h` | Add that include                               |
| TCP throughput << UDP      | Default TCP MSS = 536 B             | You must set `TcpSocket::SegmentSize` as above |

---

## 6 Part 4: Hidden-Terminal Experiment

### 6.1 Task (C++): Infrastructure BSS with Hidden Terminals

**Goal:** Two stations cannot hear each other but both reach the AP. Compare **RTS/CTS off** vs **on**.

```cpp
// scratch/Lab3_Cpp_Hidden.cc
// (Use same includes as 3.1)

int main(...){
  // 1. Create 3 nodes: STA0, AP, STA1
  NodeContainer wifiStaNodes; wifiStaNodes.Create(2);
  NodeContainer wifiApNode;   wifiApNode.Create(1);

  // 2. Setup WiFi helpers, phy, channel
  //   as in 3.1, but after phy:
  // Place STA0 at (0,0), AP at (d,0), STA1 at (2d,0)
  double d=200.0;
  // mobility setup accordingly…

  // 3. Install stack & IP on all nodes

  // 4. Install UDP OnOff on STA0→AP and STA1→AP
  OnOffHelper onoff0("ns3::UdpSocketFactory",
    InetSocketAddress(ipsAp,9));
  onoff0.SetAttribute(...); // pktSize=1000, rate=1Mbps

  OnOffHelper onoff1("ns3::UdpSocketFactory",
    InetSocketAddress(ipsAp,10)); // different port
  // …

  // 5. RTS/CTS toggle
  // By default OFF (use high threshold)
  Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold",
                     StringValue("2200")); // no RTS

  // Run once, gather throughput & PDR

  // 6. Enable RTS/CTS
  Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold",
                     StringValue("0"));    // always RTS

  // Re-run sim, measure again

  // 7. FlowMonitor to get rxPackets for each flow
}
```

### 6.2 Likely Issues

| Symptom                        | Cause                                                     | Fix                                                                                      |
| :----------------------------- | :-------------------------------------------------------- | :--------------------------------------------------------------------------------------- |
| Stations still hear each other | d too small or TxPower too high                           | Increase d or lower TxPower via `phy.Set("TxPowerStart", ...)`                           |
| RTS/CTS no effect              | Forgot to reconfigure `RtsCtsThreshold` before second run | Place `Config::SetDefault` **before** `waf run` invocation or call `Config::UpdateAll()` |

Replicate in Python in `Lab3_Py_Hidden.py` using `Config.SetGlobal()` equivalents.

---

## 7 Visualization

For each scenario—chain UDP, payload sweep, hidden terminal—generate a **NetAnim XML** and take at least one screenshot showing:

* Node positions,
* Packet or OnOff flows,
* RTS/CTS handshake (you can color control vs data frames if desired via `anim.UpdateNodeSize` or trace filters).

Launch via:

```bash
netanim lab3-adhoc-anim.xml
netanim lab3-hidden-anim.xml
```

---

## 8 Troubleshooting Quick Reference

1. **No routing in ad-hoc** → Install a routing helper (e.g. OLSR).
2. **Missing `#include`** → Check Doxygen for the correct module header.
3. **Python import errors** → Verify `PYTHONPATH=…/build/bindings`.
4. **Simulation ends immediately** → Ensure you call `Simulator::Stop`.
5. **RTS/CTS not taking effect** → Confirm `RtsCtsThreshold` is set **before** `WifiHelper.Install`.
6. **FlowMonitor sees zero bytes** → Install FlowMonitor **before** apps start, and ensure IP 5-tuple matches the flow.

---

## 9 Deliverables

* A **single PDF** `Lab3_Report.pdf` including:

  1. **Documentation Links** you used (Doxygen pages).
  2. **Tables & Graphs**:

     * UDP throughput vs packet size for each hop count.
     * UDP throughput vs hop count for 1200 B.
     * TCP vs UDP for 2 payloads.
     * Hidden-terminal throughput & PDR (RTS off/on).
  3. **NetAnim Screenshots** (chain & hidden).
  4. **Code Listings** (C++ & Python versions).
  5. **Troubleshooting Log** summarizing any errors you encountered and their fixes.

* **Scripts** in your ZIP:

  * `Lab3_Cpp_Adhoc.cc`, `Lab3_Cpp_PayloadSweep.cc`, `Lab3_Cpp_TCP.cc`, `Lab3_Cpp_Hidden.cc`
  * `Lab3_Py_Adhoc.py`, `Lab3_Py_PayloadSweep.py`, `Lab3_Py_TCP.py`, `Lab3_Py_Hidden.py`

Compress into `Lab3_Submission.zip` and submit.

---

**Congratulations!**  You’ve completed Lab 3, gaining hands-on experience with multi-hop ad-hoc networks, TCP vs UDP, and the hidden-terminal problem in ns-3—using both C++ and Python.
