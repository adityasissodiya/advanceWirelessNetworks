// Lab 2: Infrastructure WiFi Scenario 2 (Two STAs to one AP)
// Usage (from ns-3 root): ./ns3 run "scratch/Lab2_Cpp_Scenario2 --rate=<Mbps> --seed=<run>"

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

// Map a numeric Mbps to a valid 802.11b WifiMode string
static std::string ModeForB(double rateMbps) {
  if (rateMbps <= 1.0)   return "DsssRate1Mbps";
  if (rateMbps <= 2.0)   return "DsssRate2Mbps";
  if (rateMbps <= 5.5)   return "DsssRate5_5Mbps";
  return "DsssRate11Mbps";
}

int main (int argc, char *argv[])
{
  // Parameters
  double rate = 1.0;          // Mbps (PHY mode & offered load)
  uint32_t seed = 1;
  CommandLine cmd;
  cmd.AddValue("rate", "Physical layer data rate in Mbps (1,2,5.5,11 -> rounded up)", rate);
  cmd.AddValue("seed", "RngRun seed value", seed);
  cmd.Parse(argc, argv);

  // Randomization
  RngSeedManager::SetSeed (1);
  RngSeedManager::SetRun (seed);

  Time::SetResolution (Time::NS);

  // Nodes: 2 STAs + 1 AP
  NodeContainer staNodes;
  staNodes.Create (2);
  NodeContainer apNode;
  apNode.Create (1);

  // Channel/PHY
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
  channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");

  YansWifiPhyHelper phy; // ns-3.40: no ::Default()
  phy.SetChannel(channel.Create());

  // Wi-Fi helpers
  WifiHelper wifi;
  wifi.SetStandard(WIFI_STANDARD_80211b);
  const std::string mode = ModeForB(rate);
  wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                               "DataMode", StringValue(mode),
                               "ControlMode", StringValue(mode));

  WifiMacHelper mac;
  Ssid ssid = Ssid("lab2-ssid");

  mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid));
  NetDeviceContainer staDevices = wifi.Install(phy, mac, staNodes);

  mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
  NetDeviceContainer apDevice = wifi.Install(phy, mac, apNode);

  // Mobility: STA0 --- AP --- STA1 (10 m apart)
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> pos = CreateObject<ListPositionAllocator>();
  const double d = 10.0;
  pos->Add(Vector(-d, 0.0, 0.0)); // STA0
  pos->Add(Vector( 0.0, 0.0, 0.0)); // AP
  pos->Add(Vector( d, 0.0, 0.0)); // STA1
  mobility.SetPositionAllocator(pos);
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(staNodes);
  mobility.Install(apNode);

  // Internet stack + IPs
  InternetStackHelper stack;
  stack.Install(staNodes);
  stack.Install(apNode);

  Ipv4AddressHelper address;
  address.SetBase("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer staIfaces = address.Assign(staDevices);
  Ipv4InterfaceContainer apIfaces  = address.Assign(apDevice);   // <-- keep AP iface separately

  // Applications: two UDP OnOff clients to AP (ports 9 and 10)
  std::ostringstream offered;
  offered << rate << "Mbps";

  OnOffHelper onoff1("ns3::UdpSocketFactory",
                     InetSocketAddress(apIfaces.GetAddress(0), 9));  // STA0 -> AP:9
  onoff1.SetAttribute("DataRate", StringValue(offered.str()));
  onoff1.SetAttribute("PacketSize", UintegerValue(1000));
  onoff1.SetAttribute("OnTime",  StringValue("ns3::ConstantRandomVariable[Constant=1]"));
  onoff1.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  ApplicationContainer app1 = onoff1.Install(staNodes.Get(0));
  app1.Start(Seconds(1.0));
  app1.Stop (Seconds(10.0));

  OnOffHelper onoff2("ns3::UdpSocketFactory",
                     InetSocketAddress(apIfaces.GetAddress(0), 10)); // STA1 -> AP:10
  onoff2.SetAttribute("DataRate", StringValue(offered.str()));
  onoff2.SetAttribute("PacketSize", UintegerValue(1000));
  onoff2.SetAttribute("OnTime",  StringValue("ns3::ConstantRandomVariable[Constant=1]"));
  onoff2.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  ApplicationContainer app2 = onoff2.Install(staNodes.Get(1));
  app2.Start(Seconds(1.0));
  app2.Stop (Seconds(10.0));

  // Packet sinks on AP (ports 9 and 10)
  PacketSinkHelper sink1("ns3::UdpSocketFactory",
                         InetSocketAddress(Ipv4Address::GetAny(), 9));
  PacketSinkHelper sink2("ns3::UdpSocketFactory",
                         InetSocketAddress(Ipv4Address::GetAny(), 10));
  ApplicationContainer srv1 = sink1.Install(apNode.Get(0));
  ApplicationContainer srv2 = sink2.Install(apNode.Get(0));
  srv1.Start(Seconds(0.0)); srv1.Stop(Seconds(10.0));
  srv2.Start(Seconds(0.0)); srv2.Stop(Seconds(10.0));

  // FlowMonitor & NetAnim
  FlowMonitorHelper fmHelper;
  Ptr<FlowMonitor> monitor = fmHelper.InstallAll();
  AnimationInterface anim("Lab2_Scenario2.xml");

  // Run simulation
  Simulator::Stop(Seconds(10.0));
  Simulator::Run();

  // Per-flow throughput via classifier (don’t assume flow IDs)
  monitor->CheckForLostPackets();
  const auto &stats = monitor->GetFlowStats();

  Ptr<Ipv4FlowClassifier> classifier =
      DynamicCast<Ipv4FlowClassifier>(fmHelper.GetClassifier());

  uint64_t rxPort9Bytes = 0, rxPort10Bytes = 0;
  for (const auto &kv : stats) {
    Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(kv.first);
    if (t.destinationPort == 9)  rxPort9Bytes  += kv.second.rxBytes;
    if (t.destinationPort == 10) rxPort10Bytes += kv.second.rxBytes;
  }

  const double activeSecs = 9.0; // apps run 1s..10s
  const double thr1 = (rxPort9Bytes  * 8.0) / activeSecs;
  const double thr2 = (rxPort10Bytes * 8.0) / activeSecs;

  std::cout << "Scenario2: PHYMode=" << mode
            << ", offered(each)=" << offered.str()
            << ", rxBytes(flow1@port9)=" << rxPort9Bytes
            << ", rxBytes(flow2@port10)=" << rxPort10Bytes << "\n";
  std::cout << "  throughput flow1: " << thr1  << " bps (" << thr1/1e6  << " Mbps)\n";
  std::cout << "  throughput flow2: " << thr2  << " bps (" << thr2/1e6  << " Mbps)\n";

  Simulator::Destroy();
  return 0;
}
