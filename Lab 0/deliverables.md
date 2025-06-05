## **Part 7: Lab 0 Deliverables**

1. **A single PDF report** (named `Lab0_NS3_Report.pdf`) containing:

   * **Section 1: Installation Verification**

     * Screen captures (or pasted console logs) showing:

       * `./waf --run hello-simulator` output in C++.
       * `./waf --pyrun scratch/hello_py.py` output in Python.

   * **Section 2: Documentation Walkthrough**

     * A short paragraph (1–2 sentences each) describing:

       1. Where you found the C++ API pages for `Simulator`, `NodeContainer`, and `PointToPointHelper`. Provide links.
       2. Where you located the Python binding documentation (link to Python tutorial).

   * **Section 3: “Hello, Simulator!” Code Snippets**

     * Paste your final `hello-sim.cc` and `hello_py.py` code (with your modifications).
     * Under each snippet, bullet-point the key lines you added and why.

   * **Section 4: Visualization Screenshots**

     * Two screenshots (or embedded images) of **NetAnim** running:

       1. C++ version (`hello-animation.xml`) – show node positions and a packet animation frame.
       2. Python version (`hello_py_animation.xml`).
     * Briefly explain in 2–3 sentences how enabling `AnimationInterface` created that XML.

   * **Section 5: Troubleshooting Log**

     * For each “Likely Issue” from this lab (1–13), write:

       1. A one-sentence description of the cause.
       2. The exact command or code change you applied to fix it.

2. **Name your Python binding scripts** exactly:

   * `hello_py.py`
   * `hello_py_anim.py`

3. **Name your C++ scripts** exactly:

   * `hello-sim.cc`
   * `hello-sim-anim.cc`

4. **Submit all four scripts** along with the PDF in a single compressed archive named `Lab0_Submission.zip`.

---