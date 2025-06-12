// Lab 3 Starter: TCP vs UDP in Ad-hoc Chain
// Usage: cmake-build-dir/src/scratch/Lab3_Cpp_TCP \
//        --pktSize=<bytes> --seed=<run>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

int main(int argc, char *argv[])
{
    uint32_t pktSize = 300;
    uint32_t seed = 1;
    CommandLine cmd;
    cmd.AddValue("pktSize", "TCP segment/UDP packet size", pktSize);
    cmd.AddValue("seed", "RngRun seed", seed);
    cmd.Parse(argc, argv);

    RngSeedManager::SetSeed(1);
    RngSeedManager::SetRun(seed);
    Time::SetResolution(Time::NS);

    NodeContainer nodes;
    nodes.Create(3);

    // WiFi ad-hoc
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
    mac.SetType("ns3::AdhocWifiMac");
    NetDeviceContainer devices = wifi.Install(phy, mac, nodes);

    MobilityHelper mob;
    Ptr<ListPositionAllocator> pos = CreateObject<ListPositionAllocator>();
    pos->Add(Vector(0,0,0)); pos->Add(Vector(200,0,0)); pos->Add(Vector(400,0,0));
    mob.SetPositionAllocator(pos);
    mob.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mob.Install(nodes);

    InternetStackHelper internet;
    internet.Install(nodes);

    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(pktSize));

    Ipv4AddressHelper addr;
    addr.SetBase("10.1.6.0", "255.255.255.0");
    Ipv4InterfaceContainer ifs = addr.Assign(devices);

    // TCP on Node0 -> Node2
    PacketSinkHelper sink("ns3::TcpSocketFactory",
                          InetSocketAddress(Ipv4Address::GetAny(), 9));
    auto server = sink.Install(nodes.Get(2));
    server.Start(Seconds(0.0)); server.Stop(Seconds(10.0));

    OnOffHelper client("ns3::TcpSocketFactory",
                       InetSocketAddress(ifs.GetAddress(2), 9));
    client.SetAttribute("DataRate", StringValue("5Mbps"));  // high to saturate
    client.SetAttribute("PacketSize", UintegerValue(pktSize));
    auto apps = client.Install(nodes.Get(0));
    apps.Start(Seconds(1.0)); apps.Stop(Seconds(10.0));

    FlowMonitorHelper fm;
    Ptr<FlowMonitor> mon = fm.InstallAll();

    Simulator::Stop(Seconds(11.0));
    Simulator::Run();

    mon->CheckForLostPackets();
    double rx = mon->GetFlowStats()[1].rxBytes;
    double thr = rx * 8.0 / 10.0;
    std::cout << "TCP throughput: " << thr << " bps" << std::endl;

    Simulator::Destroy();
    return 0;
}