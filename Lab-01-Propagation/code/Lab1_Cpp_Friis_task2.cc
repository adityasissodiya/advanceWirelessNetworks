// Lab 1: Friis (ns-3.40, IBSS 802.11g @ 6 Mbps on 2.4 GHz)
// Usage: ./ns3 run "scratch/Lab1_Cpp_Friis --distance=50"
// scratch/Lab1_FriisArgs.cc
#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/propagation-module.h"

using namespace ns3;

int main(int argc, char* argv[]) {
    // Defaults
    double txPowerDbm = 20.0;   // Transmit power (dBm)
    double frequency  = 2.4e9;  // Frequency (Hz)
    int dMin = 1;               // start distance (m)
    int dMax = 25;             // end distance (m)
    double step = 1.0;          // step size (m)

    CommandLine cmd;
    cmd.AddValue("txDbm", "Transmit power in dBm", txPowerDbm);
    cmd.AddValue("freq",  "Carrier frequency in Hz", frequency);
    cmd.AddValue("dMin",  "Minimum distance in meters", dMin);
    cmd.AddValue("dMax",  "Maximum distance in meters", dMax);
    cmd.AddValue("step",  "Distance step in meters", step);
    cmd.Parse(argc, argv);

    // Create Friis model
    Ptr<FriisPropagationLossModel> friis = CreateObject<FriisPropagationLossModel>();
    friis->SetAttribute("Frequency", DoubleValue(frequency));

    // Create nodes and mobility
    NodeContainer nodes;
    nodes.Create(2);
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    Ptr<MobilityModel> txMobility = nodes.Get(0)->GetObject<MobilityModel>();
    Ptr<MobilityModel> rxMobility = nodes.Get(1)->GetObject<MobilityModel>();

    txMobility->SetPosition(Vector(0.0, 0.0, 1.5));
    rxMobility->SetPosition(Vector(0.0, 0.0, 1.5));

    // Output header
    std::cout << "distance,path_loss_db\n";

    for (double d = dMin; d <= dMax; d += step) {
        rxMobility->SetPosition(Vector(d, 0.0, 1.5));
        double rxDbm = friis->CalcRxPower(txPowerDbm, txMobility, rxMobility);
        double plDb  = txPowerDbm - rxDbm;
        std::cout << d << "," << plDb << "\n";
    }
    return 0;
}
