// code/Lab3_Cpp_Adhoc.cc
// Lab 3 Starter: Ad-hoc Multi-hop UDP Chain Performance
// Usage: cmake-build-dir/src/scratch/Lab3_Cpp_Adhoc --numNodes= --pktSize= --distance= --seed=

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

int main(int argc, char *argv[])
{
// Command-line parameters
uint32_t numNodes = 3;
uint32_t pktSize = 1000;
double distance = 200.0;
uint32_t seed = 1;
CommandLine cmd;
cmd.AddValue("numNodes", "Number of nodes in chain", numNodes);
cmd.AddValue("pktSize", "UDP packet size in bytes", pktSize);
cmd.AddValue("distance", "Distance between adjacent nodes (m)", distance);
cmd.AddValue("seed", "RngRun seed value", seed);
cmd.Parse(argc, argv);
// RNG setup
RngSeedManager::SetSeed(1);
RngSeedManager::SetRun(seed);

Time::SetResolution(Time::NS);

// Create nodes
NodeContainer nodes;
nodes.Create(numNodes);

// Wi-Fi ad-hoc setup
YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
Ptr<TwoRayGroundPropagationLossModel> lossModel = CreateObject<TwoRayGroundPropagationLossModel>();
channel.AddPropagationLoss("ns3::TwoRayGroundPropagationLossModel");

YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
phy.SetChannel(channel.Create());

WifiHelper wifi;
wifi.SetStandard(WIFI_PHY_STANDARD_80211b);
wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                             "DataMode", StringValue("DsssRate1Mbps"),
                             "ControlMode", StringValue("DsssRate1Mbps"));

WifiMacHelper mac;
mac.SetType("ns3::AdhocWifiMac");
NetDeviceContainer devices = wifi.Install(phy, mac, nodes);

// Mobility: place nodes in straight line at given distance
MobilityHelper mobility;
Ptr<ListPositionAllocator> posAlloc = CreateObject<ListPositionAllocator>();
for (uint32_t i = 0; i < numNodes; ++i)
{
    posAlloc->Add(Vector(distance * i, 0.0, 0.0));
}
mobility.SetPositionAllocator(posAlloc);
mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
mobility.Install(nodes);

// Internet stack
InternetStackHelper internet;
internet.Install(nodes);

Ipv4AddressHelper address;
address.SetBase("10.1.4.0", "255.255.255.0");
Ipv4InterfaceContainer interfaces = address.Assign(devices);

// OnOff UDP application: first node -> last node
OnOffHelper onoff("ns3::UdpSocketFactory",
                  InetSocketAddress(interfaces.GetAddress(numNodes-1), 9));
onoff.SetAttribute("DataRate", StringValue("1Mbps"));
onoff.SetAttribute("PacketSize", UintegerValue(pktSize));
ApplicationContainer clientApps = onoff.Install(nodes.Get(0));
clientApps.Start(Seconds(1.0));
clientApps.Stop(Seconds(10.0));

// Packet sink on last node
PacketSinkHelper sink("ns3::UdpSocketFactory",
                      InetSocketAddress(Ipv4Address::GetAny(), 9));
ApplicationContainer serverApps = sink.Install(nodes.Get(numNodes-1));
serverApps.Start(Seconds(0.0));
serverApps.Stop(Seconds(10.0));

// FlowMonitor & NetAnim
FlowMonitorHelper flowmon;
Ptr<FlowMonitor> monitor = flowmon.InstallAll();
AnimationInterface anim("Lab3_Adhoc.xml");

// Run simulation
Simulator::Stop(Seconds(11.0));
Simulator::Run();

// Throughput calculation
monitor->CheckForLostPackets();
auto stats = monitor->GetFlowStats();
double rxBytes = stats[1].rxBytes;  // flow ID 1 corresponds to our UDP flow
double throughput = rxBytes * 8.0 / 9.0;  // bps
std::cout << "Ad-hoc UDP throughput: " << throughput << " bps\n";

Simulator::Destroy();
return 0;
}