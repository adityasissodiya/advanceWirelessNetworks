// Lab 2: Infrastructure WiFi — Scenario 2 (Single flow via an AP)
// ------------------------------------------------------------------------------------
// GOAL (spec, Fig. 1):
//   • Three nodes form an EQUILATERAL TRIANGLE with side length = 10 m.
//     - One Access Point (AP) — infrastructure Wi‑Fi (IEEE 802.11b)
//     - One STA sender  ---->  one STA receiver (UDP traffic)
//   • Vary PHY data rate among {1, 5.5, 11} Mb/s (we pass this in as --rate)
//   • Offered load is intentionally MUCH HIGHER than the PHY (saturation), per spec note.
//   • Measure application-layer GOODPUT (bits received at the sink over active time).
//   • Repeat runs with different seeds (we pass this in as --seed).
//   • Produce a NetAnim XML called `scenario1_anim.xml`.
//
// WHAT THIS FILE FIXES vs the starter:
//   1) Correct geometry: equilateral triangle of side 10 m (NOT a line).
//   2) Saturating offered load: OnOff DataRate set to a large value (e.g., 100 Mbps),
//      independent of the PHY rate.
//   3) Deliverable‑compliant NetAnim filename: scenario1_anim.xml
//
// HOW TO RUN (from ns-3 root):
//   ./ns3 run "scratch/Lab2_Cpp_Scenario1 --rate=11 --seed=2"
// ------------------------------------------------------------------------------------

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

// --------- Utility: map numeric Mbps to a legal 802.11b WifiMode string ---------
// IEEE 802.11b supports DSSS/CCK modes at 1, 2, 5.5, and 11 Mbps.
static std::string
ModeForB (double rateMbps)
{
  if (rateMbps <= 1.0)   return "DsssRate1Mbps";
  if (rateMbps <= 2.0)   return "DsssRate2Mbps";
  if (rateMbps <= 5.5)   return "DsssRate5_5Mbps";
  /* else */             return "DsssRate11Mbps";
}

int
main (int argc, char *argv[])
{
  // ------------------------- Simulation parameters (CLI) -------------------------
  // rate:   the *PHY* data rate we lock the Wi‑Fi manager to (ConstantRateWifiManager)
  // seed:   the RngRun so we can repeat with different contention/backoff timings
  double   distance = 150.0;    // Distance between Nodes
  double   antHeight = 2;   // Antenna height
  double   rate = 1.0;        // Mbps (valid: 1, 2, 5.5, 11)
  uint32_t payload = 1000;    // Paylod in Bytes
  uint32_t seed = 1;          // RngRun index (use 1 and 2 per spec)
  bool useRtsCts = false;      // Enable/disable RTS/CTS
  CommandLine cmd;
  cmd.AddValue("distance", "Distance between Nodes", distance);
  cmd.AddValue("antHeight", "Antenna height", antHeight);
  cmd.AddValue("rate", "802.11b PHY data rate in Mbps (1, 2, 5.5, 11 -> rounded up)", rate);
  cmd.AddValue("payload", "Payload in Bytes (use 1 and 2 for the lab)", payload);
  cmd.AddValue("seed", "RngRun value for repeatability (use 1 and 2 for the lab)", seed);
  cmd.AddValue("useRtsCts", "Enable/disable RTS/CTS", useRtsCts);
  cmd.Parse(argc, argv);

  // ----------------------------- Randomization setup -----------------------------
  // Keep the Seed constant across all runs; vary only the Run to randomize per‑trial.
  RngSeedManager::SetSeed(1);
  RngSeedManager::SetRun(seed);

  // Use nanosecond resolution for events (safe default for Wi‑Fi experiments).
  Time::SetResolution(Time::NS);

  // Use RtsCts, must be > than payload to ignore
  if (useRtsCts) {
    Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue("0"));
  } else {
    Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue(std::to_string(2*payload + 1)));
  }

  // ---------------------------- Topology: nodes & roles ---------------------------
  // We create three nodes:
  //   • 1 AP node (infrastructure BSS)
  //   • 2 STA nodes: both will be the *sender*
  NodeContainer staNodes;  staNodes.Create(2);
  NodeContainer apNode;    apNode.Create(1);

  // --------------------------- Channel / PHY configuration ------------------------
  // YansWifiChannelHelper::Default() sets Friis + LogDistance + RandomLoss by default.
  // That's why we use the TwoRay Config from Lab1
  // We keep defaults; the geometry (10 m sides) gives comparable path loss on all links.
  
  // FRIIS model
  // YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
  
  // TwoRay model
  YansWifiChannelHelper channel;
  channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  channel.AddPropagationLoss("ns3::TwoRayGroundPropagationLossModel");

  // ns-3.40: create a YansWifiPhyHelper and explicitly bind it to the channel.
  YansWifiPhyHelper phy;
  phy.SetChannel(channel.Create());
  phy.Set("TxPowerStart", DoubleValue(23.0));
  phy.Set("TxPowerEnd",   DoubleValue(23.0));

  // ------------------------------- Wi‑Fi MAC & rate -------------------------------
  // Lock the standard to 802.11b and force both Data/Control to the requested mode,
  // so the PHY rate is *constant* during the experiment.
  WifiHelper wifi;
  wifi.SetStandard(WIFI_STANDARD_80211b);
  const std::string mode = ModeForB(rate);
  wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                               "DataMode",    StringValue(mode),
                               "ControlMode", StringValue(mode));

  WifiMacHelper mac;
  Ssid ssid = Ssid("lab2-ssid");

  // STA devices (non-AP)
  mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false));
  NetDeviceContainer staDevs = wifi.Install(phy, mac, staNodes);

  // AP device
  mac.SetType("ns3::ApWifiMac",  "Ssid", SsidValue(ssid));
  NetDeviceContainer apDev  = wifi.Install(phy, mac, apNode);

  // --------------------------------- Mobility model --------------------------------
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> pos = CreateObject<ListPositionAllocator>();
  pos->Add(Vector( 0.0, 0.0, antHeight));       // AP
  pos->Add(Vector(-distance, 0.0, antHeight));  // STA sender 1
  pos->Add(Vector(distance, 0.0, antHeight));   // STA sender 2
  mobility.SetPositionAllocator(pos);
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(apNode);
  mobility.Install(staNodes);

  // ------------------------------- Internet + IP stack -----------------------------
  // Install TCP/IP on all nodes so UDP sockets work end-to-end.
  InternetStackHelper stack;
  stack.Install(apNode);
  stack.Install(staNodes);

  // Single IPv4 subnet for simplicity.
  Ipv4AddressHelper ipv4;
  ipv4.SetBase("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer staIfaces = ipv4.Assign(staDevs);
  Ipv4InterfaceContainer apIface   = ipv4.Assign(apDev);

  // ------------------------------ Applications (traffic) ---------------------------
  // UDP OnOff client (sender -> receiver).  The IMPORTANT bit:
  //   • Offered load (OnOff DataRate) is set VERY HIGH (100 Mbps), far above the PHY,
  //     to force saturation — per NOTE1 in the spec.
  //   • Packet size is 1000 B (as required).
  //   • The app runs from t=[1s,10s]; we measure goodput over the 9 s active window.

  // Flow 1: Node0 -> Node1 (AP) on port 9
  OnOffHelper onoff1("ns3::UdpSocketFactory",
                    InetSocketAddress(apIface.GetAddress(0), 9)); // -> receiver:port 9
  onoff1.SetAttribute("DataRate",   StringValue("100Mbps"));        // saturating offered load
  onoff1.SetAttribute("PacketSize", UintegerValue(payload));           // payload size
  onoff1.SetAttribute("OnTime",     StringValue("ns3::ConstantRandomVariable[Constant=1]"));
  onoff1.SetAttribute("OffTime",    StringValue("ns3::ConstantRandomVariable[Constant=0]"));

  // Flow 2: Node2 -> Node1 (AP) on port 10
  ApplicationContainer client1 = onoff1.Install(staNodes.Get(0)); // sender index 0
  client1.Start(Seconds(1.0));
  client1.Stop (Seconds(10.0));

  OnOffHelper onoff2("ns3::UdpSocketFactory",
                    InetSocketAddress(apIface.GetAddress(0), 10)); // -> receiver:port 9
  onoff2.SetAttribute("DataRate",   StringValue("100Mbps"));        // saturating offered load
  onoff2.SetAttribute("PacketSize", UintegerValue(payload));           // payload size
  onoff2.SetAttribute("OnTime",     StringValue("ns3::ConstantRandomVariable[Constant=1]"));
  onoff2.SetAttribute("OffTime",    StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  
  ApplicationContainer client2 = onoff2.Install(staNodes.Get(1)); // sender index 0
  client2.Start(Seconds(1.0));
  client2.Stop (Seconds(10.0));

  // Packet sinks on AP (Node1) for both flows
  PacketSinkHelper sink1("ns3::UdpSocketFactory",
                        InetSocketAddress(Ipv4Address::GetAny(), 9));
  PacketSinkHelper sink2("ns3::UdpSocketFactory",
                        InetSocketAddress(Ipv4Address::GetAny(), 10));
  ApplicationContainer server1 = sink1.Install(apNode);
  ApplicationContainer server2 = sink2.Install(apNode);
  server1.Start(Seconds(0.0));
  server2.Start(Seconds(0.0));
  server1.Stop(Seconds(10.0));
  server2.Stop(Seconds(10.0));

  // ---------------------------- FlowMonitor + NetAnim ------------------------------
  FlowMonitorHelper fmHelper;
  Ptr<FlowMonitor> monitor = fmHelper.InstallAll();

  // NetAnim trace (deliverable‑compliant filename)
  std::string namePrefix = "scenario2_" + std::string(useRtsCts ? "with_rts" : "without_rts");
  AnimationInterface anim(namePrefix + "_anim.xml");
  // Make the animation self‑describing
  anim.SetConstantPosition(apNode.Get(0), 0.0, antHeight);
  anim.SetConstantPosition(staNodes.Get(0), -distance, antHeight);
  anim.SetConstantPosition(staNodes.Get(1), distance, antHeight);
  anim.UpdateNodeDescription(apNode.Get(0),   "AP receiver");
  anim.UpdateNodeDescription(staNodes.Get(0), "STA sender 1");
  anim.UpdateNodeDescription(staNodes.Get(1), "STA sender 2");

  // ------------------------------- PCAP Tracing ------------------------------
  phy.EnablePcapAll(namePrefix);

  // ------------------------------- Run the simulation ------------------------------
  Simulator::Stop(Seconds(10.0));
  Simulator::Run();

  // ------------------------------ Throughput calculation ---------------------------
  // Sum all bytes received at the sink(s) and divide by active duration (9 s).
  monitor->CheckForLostPackets();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(fmHelper.GetClassifier());
  const auto &stats = monitor->GetFlowStats();

  std::cout << "\n--- Flow Monitor Statistics ---\n";
  for (const auto &kv : stats)
  {
    FlowId flowId = kv.first;
    const FlowMonitor::FlowStats &flowStats = kv.second;
    
    std::cout << "Flow " << flowId << ": "
              << " Tx Packets: " << flowStats.txPackets
              << ", Rx Packets: " << flowStats.rxPackets
              << ", Lost Packets: " << flowStats.lostPackets
              << ", PDR: " << (flowStats.rxPackets * 100.0 / (flowStats.rxPackets + flowStats.lostPackets)) << "%"
              << ", Throughput: " << (flowStats.rxBytes * 8.0 / 9.0 / 1e6) << " Mbps"
              << std::endl;
  }

  // Calculate overall throughput
  uint64_t totalRxBytes = 0;
  uint64_t totalTxPackets = 0;
  uint64_t totalRxPackets = 0;
  const double activeSecs = 9.0;  // apps active in [1,10]s

  for (auto const &kv : stats) {
    Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(kv.first);
    double throughput = kv.second.rxBytes * 8.0 / activeSecs / 1e6;
    double pdr = (kv.second.txPackets > 0)
                   ? double(kv.second.rxPackets) / kv.second.txPackets
                   : 0.0;
    std::cout << "Flow " << t.sourceAddress << " -> " << t.destinationAddress
              << " throughput=" << throughput << " Mbps"
              << " PDR=" << pdr << std::endl;
  }

  const double goodput_bps = (totalRxBytes * 8.0) / activeSecs;
  const double pdr = (totalRxPackets * 100.0) / totalTxPackets;

  std::cout << "AP address=" << apIface.GetAddress(0) << std::endl;
  std::cout << "STA0 addr=" << staIfaces.GetAddress(0) << std::endl;
  std::cout << "STA1 addr=" << staIfaces.GetAddress(1) << std::endl;

  std::cout << "\n[Scenario2] PHYMode=" << mode
            << "  distance=" << distance
            << "  antHeight=" << antHeight
            << "  payload=" << payload
            << "  useRtsCts=" << (useRtsCts ? "true" : "false")
            << "  offered=100Mbps"
            << "  totalRxBytes=" << totalRxBytes
            << "  throughput=" << goodput_bps << " bps (" << (goodput_bps/1e6) << " Mbps)"
            << "  PDR=" << pdr << "%"
            << std::endl;

  Simulator::Destroy();
  return 0;
}
