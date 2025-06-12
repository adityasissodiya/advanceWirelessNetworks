#!/usr/bin/env python3
# code/Lab4_Py_LTE.py
# Lab 4 Python Starter: LTE EPC End-to-End Throughput
# Usage: python3 code/Lab4_Py_LTE.py --dataRate <Mbps> --antenna <type> --distance <m> --seed <run>

import ns.core
import ns.network
import ns.lte
import ns.internet
import ns.mobility
import ns.applications
import ns.flow_monitor
import ns.netanim

from ns.core import CommandLine, Seconds, UintegerValue, DataRateValue
from ns.network import NodeContainer, Ipv4AddressHelper
from ns.lte import LteHelper, PointToPointEpcHelper
from ns.internet import InternetStackHelper, Ipv4StaticRoutingHelper
from ns.mobility import MobilityHelper, ListPositionAllocator
from ns.applications import OnOffHelper, PacketSinkHelper
from ns.flow_monitor import FlowMonitorHelper
from ns.netanim import AnimationInterface

def main():
    # Parameters
    dataRate = 1.0
    antennaType = 'Isotropic'
    distance = 100.0
    seed = 1

    cmd = CommandLine()
    cmd.AddValue('dataRate', 'Application data rate in Mbps', dataRate)
    cmd.AddValue('antenna', 'Antenna model: Parabolic, Cosine, Isotropic', antennaType)
    cmd.AddValue('distance', 'UE distance (m)', distance)
    cmd.AddValue('seed', 'RngRun seed value', seed)
    cmd.Parse()

    # RNG & time
    ns.core.RngSeedManager.SetSeed(1)
    ns.core.RngSeedManager.SetRun(seed)
    ns.core.Time.SetResolution(ns.core.Time.NS)

    # LTE + EPC setup
    lte = LteHelper()
    epc = PointToPointEpcHelper()
    lte.SetEpcHelper(epc)
    lte.SetAttribute('PathlossModel', ns.core.StringValue('ns3::TwoRayGroundPropagationLossModel'))
    lte.SetEnbAntennaModelType(antennaType)
    lte.SetSchedulerType('ns3::PfFfMacScheduler')
    lte.SetAttribute('DlEarfcn', ns.core.UintegerValue(100))
    lte.SetAttribute('UlEarfcn', ns.core.UintegerValue(18100))

    # Create nodes
    enbNodes = NodeContainer()
    enbNodes.Create(1)
    ueNodes = NodeContainer()
    ueNodes.Create(1)

    # Mobility
    mob = MobilityHelper()
    mob.SetMobilityModel('ns3::ConstantPositionMobilityModel')
    mob.Install(enbNodes)
    pos = ListPositionAllocator()
    pos.Add((0.0, 0.0, 0.0))
    pos.Add((distance, 0.0, 0.0))
    mob.SetPositionAllocator(pos)
    mob.Install(ueNodes)

    # Install devices
    enbDevs = lte.InstallEnbDevice(enbNodes)
    ueDevs = lte.InstallUeDevice(ueNodes)

    # Install IP stack on UE
    InternetStackHelper().Install(ueNodes)
    pgw = epc.GetPgwNode()
    remote = NodeContainer()
    remote.Create(1)
    InternetStackHelper().Install(remote)

    # Point-to-point link to PGW
    p2p = ns.internet.PointToPointHelper()
    p2p.SetDeviceAttribute('DataRate', ns.core.StringValue('100Gbps'))
    p2p.SetChannelAttribute('Delay', ns.core.StringValue('10ms'))
    devs = p2p.Install(pgw, remote.Get(0))

    # Assign IPs
    addr = Ipv4AddressHelper()
    addr.SetBase('1.0.0.0', '255.0.0.0')
    internetIfaces = addr.Assign(devs)

    ueIfaces = epc.AssignUeIpv4Address(ueDevs)
    lte.Attach(ueDevs.Get(0), enbDevs.Get(0))

    # Routing for remote host
    staticHelper = Ipv4StaticRoutingHelper()
    remoteStatic = staticHelper.GetStaticRouting(remote.Get(0).GetObject(ns.network.Ipv4))
    remoteStatic.AddNetworkRouteTo(ns.network.Ipv4Address('7.0.0.0'),
                                  ns.network.Ipv4Mask('255.0.0.0'), 1)

    # Applications: UDP OnOff from remote -> UE
    port = 8000
    onoff = OnOffHelper('ns3::UdpSocketFactory',
                        ns.network.InetSocketAddress(ueIfaces.GetAddress(0), port))
    onoff.SetAttribute('DataRate', DataRateValue(ns.core.DataRate(int(dataRate * 1e6))))
    onoff.SetAttribute('PacketSize', UintegerValue(1024))
    client = onoff.Install(remote.Get(0))
    client.Start(Seconds(2.0))
    client.Stop(Seconds(20.0))

    sink = PacketSinkHelper('ns3::UdpSocketFactory',
                            ns.network.InetSocketAddress(ns.network.Ipv4Address.GetAny(), port))
    server = sink.Install(ueNodes.Get(0))
    server.Start(Seconds(1.0))
    server.Stop(Seconds(20.0))

    # FlowMonitor & NetAnim
    fm = FlowMonitorHelper()
    monitor = fm.InstallAll()
    anim = AnimationInterface('Lab4_LTE.xml')

    ns.core.Simulator.Stop(Seconds(21.0))
    ns.core.Simulator.Run()

    monitor.CheckForLostPackets()
    stats = monitor.GetFlowStats()
    thr = stats[1].rxBytes * 8.0 / 19.0
    print(f"LTE downlink throughput: {thr} bps")

    ns.core.Simulator.Destroy()

if __name__ == '__main__':
    main()
