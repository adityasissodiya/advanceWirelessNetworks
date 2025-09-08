

# Lab 3 Deliverables

## Part 1 – Multi-Hop UDP

1. **Simulation results**

   * `udp_chain_results.csv` — columns:

     ```
     num_nodes,pkt_size,seed1_bps,seed2_bps,avg_bps
     ```

     covering all hop counts {3, 4, 5, 6} and payload sizes {300B, 700B, 1200B}.

2. **Plots & animations**

   * `udp_chain_plot.png` — throughput vs. hop count for a fixed packet size (e.g., 1200B).
   * `udp_chain_anim.xml` — NetAnim XML for one representative run.
   * `udp_chain_screenshot.png` — screenshot of the NetAnim animation.

---

## Part 2 – Payload Sweep

1. **Simulation results**

   * `payload_sweep_results.csv` — columns:

     ```
     num_nodes,pkt_size,throughput_bps
     ```

     for all combinations of hop counts {3, 4, 5, 6} and payload sizes {300B, 700B, 1200B}.

2. **Plots**

   * `payload_sweep_plots/` — one PNG per hop count (e.g., `hop3.png`, `hop4.png`, etc.), showing throughput vs. packet size.

---

## Part 3 – TCP vs. UDP

1. **Simulation results**

   * `tcp_udp_comparison.csv` — columns:

     ```
     protocol,pkt_size,seed1_bps,seed2_bps,avg_bps
     ```

     for `protocol ∈ {TCP, UDP}` and packet sizes {300B, 1200B} with 3 stations.

2. **Plot**

   * `tcp_udp_comparison_plot.png` — bar or line plot comparing TCP vs. UDP throughputs.

---

## Part 4 – Discussion & Theory

1. **Analysis text files**

   * `throughput_vs_nominal.txt` — short discussion comparing the nominal PHY bitrate (1 Mbps) with observed application-layer throughput.
   * `best_packet_size.txt` — reflection on how to theoretically determine the best packet size for minimizing transmission time of 1 GB data in multi-hop ad hoc.

---

## File Naming Summary

* `udp_chain_results.csv`, `udp_chain_plot.png`, `udp_chain_anim.xml`, `udp_chain_screenshot.png`
* `payload_sweep_results.csv`, `payload_sweep_plots/hop3.png`, `payload_sweep_plots/hop4.png`, etc.
* `tcp_udp_comparison.csv`, `tcp_udp_comparison_plot.png`
* `throughput_vs_nominal.txt`, `best_packet_size.txt`

---

**All CSVs must have header rows. All plots must include axis labels and legends. File names must match exactly.**
