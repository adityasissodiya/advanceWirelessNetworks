````markdown
# Lab 02 – Wi-Fi Performance Factors

This lab explores how different factors impact Wi-Fi network performance. You will evaluate how PHY data rate, packet size, and hidden terminals affect throughput and reliability in an infrastructure Wi-Fi setup.

## Learning Goals
- Understand the relationship between PHY data rate and application throughput.
- Measure the effect of packet payload size on throughput.
- Explore the hidden terminal problem and observe how RTS/CTS improves fairness.
- Gain experience using FlowMonitor and NetAnim in performance evaluation.

## Provided Files
- **docs/**
  - `Lab-02-Instructions.md` – step-by-step instructions.
  - `deliverables.md` – list of required submission files.
- **code/**
  - `Lab2_Cpp_Scenario1.cc` – Scenario 1: one AP, one STA.
  - `Lab2_Cpp_Scenario2.cc` – Scenario 2: payload sweep & hidden terminals.
  - `Lab2_Py_Scenario1.py` – Python equivalent of Scenario 1.
  - `Lab2_Py_Scenario2.py` – Python equivalent of Scenario 2.

````
## Running the Code

### Scenario 1 – Rate vs Throughput
- **C++**:
  ```bash
  cp Lab-02-WiFiPerformance/code/Lab2_Cpp_Scenario1.cc ~/ns-allinone-3.40/ns-3.40/scratch/
  cd ~/ns-allinone-3.40/ns-3.40
  ./ns3 build
  ./ns3 run scratch/Lab2_Cpp_Scenario1 --rate=11 --seed=1


* **Python**:

  ```bash
  python3 Lab-02-WiFiPerformance/code/Lab2_Py_Scenario1.py --rate=11 --seed=1
  ```

Repeat for multiple data rates and seeds as specified. Log the results into a CSV.

### Scenario 2 – Payload Sweep & Hidden Terminals

* **C++**:

  ```bash
  ./ns3 run scratch/Lab2_Cpp_Scenario2 --payload=500 --enableRtsCts=false
  ```
* **Python**:

  ```bash
  python3 Lab-02-WiFiPerformance/code/Lab2_Py_Scenario2.py --payload=500 --enableRtsCts=false
  ```

Vary payload sizes and toggle RTS/CTS (`--enableRtsCts=true/false`) to collect all required data.

---

## Data Collection

* **Scenario 1 (Rate Sweep)**:

  * Run at several PHY rates with at least two seeds.
  * Save results in `scenario1_results.csv`.
  * Plot throughput vs PHY rate: `scenario1_plot.png`.
  * Save NetAnim output: `scenario1_anim.xml`, `scenario1_screenshot.png`.

* **Scenario 2 Part 1 (Payload Sweep)**:

  * Test multiple payload sizes at different rates.
  * Save combined results in `payload_sweep_results.csv`.
  * Plot throughput vs payload for each rate: `payload_sweep_plot.png`.

* **Scenario 2 Part 2 (Hidden Terminals)**:

  * Run with RTS/CTS disabled and enabled.
  * Save FlowMonitor results:

    * `hidden_off_results.csv`
    * `hidden_on_results.csv`
  * Plot comparison: `hidden_comparison.png`.
  * Save NetAnim outputs and screenshots:

    * `hidden_off_anim.xml`, `hidden_on_anim.xml`
    * `hidden_off_screenshot.png`, `hidden_on_screenshot.png`

---

## Deliverables Checklist

(see `docs/deliverables.md` for details)

* `choice.txt` – language used (`C++` or `Python`).
* Scenario 1: `scenario1_results.csv`, `scenario1_plot.png`, anim XML + screenshot.
* Scenario 2 Part 1: `payload_sweep_results.csv`, `payload_sweep_plot.png`.
* Scenario 2 Part 2: hidden terminal CSVs, plot, anim XMLs, screenshots.

---

## Common Pitfalls

* **RTS/CTS not applied:** Set `RtsCtsThreshold=0` *before* installing Wi-Fi devices【troubleshooting note】, or the toggle will have no effect.
* **FlowMonitor placement:** Must be installed before starting traffic apps, otherwise throughput will log as zero.
* **Rate string typos:** Use exact Wi-Fi mode names (e.g., `DsssRate5_5Mbps` not `DsssRate5.5Mbps`).
* **Unlabeled plots:** Every figure must have axis labels and a legend, or points will be deducted.

---
