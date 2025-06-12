# Lab 01 Deliverables

Place all files in `Lab-01-Propagation/submission/`. Submit exactly one archive or pull request with the following:

1. **Language choice**  
   - `choice.txt` containing exactly one line:  
     ```
     C++
     ```  
     or  
     ```
     Python
     ```

2. **Simulated propagation data & plots**  
   For each loss model (Two-Ray, Cost231-Hata, Friis):  
   - `<model>_results.csv` — two columns `distance,bitrate` with your measured data.  
     - e.g. `two_ray_results.csv`  
   - `<model>_plot.png` — plot of bitrate vs. distance.  
     - e.g. `two_ray_plot.png`

3. **Real-world path-loss comparison**  
   - `measured_pathloss.csv` — two columns `distance,path_loss_db` from your RSSI measurements.  
   - `friis_pathloss.csv` — two columns `distance,path_loss_db` computed via the Friis formula.  
   - `pathloss_comparison.png` — combined plot of measured vs. Friis path-loss.

---

**File names must match exactly.**  
Do not include any extra files or build artifacts.  
Ensure all CSV files have headers and all plots are clearly labeled (axes titles and legend).  

---
