# Lab 02 Deliverables

Place all files in `Lab-02-WiFiPerformance/submission/`. Submit exactly one archive or pull request containing the following:

1. **Language choice**  
   - `choice.txt` containing exactly one line:  
     ```
     C++
     ```  
     or  
     ```
     Python
     ```

2. **Scenario 1 (Infrastructure WiFi)**  
   - `scenario1_results.csv` — columns: `rate_mbps,seed1_bps,seed2_bps,avg_bps`  
   - `scenario1_plot.png` — application throughput vs. PHY rate (averaged)  
   - `scenario1_anim.xml` — NetAnim XML for one PHY rate example  
   - `scenario1_screenshot.png` — screenshot of NetAnim showing topology & flows

3. **Scenario 2, Part 1 (Payload Sweep)**  
   - `payload_sweep_results.csv` — columns: `rate_mbps,payload_bytes,throughput_bps` for all 9 experiments  
   - `payload_sweep_plot.png` — throughput vs. payload size curves (one curve per PHY rate)

4. **Scenario 2, Part 2 (Hidden Terminal)**  
   - `hidden_off_results.csv` — columns: `flow_id,throughput_bps,pdr` with RTS/CTS disabled  
   - `hidden_on_results.csv`  — same columns with RTS/CTS enabled  
   - `hidden_comparison.png`   — bar chart or line plot comparing throughput/PDR off vs. on  
   - `hidden_off_anim.xml`     — NetAnim XML with RTS/CTS disabled  
   - `hidden_on_anim.xml`      — NetAnim XML with RTS/CTS enabled  
   - `hidden_off_screenshot.png` — screenshot of NetAnim (RTS off)  
   - `hidden_on_screenshot.png`  — screenshot of NetAnim (RTS on)

---

**File names must match exactly.**  
- CSV files require a header row.  
- All plots must have labeled axes and legends.  
- Anim XML and screenshots must correspond (off/on).  
- Do not include any additional files or build artifacts.  
---
