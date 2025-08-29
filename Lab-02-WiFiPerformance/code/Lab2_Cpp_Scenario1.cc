// Lab 2 Starter: Infrastructure WiFi Scenario 1 (Single AP, One Sender & Receiver)
// Usage (from ns-3 root): ./ns3 run "scratch/Lab2_Cpp_Scenario1 --rate=<Mbps> --seed=<run>"

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
static std::string ModeForB (double rateMbps)
{
  // legal for 11b: 1, 2, 5.5, 11 Mbps
  if (rateMbps <= 1.0)     return "DsssRate1Mbps";
  if (rateMbps <= 2.0)     return "DsssRate2Mbps";
  if (rateMbps <= 5.5)     return "DsssRate5_5Mbps";
  /* else */                return "DsssRate11Mbps";
}

int main (int argc, char *argv[])
{
  // Parameters
  double rate = 1.0;   // Mbps (intended PHY data mode; we’ll also use it for app rate)
  uint32_t seed = 1;

  CommandLine cmd;
  cmd.AddValue("rate", "Physical layer data rate in Mbps (valid: 1, 2, 5.5, 11; rounded up)", rate);
  cmd.AddValue("seed", "RngRun seed value", seed);
  cmd.Parse(argc, argv);

  // Randomization
  RngSeedManager::SetSeed (1);
  RngSeedManager::SetRun (seed);

  Time::SetResolution (Time::NS);

  // Create nodes: 2 STA (sender & receiver) + 1 AP
  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (2);
  NodeContainer wifiApNode;
  wifiApNode.Create (1);

  // Wi-Fi channel
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
  channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");

  // PHY (ns-3.40: no ::Default())
  YansWifiPhyHelper phy;
  phy.SetChannel(channel.Create());

  // Wi-Fi helpers
  WifiHelper wifi;
  // ns-3.40 enum name (old WIFI_PHY_STANDARD_… no longer exists)
  wifi.SetStandard(WIFI_STANDARD_80211b);

  const std::string mode = ModeForB(rate);
  wifi.SetRemoteStationManager(
      "ns3::ConstantRateWifiManager",
      "DataMode", StringValue(mode),
      "ControlMode", StringValue(mode)
  );

  WifiMacHelper mac;
  Ssid ssid = Ssid ("lab2-ssid");

  mac.SetType("ns3::StaWifiMac",
              "Ssid", SsidValue(ssid));
  NetDeviceContainer staDevices = wifi.Install(phy, mac, wifiStaNodes);

  mac.SetType("ns3::ApWifiMac",
              "Ssid", SsidValue(ssid));
  NetDeviceContainer apDevice = wifi.Install(phy, mac, wifiApNode);

  // Mobility: place nodes in a row (10m apart)
  MobilityHelper mobility;
  mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                "MinX", DoubleValue(0.0),
                                "MinY", DoubleValue(0.0),
                                "DeltaX", DoubleValue(10.0),
                                "DeltaY", DoubleValue(0.0),
                                "GridWidth", UintegerValue(3),
                                "LayoutType", StringValue("RowFirst"));
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(wifiStaNodes);
  mobility.Install(wifiApNode);

  // Internet stack
  InternetStackHelper stack;
  stack.Install (wifiApNode);
  stack.Install (wifiStaNodes);

  Ipv4AddressHelper address;
  address.SetBase("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer staIfaces = address.Assign(staDevices);
  address.Assign(apDevice);

  // Application: UDP OnOff node0 -> node1
  std::ostringstream offered;
  offered << rate << "Mbps"; // match offered load to chosen PHY mode by default
  OnOffHelper onoff("ns3::UdpSocketFactory",
                    InetSocketAddress(staIfaces.GetAddress(1), 9));
  onoff.SetAttribute("DataRate", StringValue(offered.str()));
  onoff.SetAttribute("PacketSize", UintegerValue(1000));
  onoff.SetAttribute("OnTime",  StringValue("ns3::ConstantRandomVariable[Constant=1]"));
  onoff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));

  ApplicationContainer clientApp = onoff.Install(wifiStaNodes.Get(0));
  clientApp.Start(Seconds(1.0));
  clientApp.Stop(Seconds(10.0));

  // PacketSink on node1
  PacketSinkHelper sink("ns3::UdpSocketFactory",
                        InetSocketAddress(Ipv4Address::GetAny(), 9));
  ApplicationContainer serverApp = sink.Install(wifiStaNodes.Get(1));
  serverApp.Start(Seconds(0.0));
  serverApp.Stop(Seconds(10.0));

  // FlowMonitor & NetAnim
  FlowMonitorHelper fmHelper;
  Ptr<FlowMonitor> monitor = fmHelper.InstallAll();
  AnimationInterface anim("Lab2_Scenario1.xml");

  // Run simulation
  Simulator::Stop(Seconds(10.0));
  Simulator::Run();

  // Throughput calculation: sum across flows (GetFlowStats returns const map)
  monitor->CheckForLostPackets();
  const auto &stats = monitor->GetFlowStats();
  uint64_t totalRxBytes = 0;
  for (const auto &kv : stats) {
    totalRxBytes += kv.second.rxBytes;
  }
  const double activeSecs = 9.0; // 1s..10s
  const double throughput = (totalRxBytes * 8.0) / activeSecs;  // bps

  std::cout << "Scenario1: PHYMode=" << mode
            << ", offered=" << offered.str()
            << ", totalRxBytes=" << totalRxBytes
            << ", throughput=" << throughput << " bps ("
            << throughput/1e6 << " Mbps)\n";

  Simulator::Destroy();
  return 0;
}
