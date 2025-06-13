# Lab 03 Deliverables

Place all files in `Lab-03-Adhoc/submission/`. Submit one archive or pull request containing:

1. **Language choice**  
   - `choice.txt` with exactly one line:  
     ```
     C++
     ```  
     or  
     ```
     Python
     ```

2. **Part 1 – Multi-Hop UDP**  
   - `udp_chain_results.csv` – columns:  
     ```
     num_nodes,pkt_size,seed1_bps,seed2_bps,avg_bps
     ```  
   - `udp_chain_plot.png` – throughput vs. hop count for a fixed packet size (e.g., 500 B).  
   - `udp_chain_anim.xml` – NetAnim XML for one example run.  
   - `udp_chain_screenshot.png` – screenshot of NetAnim animation.

3. **Part 2 – Payload Sweep**  
   - `payload_sweep_results.csv` – columns:  
     ```
     num_nodes,pkt_size,throughput_bps
     ```  
     covering all combinations of hops and packet sizes.  
   - `payload_sweep_plots/` – one PNG per hop count showing throughput vs. packet size (e.g., `hop3.png`, `hop4.png`, etc.).

4. **Part 3 – TCP vs. UDP**  
   - `tcp_udp_comparison.csv` – columns:  
     ```
     protocol,pkt_size,seed1_bps,seed2_bps,avg_bps
     ```  
     for `protocol ∈ {TCP,UDP}` and packet sizes 300 B & 1200 B.  
   - `tcp_udp_comparison_plot.png` – bar or line plot comparing TCP vs. UDP throughputs.

5. **Part 4 – Hidden Terminal**  
   - `hidden_off.csv` – columns:  
     ```
     flow_id,throughput_bps,pdr
     ```  
     with RTS/CTS disabled.  
   - `hidden_on.csv` – same columns with RTS/CTS enabled.  
   - `hidden_comparison_plot.png` – comparative plot of throughput & PDR off vs. on.  
   - `hidden_off_anim.xml` & `hidden_on_anim.xml` – NetAnim XMLs.  
   - `hidden_off_screenshot.png` & `hidden_on_screenshot.png` – NetAnim screenshots.

---

**File names must match exactly.**  
- All CSVs require a header row.  
- Plots must include axis labels and legends.  
- XML and screenshots must correspond (off/on).  
- Do not include extra files or build artifacts.  

---
