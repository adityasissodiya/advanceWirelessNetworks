// code/Lab2_Cpp_Scenario1.cc
// Lab 2 Starter: Infrastructure WiFi Scenario 1 (Single AP, One Sender & Receiver)
// Usage: cmake-build-dir/src/scratch/Lab2_Cpp_Scenario1 --rate=<Mbps> --seed=<run>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

int main (int argc, char *argv[])
{
  // Parameters
  double rate = 1.0;          // Mbps
  uint32_t seed = 1;
  CommandLine cmd;
  cmd.AddValue("rate", "Physical layer data rate in Mbps", rate);
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

  // Configure WiFi channel
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
  channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");

  YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
  phy.SetChannel(channel.Create());

  WifiHelper wifi;
  wifi.SetStandard(WIFI_PHY_STANDARD_80211b);
  std::ostringstream phyRate;
  phyRate << rate << "Mbps";
  wifi.SetRemoteStationManager(
      "ns3::ConstantRateWifiManager",
      "DataMode", StringValue("DsssRate11Mbps"),
      "ControlMode", StringValue("DsssRate11Mbps")
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

  // Install Internet stack
  InternetStackHelper stack;
  stack.Install (wifiApNode);
  stack.Install (wifiStaNodes);

  Ipv4AddressHelper address;
  address.SetBase("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer staInterfaces = address.Assign(staDevices);
  address.Assign(apDevice);

  // OnOff application: node0 -> node1
  OnOffHelper onoff("ns3::UdpSocketFactory",
                    InetSocketAddress(staInterfaces.GetAddress(1), 9));
  onoff.SetAttribute("DataRate", StringValue(phyRate.str()));
  onoff.SetAttribute("PacketSize", UintegerValue(1000));
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

  // Calculate throughput
  monitor->CheckForLostPackets();
  auto stats = monitor->GetFlowStats();
  double rxBytes = stats[1].rxBytes;
  double throughput = rxBytes * 8.0 / 9.0;  // bps
  std::cout << "Scenario1 throughput: " << throughput << " bps\n";

  Simulator::Destroy();
  return 0;
}
