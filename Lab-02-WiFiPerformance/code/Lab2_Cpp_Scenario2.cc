// code/Lab2\_Cpp\_Scenario2.cc
// Lab 2 Starter: Infrastructure WiFi Scenario 2 (Two Triangles)
// Usage: cmake-build-dir/src/scratch/Lab2\_Cpp\_Scenario2 --rate=<Mbps> --seed=<run>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

int
main (int argc, char \*argv\[])
{
// Parameters
double rate = 1.0;          // Mbps
uint32\_t seed = 1;
CommandLine cmd;
cmd.AddValue("rate", "Physical layer data rate in Mbps", rate);
cmd.AddValue("seed", "RngRun seed value", seed);
cmd.Parse(argc, argv);

// Randomization
RngSeedManager::SetSeed (1);
RngSeedManager::SetRun (seed);

Time::SetResolution (Time::NS);

// Create nodes: 2 STA senders + 1 AP
NodeContainer staNodes;
staNodes.Create (2);
NodeContainer apNode;
apNode.Create (1);

// WiFi setup
YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
phy.SetChannel(channel.Create());

WifiHelper wifi;
wifi.SetStandard(WIFI\_PHY\_STANDARD\_80211b);
std::ostringstream phyRate;
phyRate << rate << "Mbps";
wifi.SetRemoteStationManager(
"ns3::ConstantRateWifiManager",
"DataMode", StringValue("DsssRate11Mbps"),
"ControlMode", StringValue("DsssRate11Mbps")
);

WifiMacHelper mac;
Ssid ssid = Ssid ("lab2-ssid");

// Install STA devices
mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid));
NetDeviceContainer staDevices = wifi.Install(phy, mac, staNodes);

// Install AP device
mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
NetDeviceContainer apDevice = wifi.Install(phy, mac, apNode);

// Mobility: two triangles opposite around AP
MobilityHelper mobility;
Ptr<ListPositionAllocator> posAlloc = CreateObject<ListPositionAllocator>();
double d = 10.0;
posAlloc->Add (Vector(-d,  0.0, 0.0)); // STA0
posAlloc->Add (Vector( 0.0,  0.0, 0.0)); // AP
posAlloc->Add (Vector( d,   0.0, 0.0)); // STA1
mobility.SetPositionAllocator (posAlloc);
mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
mobility.Install (staNodes);
mobility.Install (apNode);

// Internet stack
InternetStackHelper stack;
stack.Install (staNodes);
stack.Install (apNode);

Ipv4AddressHelper address;
address.SetBase ("10.1.3.0", "255.255.255.0");
Ipv4InterfaceContainer staIfaces = address.Assign (staDevices);
address.Assign (apDevice);

// Applications: two OnOff clients to AP on ports 9 and 10
OnOffHelper onoff1 ("ns3::UdpSocketFactory",
InetSocketAddress (staIfaces.GetAddress(2), 9)); // STA0->AP
onoff1.SetAttribute ("DataRate", StringValue(phyRate.str()));
onoff1.SetAttribute ("PacketSize", UintegerValue(1000));
ApplicationContainer app1 = onoff1.Install (staNodes.Get(0));
app1.Start (Seconds (1.0));
app1.Stop  (Seconds (10.0));

OnOffHelper onoff2 ("ns3::UdpSocketFactory",
InetSocketAddress (staIfaces.GetAddress(2), 10)); // STA1->AP
onoff2.SetAttribute ("DataRate", StringValue(phyRate.str()));
onoff2.SetAttribute ("PacketSize", UintegerValue(1000));
ApplicationContainer app2 = onoff2.Install (staNodes.Get(1));
app2.Start (Seconds (1.0));
app2.Stop  (Seconds (10.0));

// Packet sinks on AP
PacketSinkHelper sink1 ("ns3::UdpSocketFactory",
InetSocketAddress (Ipv4Address::GetAny (), 9));
PacketSinkHelper sink2 ("ns3::UdpSocketFactory",
InetSocketAddress (Ipv4Address::GetAny (), 10));
ApplicationContainer srv1 = sink1.Install (apNode.Get(0));
ApplicationContainer srv2 = sink2.Install (apNode.Get(0));
srv1.Start (Seconds (0.0)); srv1.Stop (Seconds (10.0));
srv2.Start (Seconds (0.0)); srv2.Stop (Seconds (10.0));

// FlowMonitor & NetAnim
FlowMonitorHelper fmHelper;
Ptr<FlowMonitor> monitor = fmHelper.InstallAll ();
AnimationInterface anim ("Lab2\_Scenario2.xml");

// Run simulation
Simulator::Stop (Seconds (10.0));
Simulator::Run ();

// Report throughput for both flows
monitor->CheckForLostPackets ();
auto stats = monitor->GetFlowStats ();
double rxBytes1 = stats\[1].rxBytes;
double rxBytes2 = stats\[2].rxBytes;
double thr1 = rxBytes1 \* 8.0 / 9.0;
double thr2 = rxBytes2 \* 8.0 / 9.0;
std::cout << "Scenario2 throughput flow1: " << thr1 << " bps\n";
std::cout << "Scenario2 throughput flow2: " << thr2 << " bps\n";

Simulator::Destroy ();
return 0;
}