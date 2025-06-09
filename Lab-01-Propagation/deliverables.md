## **Lab 1 Deliverables**

Submit a **single PDF report** named `Lab1_NS3_Report.pdf` containing:

1. **Section 1: Overview & Documentation Guidance**

   * List which Doxygen pages you used to configure Two-Ray, Cost231, and Friis (provide links).
   * Briefly describe each model’s theoretical difference (1–2 sentences).

2. **Section 2: Two-Ray Empirical Range (*dᵢ*)**

   * Show the **sweep results** table or console logs for your distances × rxPackets (C++ and/or Python).
   * State *dᵢ* (e.g., “No packets at 80 m; last nonzero at 70 m, so *dᵢ* = 70 m”).

3. **Section 3: Throughput vs. Distance for Three Models**

   * Three sub‐sections: *Two-Ray*, *Cost231-Hata*, *Friis*.

     * For each, include a table of distance vs. throughput.
     * Include a short discussion: e.g.,

       > “Two-Ray: at 70 m, throughput = 5 Mbps; at 9 m, throughput = 8.5 Mbps.”
   * Combine all three results in a single **chart** (PNG) labeled “Throughput vs. Distance (Lab 1)”.

4. **Section 4: NetAnim Screenshots** (2 images)

   * Screenshot of **Two-Ray simulation** at a mid‐range distance (e.g., 50 m) showing nodes and packet arrows.
   * Screenshot of **Cost231** (or Friis) similarly.
   * Caption each (include timestamp, model used, distance).

5. **Section 5: (Optional) Real‐World Measurement**

   * If you performed the corridor test:

     * Table of *distance (m)* vs. *RSSI (dBm)* vs. *measured path loss (dB)*.
     * Chart comparing measured path loss vs. Friis model.
     * Brief analysis of why Friis deviates (e.g. multipath, walls, hardware variation).

6. **Section 6: Troubleshooting Log**

   * For each “Likely Issue” (#1–#15) encountered, list:

     1. **Error message** or symptom.
     2. **Cause** (one line).
     3. **Fix** (code snippet or command).

7. **Appendix: Source Code Listings**

   * Include final versions of all scripts you used:

     * `Lab1_Cpp_Base.cc`
     * `Lab1_Cpp_TwoRay.cc`
     * `Lab1_Cpp_Sweep.cc`
     * `Lab1_Cpp_TwoRay_Results.cc`
     * `Lab1_Cpp_Cost231_Results.cc`
     * `Lab1_Cpp_Friis_Results.cc`
     * `Lab1_Py_Base.py`
     * `Lab1_Py_TwoRay.py`
     * `Lab1_Py_Sweep.py`
     * `Lab1_Py_TwoRay_Results.py`
     * `Lab1_Py_Cost231_Results.py`
     * `Lab1_Py_Friis_Results.py`

> **Submission:**
>
> * Package your PDF + all scripts in a single ZIP named `Lab1_Submission.zip`.
> * Ensure your code is **well‐commented**—highlight where you set the propagation model, distances, and how you measure throughput.
> * Include your **NetAnim XML files** (`lab1-base-animation.xml`, etc.) if you want the TA to inspect them, but optional.

---

### **Congratulations, you have completed Lab 1!**

You now understand how three key propagation models affect WiFi throughput, how to measure throughput in ns-3, and how to guide yourself through the ns-3 documentation. In **Lab 2**, we will extend this by exploring **WiFi network performance** in slightly larger topologies and diving deeper into WiFi parameters. Good luck!
