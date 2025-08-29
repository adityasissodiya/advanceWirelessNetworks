/*
 * Lab 1: COST231-Hata Propagation Loss Model
 * Usage (from ns-3 root): ./ns3 run "scratch/Lab1_Cpp_Cost231 --distance=X"
 */
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/netanim-module.h"   // <-- needed for AnimationInterface

using namespace ns3;

int main (int argc, char *argv[])
{
  double distance = 50.0; // meters
  CommandLine cmd;
  cmd.AddValue ("distance", "Distance between nodes (m)", distance);
  cmd.Parse (argc, argv);

  Time::SetResolution (Time::NS);

  // Nodes
  NodeContainer nodes;
  nodes.Create (2);

  // Channel + propagation
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  channel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");

  // COST231 attributes:
  // - CitySize: "SmallCity" | "MediumCity" | "LargeCity"
  // - Frequency: in MHz (for ns-3.40 COST231 model)
  // - C: 0 dB (medium/small city) or 3 dB (metropolitan); using 10 here per your starter, but 0 or 3 are typical.
channel.AddPropagationLoss("ns3::Cost231PropagationLossModel",
                              "Frequency",       DoubleValue(1.8e9),  // Hz
                              "BSAntennaHeight", DoubleValue(50.0),   // m (typical macro)
                              "SSAntennaHeight", DoubleValue(3.0),    // m (UE)
                              "MinDistance",     DoubleValue(0.5));   // m

  // PHY
  YansWifiPhyHelper phy;                     // <-- no ::Default() in ns-3.40
  phy.SetChannel (channel.Create ());

  // Wi-Fi helpers
  WifiHelper wifi;
  // Use modern enum (ns-3.40) – old WIFI_PHY_STANDARD_* is gone
  wifi.SetStandard (WIFI_STANDARD_80211a);

  // Fix the accidental literal "\n" and set constant rate
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue ("OfdmRate6Mbps"),
                                "ControlMode", StringValue ("OfdmRate6Mbps"));

  WifiMacHelper mac;
  mac.SetType ("ns3::AdhocWifiMac");

  NetDeviceContainer devices = wifi.Install (phy, mac, nodes);

  // Mobility
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> pos = CreateObject<ListPositionAllocator> ();
  pos->Add (Vector (0.0, 0.0, 0.0));
  pos->Add (Vector (distance, 0.0, 0.0));
  mobility.SetPositionAllocator (pos);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);

  // Internet stack + IPs
  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer ifaces = address.Assign (devices);

  // Traffic: UDP OnOff → PacketSink
  OnOffHelper onoff ("ns3::UdpSocketFactory",
                     InetSocketAddress (ifaces.GetAddress (1), 9));
  onoff.SetAttribute ("DataRate", StringValue ("1Mbps"));
  onoff.SetAttribute ("PacketSize", UintegerValue (1000));
  // Optional: make it always-on (no burstiness)
  onoff.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

  ApplicationContainer txApp = onoff.Install (nodes.Get (0));
  txApp.Start (Seconds (1.0));
  txApp.Stop  (Seconds (10.0));

  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), 9));
  ApplicationContainer rxApp = sink.Install (nodes.Get (1));
  rxApp.Start (Seconds (0.0));
  rxApp.Stop  (Seconds (10.0));

  // Flow monitor
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

  // Stop + animation
  Simulator::Stop (Seconds (10.0));
  AnimationInterface anim ("Lab1_Cost231.xml");

  Simulator::Run ();

  // Throughput calculation (sum all flows' rxBytes)
  monitor->CheckForLostPackets ();
  const auto &stats = monitor->GetFlowStats ();

  uint64_t totalRxBytes = 0;
  for (const auto &kv : stats) {
    totalRxBytes += kv.second.rxBytes;
  }

  // Active traffic runs from t=1s to t=10s → 9 seconds
  const double activeSecs = 9.0;
  const double throughput_bps = (totalRxBytes * 8.0) / activeSecs;

  std::cout << "Distance(m)=" << distance
            << "  TotalRxBytes=" << totalRxBytes
            << "  Throughput=" << throughput_bps << " bps"
            << " (" << throughput_bps / 1e6 << " Mbps)"
            << std::endl;

  Simulator::Destroy ();
  return 0;
}
