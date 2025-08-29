/*
 * Lab 1: Two-Ray Ground Propagation Loss Model
 * Usage (from ns-3 root): ./ns3 run "scratch/Lab1_Cpp_TwoRay --distance=X"
 */
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/netanim-module.h"   // AnimationInterface

using namespace ns3;

int main (int argc, char *argv[])
{
  double distance = 50.0; // meters
  double antHeight = 1.5; // meters (use Z as antenna height)
  CommandLine cmd;
  cmd.AddValue ("distance", "Distance between nodes (m)", distance);
  cmd.AddValue ("antHeight", "Antenna height used as Z (m)", antHeight);
  cmd.Parse (argc, argv);

  Time::SetResolution (Time::NS);

  // Nodes
  NodeContainer nodes;
  nodes.Create (2);

  // Channel + propagation (Two-Ray)
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  channel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  channel.AddPropagationLoss ("ns3::TwoRayGroundPropagationLossModel",
                              "Frequency",   DoubleValue (2.4e9), // Hz
                              "MinDistance", DoubleValue (1.0));  // avoid near-field singularity

  // PHY
  YansWifiPhyHelper phy;                 // no ::Default() in ns-3.40
  phy.SetChannel (channel.Create ());

  // Wi-Fi helpers
  WifiHelper wifi;
  wifi.SetStandard (WIFI_STANDARD_80211a);
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode",    StringValue ("OfdmRate6Mbps"),
                                "ControlMode", StringValue ("OfdmRate6Mbps"));

  WifiMacHelper mac;
  mac.SetType ("ns3::AdhocWifiMac");

  NetDeviceContainer devices = wifi.Install (phy, mac, nodes);

  // Mobility (use Z as antenna height for Two-Ray)
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> pos = CreateObject<ListPositionAllocator> ();
  pos->Add (Vector (0.0,        0.0, antHeight));
  pos->Add (Vector (distance,   0.0, antHeight));
  mobility.SetPositionAllocator (pos);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);

  // Internet + IPs
  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer ifaces = address.Assign (devices);

  // Traffic: UDP OnOff → PacketSink
  OnOffHelper onoff ("ns3::UdpSocketFactory",
                     InetSocketAddress (ifaces.GetAddress (1), 9));
  onoff.SetAttribute ("DataRate", StringValue ("1Mbps"));
  onoff.SetAttribute ("PacketSize", UintegerValue (1000));
  onoff.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

  ApplicationContainer tx = onoff.Install (nodes.Get (0));
  tx.Start (Seconds (1.0));
  tx.Stop  (Seconds (10.0));

  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), 9));
  ApplicationContainer rx = sink.Install (nodes.Get (1));
  rx.Start (Seconds (0.0));
  rx.Stop  (Seconds (10.0));

  // Flow monitor
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

  // Stop + animation
  Simulator::Stop (Seconds (10.0));
  AnimationInterface anim ("Lab1_TwoRay.xml");

  Simulator::Run ();

  // Throughput (sum across all flows)
  monitor->CheckForLostPackets ();
  const auto &stats = monitor->GetFlowStats ();
  uint64_t totalRxBytes = 0;
  for (const auto &kv : stats) {
    totalRxBytes += kv.second.rxBytes;
  }
  const double activeSecs = 9.0; // app from 1s..10s
  const double thr_bps = (totalRxBytes * 8.0) / activeSecs;

  std::cout << "TwoRay: distance=" << distance
            << "m, antHeight=" << antHeight << "m, totalRxBytes=" << totalRxBytes
            << ", throughput=" << thr_bps << " bps (" << thr_bps/1e6 << " Mbps)\n";

  Simulator::Destroy ();
  return 0;
}
