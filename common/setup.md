# Setup

All labs have been developed and validated on **ns-3 version 3.40**. Compatibility with other releases is unverified.

---

## System Requirements

- **Operating system:**  
  - Ubuntu 22.04 LTS (or later)  
  - Fedora 35 (or later)  
  - macOS Sonoma  
  - Windows 10/11 with WSL 2  
- **Disk space:** ≥ 10 GB free  
- **RAM:** ≥ 4 GB (8 GB+ recommended)  

---

## Dependencies

### Ubuntu 22.04+

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential gcc g++ \
    python3 python3-dev python3-pip \
    autoconf automake libxmu-dev cvs git cmake p7zip-full \
    python3-matplotlib python3-tk \
    qtbase5-dev qttools5-dev-tools \
    gnuplot-x11 wireshark net-tools
````

### Fedora 35+

```bash
sudo dnf install -y \
    @development-tools gcc-c++ \
    python3 python3-devel python3-pip \
    autoconf automake libXmu-devel cvs git cmake p7zip \
    python3-matplotlib python3-tkinter \
    qt5-qtbase-devel qt5-qmake qt5-qttools-devel \
    gnuplot wireshark net-tools
```

### macOS Sonoma

```bash
brew install python@3 cmake qt wireshark gnuplot
pip3 install matplotlib pybindgen
```

---

## Windows 10/11 (WSL 2)

1. **Enable WSL & Virtual Machine Platform**
   Open PowerShell as Administrator and run:

   ```powershell
   dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart
   dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart
   Restart-Computer
   ```

2. **Set WSL2 as default**

   ```powershell
   wsl --set-default-version 2
   ```

3. **Install Ubuntu 22.04 LTS** from Microsoft Store. Launch and create UNIX user.

4. **Install Ubuntu dependencies**
   In the Ubuntu shell:

   ```bash
   sudo apt-get update
   sudo apt-get install -y \
       build-essential gcc g++ \
       python3 python3-dev python3-pip \
       autoconf automake libxmu-dev cvs git cmake p7zip-full \
       python3-matplotlib python3-tk \
       qtbase5-dev qttools5-dev-tools \
       gnuplot-x11 wireshark net-tools
   ```

5. **Enable GUI support (NetAnim)**

   * Install a Windows X server (e.g. VcXsrv, X410).
   * In Ubuntu:

     ```bash
     echo "export DISPLAY=$(grep -m1 nameserver /etc/resolv.conf | awk '{print $2}'):0" >> ~/.bashrc
     source ~/.bashrc
     ```

---

## Download & Build ns-3 3.40

1. **Download**

   ```bash
   cd ~
   wget https://www.nsnam.org/releases/ns-allinone-3.40.tar.bz2
   tar xjf ns-allinone-3.40.tar.bz2
   cd ns-allinone-3.40
   ```

2. **Build all components**

   ```bash
   ./build.py \
     --enable-examples \
     --enable-tests \
     --enable-python-bindings \
     --qmake-path /usr/lib/qt5/bin/qmake
   ```

3. **Configure & compile via CMake**

   ```bash
   cd ns-3.40
   cmake -S . -B build \
     -DNS3_BUILD_EXAMPLES=ON \
     -DNS3_BUILD_TESTS=ON \
     -DNS3_BUILD_PYTHON_BINDINGS=ON
   cmake --build build -j$(nproc)
   ```

4. **Verify C++ build**

   ```bash
   ./build/src/core/examples/hello-simulator
   # should print "Hello Simulator"
   ```

5. **Verify Python bindings**

   ```bash
   export PYTHONPATH=~/ns-allinone-3.40/ns-3.40/build/bindings:$PYTHONPATH
   python3 - <<'EOF'
   import ns.core
   print(ns.core.Simulator.Now())
   EOF
   # should output "0s"
   ```

---
