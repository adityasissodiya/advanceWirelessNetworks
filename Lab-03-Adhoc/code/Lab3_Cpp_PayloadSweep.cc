// Lab 3 Starter: Payload Sweep for Ad-hoc Chain
// Usage: cmake-build-dir/src/scratch/Lab3_Cpp_PayloadSweep \
//        --numNodes=<N> --distance=<m> --seed=<run>
// Hardcode packet sizes inside loop: {300,700,1200}

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
    uint32_t numNodes = 3;
    double distance = 200.0;
    uint32_t seed = 1;
    CommandLine cmd;
    cmd.AddValue("numNodes", "Number of nodes in chain", numNodes);
    cmd.AddValue("distance", "Distance between adjacent nodes (m)", distance);
    cmd.AddValue("seed", "RngRun seed value", seed);
    cmd.Parse(argc, argv);

    RngSeedManager::SetSeed(1);
    RngSeedManager::SetRun(seed);
    Time::SetResolution(Time::NS);

    std::vector<uint32_t> pktSizes = {300, 700, 1200};
    for (auto pktSize : pktSizes) {
        // Re-create and configure nodes
        NodeContainer nodes;
        nodes.Create(numNodes);

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

        MobilityHelper mobility;
        Ptr<ListPositionAllocator> pos = CreateObject<ListPositionAllocator>();
        for (uint32_t i = 0; i < numNodes; ++i)
            pos->Add(Vector(distance * i, 0.0, 0.0));
        mobility.SetPositionAllocator(pos);
        mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
        mobility.Install(nodes);

        InternetStackHelper internet;
        internet.Install(nodes);

        Ipv4AddressHelper address;
        address.SetBase("10.1.4.0", "255.255.255.0");
        Ipv4InterfaceContainer ifaces = address.Assign(devices);

        // OnOff application
        OnOffHelper onoff("ns3::UdpSocketFactory",
                          InetSocketAddress(ifaces.GetAddress(numNodes-1), 9));
        onoff.SetAttribute("DataRate", StringValue("1Mbps"));
        onoff.SetAttribute("PacketSize", UintegerValue(pktSize));
        ApplicationContainer client = onoff.Install(nodes.Get(0));
        client.Start(Seconds(1.0));
        client.Stop(Seconds(10.0));

        PacketSinkHelper sink("ns3::UdpSocketFactory",
                              InetSocketAddress(Ipv4Address::GetAny(), 9));
        ApplicationContainer server = sink.Install(nodes.Get(numNodes-1));
        server.Start(Seconds(0.0));
        server.Stop(Seconds(10.0));

        FlowMonitorHelper fmHelper;
        Ptr<FlowMonitor> monitor = fmHelper.InstallAll();

        Simulator::Stop(Seconds(11.0));
        Simulator::Run();

        monitor->CheckForLostPackets();
        double rxBytes = monitor->GetFlowStats()[1].rxBytes;
        double throughput = rxBytes * 8.0 / 9.0;
        std::cout << "pktSize=" << pktSize
                  << " bytes, throughput=" << throughput << " bps" << std::endl;

        Simulator::Destroy();
    }
    return 0;
}