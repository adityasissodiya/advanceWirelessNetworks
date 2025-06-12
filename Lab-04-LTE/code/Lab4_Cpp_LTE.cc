// Lab 4 Starter: LTE EPC End-to-End Throughput
// Usage: cmake-build-dir/src/scratch/Lab4_Cpp_LTE --dataRate=<Mbps> --antenna=<type> --distance=<m> --seed=<run>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/lte-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

int main(int argc, char *argv[])
{
    // Parameters
    double dataRate = 1.0;         // Mbps for OnOff
    std::string antennaType = "Isotropic"; // Parabolic, Cosine, Isotropic
    double distance = 100.0;       // UE distance (m)
    uint32_t seed = 1;

    CommandLine cmd;
    cmd.AddValue("dataRate", "Application data rate in Mbps", dataRate);
    cmd.AddValue("antenna", "Antenna model: Parabolic, Cosine, Isotropic", antennaType);
    cmd.AddValue("distance", "Distance between eNodeB and UE (m)", distance);
    cmd.AddValue("seed", "RngRun seed value", seed);
    cmd.Parse(argc, argv);

    // RNG and time
    RngSeedManager::SetSeed(1);
    RngSeedManager::SetRun(seed);
    Time::SetResolution(Time::NS);

    // Create EPC
    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
    Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper>();
    lteHelper->SetEpcHelper(epcHelper);

    // Configure AMc and scheduler
    lteHelper->SetAttribute("PathlossModel", StringValue("ns3::TwoRayGroundPropagationLossModel"));
    lteHelper->SetEnbAntennaModelType(antennaType);
    lteHelper->SetAttribute("DlEarfcn", UintegerValue(100));
    lteHelper->SetAttribute("UlEarfcn", UintegerValue(18100));
    lteHelper->SetSchedulerType("ns3::PfFfMacScheduler");

    // Create nodes: eNodeB and UE
    NodeContainer enbNodes;
    enbNodes.Create(1);
    NodeContainer ueNodes;
    ueNodes.Create(1);

    // Install mobility
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(enbNodes);
    Ptr<ListPositionAllocator> posAlloc = CreateObject<ListPositionAllocator>();
    posAlloc->Add(Vector(0.0, 0.0, 0.0));
    posAlloc->Add(Vector(distance, 0.0, 0.0));
    mobility.SetPositionAllocator(posAlloc);
    mobility.Install(ueNodes);

    // Install LTE devices
    NetDeviceContainer enbDevs = lteHelper->InstallEnbDevice(enbNodes);
    NetDeviceContainer ueDevs = lteHelper->InstallUeDevice(ueNodes);

    // Install IP stack on UEs
    InternetStackHelper internet;
    internet.Install(ueNodes);
    Ptr<Node> pgw = epcHelper->GetPgwNode();
    // Create remote host
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create(1);
    internet.Install(remoteHostContainer);
    // Point-to-point link to PGW
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute("DataRate", StringValue("100Gbps"));
    p2ph.SetChannelAttribute("Delay", StringValue("10ms"));
    NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHostContainer.Get(0));
    // Assign IP
    Ipv4AddressHelper address;
    address.SetBase("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIfaces = address.Assign(internetDevices);

    // Assign IP to UE
    Ipv4InterfaceContainer ueIfaces = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueDevs));

    // Attach UE to eNodeB
    lteHelper->Attach(ueDevs.Get(0), enbDevs.Get(0));

    // Populate routing tables
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting(
        remoteHostContainer.Get(0)->GetObject<Ipv4>());
    remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);

    // Install applications: UDP OnOff on remoteHost -> UE
    uint16_t port = 8000;
    OnOffHelper onoff("ns3::UdpSocketFactory",
                      InetSocketAddress(ueIfaces.GetAddress(0), port));
    onoff.SetAttribute("DataRate", DataRateValue(DataRate(uint64_t(dataRate * 1e6))));
    onoff.SetAttribute("PacketSize", UintegerValue(1024));
    ApplicationContainer clientApps = onoff.Install(remoteHostContainer.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(20.0));

    PacketSinkHelper sinkHelper("ns3::UdpSocketFactory",
                                 InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApp = sinkHelper.Install(ueNodes.Get(0));
    sinkApp.Start(Seconds(1.0));
    sinkApp.Stop(Seconds(20.0));

    // FlowMonitor & NetAnim
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();
    AnimationInterface anim("Lab4_LTE.xml");

    Simulator::Stop(Seconds(21.0));
    Simulator::Run();

    // Throughput
    monitor->CheckForLostPackets();
    auto stats = monitor->GetFlowStats();
    double rx = stats[1].rxBytes;
    double throughput = rx * 8.0 / 19.0;  // bps
    std::cout << "LTE downlink throughput: " << throughput << " bps" << std::endl;

    Simulator::Destroy();
    return 0;
}