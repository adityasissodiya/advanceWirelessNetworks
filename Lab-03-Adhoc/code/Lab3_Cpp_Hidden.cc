// Lab 3 Starter: Hidden Terminal in Ad-hoc Network
// Usage: cmake-build-dir/src/scratch/Lab3_Cpp_Hidden \
//        --distance=<m> --seed=<run> --enableRtsCts=<0|1>

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
    double distance = 200.0;
    uint32_t seed = 1;
    bool enableRtsCts = false;
    CommandLine cmd;
    cmd.AddValue("distance", "Distance STA-AP (m)", distance);
    cmd.AddValue("seed", "RngRun seed", seed);
    cmd.AddValue("enableRtsCts", "Enable RTS/CTS (1) or disable (0)", enableRtsCts);
    cmd.Parse(argc, argv);

    RngSeedManager::SetSeed(1);
    RngSeedManager::SetRun(seed);
    Time::SetResolution(Time::NS);

    NodeContainer nodes;
    nodes.Create(3); // STA0, AP, STA1

    // Set RTS/CTS threshold before devices install
    Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", \
                       StringValue(enableRtsCts ? "0" : "2200"));

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    channel.AddPropagationLoss("ns3::TwoRayGroundPropagationLossModel");

    YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
    phy.SetChannel(channel.Create());

    WifiHelper wifi;
    wifi.SetStandard(WIFI_PHY_STANDARD_80211b);
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                 "DataMode", StringValue("DsssRate1Mbps"),
                                 "ControlMode", StringValue("DsssRate1Mbps"));

    WifiMacHelper mac;
    Ssid ssid = Ssid("lab3-hidden");

    // STA0 & STA1
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid));
    NetDeviceContainer staDevices = wifi.Install(phy, mac, nodes.Get(0), nodes.Get(2));

    // AP
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    NetDeviceContainer apDevice = wifi.Install(phy, mac, nodes.Get(1));

    // Mobility: STA0 at (0), AP at (distance), STA1 at (2*distance)
    MobilityHelper mob;
    Ptr<ListPositionAllocator> pos = CreateObject<ListPositionAllocator>();
    pos->Add(Vector(0.0, 0.0, 0.0));
    pos->Add(Vector(distance, 0.0, 0.0));
    pos->Add(Vector(2*distance, 0.0, 0.0));
    mob.SetPositionAllocator(pos);
    mob.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mob.Install(nodes);

    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper addr;
    addr.SetBase("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer ifaces = addr.Assign(staDevices);
    addr.Assign(apDevice);

    // Applications (UDP)
    OnOffHelper onoff1("ns3::UdpSocketFactory",
                       InetSocketAddress(ifaces.GetAddress(0), 9));
    onoff1.SetAttribute("DataRate", StringValue("1Mbps"));
    onoff1.SetAttribute("PacketSize", UintegerValue(1000));
    auto app1 = onoff1.Install(nodes.Get(0));
    app1.Start(Seconds(1.0)); app1.Stop(Seconds(10.0));

    OnOffHelper onoff2("ns3::UdpSocketFactory",
                       InetSocketAddress(ifaces.GetAddress(2), 10));
    onoff2.SetAttribute("DataRate", StringValue("1Mbps"));
    onoff2.SetAttribute("PacketSize", UintegerValue(1000));
    auto app2 = onoff2.Install(nodes.Get(2));
    app2.Start(Seconds(1.0)); app2.Stop(Seconds(10.0));

    // Packet sinks on AP
    PacketSinkHelper sink1("ns3::UdpSocketFactory",
                          InetSocketAddress(Ipv4Address::GetAny(), 9));
    PacketSinkHelper sink2("ns3::UdpSocketFactory",
                          InetSocketAddress(Ipv4Address::GetAny(), 10));
    auto srv1 = sink1.Install(nodes.Get(1)); srv1.Start(Seconds(0.0)); srv1.Stop(Seconds(10.0));
    auto srv2 = sink2.Install(nodes.Get(1)); srv2.Start(Seconds(0.0)); srv2.Stop(Seconds(10.0));

    FlowMonitorHelper fm;
    Ptr<FlowMonitor> monitor = fm.InstallAll();
    AnimationInterface anim("Lab3_Hidden.xml");

    Simulator::Stop(Seconds(11.0));
    Simulator::Run();

    monitor->CheckForLostPackets();
    auto stats = monitor->GetFlowStats();
    double thr0 = stats[1].rxBytes * 8.0 / 9.0;
    double thr1 = stats[2].rxBytes * 8.0 / 9.0;
    std::cout << "Hidden Off flow1: " << thr0 << " bps\n";
    std::cout << "Hidden Off flow2: " << thr1 << " bps\n";

    Simulator::Destroy();
    return 0;
}