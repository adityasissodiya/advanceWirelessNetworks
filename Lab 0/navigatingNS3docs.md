**Exploring ns-3 Documentation**

Before writing any code, become familiar with **where** to find information:

1. **ns-3 Tutorial (HTML)**

   * **Link:** [https://www.nsnam.org/docs/release/3.40/tutorial/html/index.html](https://www.nsnam.org/docs/release/3.40/tutorial/html/index.html)
   * **Key Sections for Lab 0:**

     * *Chapter 1: Introduction*
     * *Chapter 2: Installing ns-3* (overview)
     * *Chapter 3: C++ Tutorial*
     * *Chapter 11: Python Tutorial* (starts on binding basics)

2. **C++ API Reference (Doxygen)**

   * **Link:** [https://www.nsnam.org/doxygen/group\_\_core.html](https://www.nsnam.org/doxygen/group__core.html)
   * Useful classes:

     * `ns3::Simulator`
     * `ns3::NodeContainer`
     * `ns3::PointToPointHelper`
     * etc.

3. **Python API Reference**

   * **Link:** [https://www.nsnam.org/doxygen/python/index.html](https://www.nsnam.org/doxygen/python/index.html)

4. **Module-Specific Documentation**

   * For any new module (e.g. wifi, internet, lte, flow-monitor), open:

     ```
     https://www.nsnam.org/docs/release/3.40/doxygen/<module>_html/index.html  
     ```
   * Example: WiFi propagation loss model reference:
     [https://www.nsnam.org/docs/release/3.40/doxygen/classns3\_1\_1FriisPropagationLossModel.html](https://www.nsnam.org/docs/release/3.40/doxygen/classns3_1_1FriisPropagationLossModel.html)

5. **General Troubleshooting**

   * Consult the **ns-3 FAQ and Issues** page:
     [https://www.nsnam.org/docs/release/3.40/faq/](https://www.nsnam.org/docs/release/3.40/faq/)
   * If build or runtime errors occur, search the **ns-3 Users’ Mailing List archives**:
     [http://mailman.isi.edu/pipermail/ns-users/](http://mailman.isi.edu/pipermail/ns-users/)

> **Tip:** Bookmark these documentation pages now. You’ll refer to them in every subsequent lab.

---

## **Part 5: Navigating ns-3 Documentation (Step-by-Step Guide)**

Whenever you need to find out **how to do X** (e.g., “Set up a point-to-point link” or “Use WiFi in ad-hoc mode”), follow this incremental path:

1. **Identify the module you need** (e.g., `point-to-point`, `wifi`, `lte`, `internet`).

2. **Open the Doxygen index for that module**:

   ```
   https://www.nsnam.org/docs/release/3.40/doxygen/group__<module>.html
   ```

   Example:

   * Point-to-Point: [https://www.nsnam.org/docs/release/3.40/doxygen/group\_\_point\_\_to\_\_point.html](https://www.nsnam.org/docs/release/3.40/doxygen/group__point__to__point.html)
   * WiFi: [https://www.nsnam.org/docs/release/3.40/doxygen/group\_\_wifi.html](https://www.nsnam.org/docs/release/3.40/doxygen/group__wifi.html)

3. **Find the Helper class** you need (e.g., `PointToPointHelper`, `YansWifiPhyHelper`).

4. **Open the Helper’s documentation page** to see its methods and attributes.

5. **Search for a complete example** in `examples/` or `tutorial/`.

   * `examples/point-to-point/first.cc` or `tutorial/` for usage patterns.

6. **For Python**:

   * Check the Python tutorial (Chapter 11) to see how the same Helper is exposed in ns-3’s Python API.
   * Use autocompletion (`ns.core.` + `Tab`) in a Python REPL to explore available classes.

> **Example**:
> You want to install the Internet Stack on nodes.
>
> 1. Module: `internet`.
> 2. Visit: [https://www.nsnam.org/docs/release/3.40/doxygen/group\_\_internet.html](https://www.nsnam.org/docs/release/3.40/doxygen/group__internet.html)
> 3. Find `InternetStackHelper`: [https://www.nsnam.org/docs/release/3.40/doxygen/classns3\_1\_1InternetStackHelper.html](https://www.nsnam.org/docs/release/3.40/doxygen/classns3_1_1InternetStackHelper.html)
> 4. Read “Install( )” method signature.
> 5. Check the C++ tutorial or `examples/internet/tcp-example.cc`.
> 6. For Python, check:
>
>    ```python
>    import ns.internet
>    dir(ns.internet.InternetStackHelper)
>    ```

---

## **Part 6: Incremental Troubleshooting Strategies**

Whenever you hit an error, follow these steps:

1. **Read the Error Carefully**

   * Identify which file and line number triggered it.
   * Determine if it’s a **compile-time error** (e.g., missing header, undefined symbol) or **runtime error** (Python import, simulation crash).

2. **Consult ns-3 Documentation**

   * If a C++ class is unknown, search the **Doxygen** for that class.
   * If a Python import fails, check your `PYTHONPATH` or rebuild Python bindings.

3. **Verify Module Inclusions**

   * For C++:

     ```cpp
     #include "ns3/ns3-module.h"
     ```

     brings in everything. Otherwise, explicitly include submodules (e.g., `"ns3/point-to-point-module.h"`).

   * For Python:

     ```python
     import ns.core
     import ns.network
     # etc.
     ```

     Make sure each is spelled correctly (case-sensitive).

4. **Check Build Logs** (for C++)

   * Run:

     ```bash
     ./waf configure --enable-examples --enable-tests --check-cxx-compiler
     ./waf build -v
     ```

     The `-v` flag shows more detailed output.

5. **Validate Environment Variables**

   * **C++**: Usually none needed beyond being in the ns-3 root when using `waf`.
   * **Python**:

     ```
     export PYTHONPATH=~/ns-3.40/build/bindings:$PYTHONPATH
     ```

     * If you open a new shell, re-export.
     * Confirm with:

       ```bash
       echo $PYTHONPATH
       ```

6. **Check Correct Version**

   * If tutorials reference ns-3.30 but you’re on 3.40, class names may differ. Always cross-check your installed version’s Doxygen (e.g., replace “3.30” with “3.40” in URLs).

7. **Ask for Help Early**

   * Best practice: If you spend more than **15 minutes** on a single error, stop and post a question on Discord.
   * When asking, provide:

     * The **exact error message**.
     * The **code snippet** that triggered it.
     * What you have tried.

---

## **Quick Reference Links**

* **ns-3 Main Site**: [https://www.nsnam.org](https://www.nsnam.org)
* **ns-3 Download & Release Notes**: [https://www.nsnam.org/releases](https://www.nsnam.org/releases)
* **C++ Tutorial (HTML)**: [https://www.nsnam.org/docs/release/3.40/tutorial/html/index.html](https://www.nsnam.org/docs/release/3.40/tutorial/html/index.html)
* **Python Tutorial**: [https://www.nsnam.org/docs/release/3.40/python/tutorial/python\_bindings.html](https://www.nsnam.org/docs/release/3.40/python/tutorial/python_bindings.html)
* **Doxygen (C++ API)**: [https://www.nsnam.org/docs/release/3.40/doxygen/index.html](https://www.nsnam.org/docs/release/3.40/doxygen/index.html)
* **Doxygen (Python API)**: [https://www.nsnam.org/docs/release/3.40/doxygen/python/index.html](https://www.nsnam.org/docs/release/3.40/doxygen/python/index.html)
* **NetAnim Documentation**: [https://www.nsnam.org/docs/release/3.40/netanim-guide.html](https://www.nsnam.org/docs/release/3.40/netanim-guide.html)
* **FAQ & Troubleshooting**: [https://www.nsnam.org/docs/release/3.40/faq/](https://www.nsnam.org/docs/release/3.40/faq/)