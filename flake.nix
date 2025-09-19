{
  description = "ns-3 on macOS (M1/M2): Unified C++ (3.40) and Experimental try using Python (3.42) environments with NetAnim";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.05";
  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachSystem [ "aarch64-darwin" ] (system:
      let
        pkgs = import nixpkgs { inherit system; };
        line = "----------------------------------------------------------------------";

        commonNativeInputs = with pkgs; [
          cmake ninja pkg-config ccache
          python311 python311Packages.virtualenv
          curl gnused gnutar bzip2
        ];
        commonBuildInputs = with pkgs; [ libxml2 ];
        developerTools = with pkgs; [ lldb llvmPackages.clang-tools ];

        netanim = pkgs.stdenv.mkDerivation rec {
          pname = "netanim";
          version = "3.108";

          # Fetch the source code from the official URL.
          # The sha256 hash ensures the download is secure and reproducible.
          src = pkgs.fetchurl {
            url = "https://www.nsnam.org/tools/netanim/netanim-${version}.tar.bz2";
            sha256 = "sha256-kJoJ/XIRkpIyw60L/JCMNu3dec4UmjqgBcUMAKRXA7s=";
          };

          # NetAnim is a Qt application. This tells Nix that it needs Qt5 to build.
          # `qtbase` provides the necessary libraries and tools, including `qmake`.
          nativeBuildInputs = [ 
            pkgs.qt5.qtbase
            pkgs.qt5.wrapQtAppsHook
         ];

         # Runtime Dependency for an Icon Theme
         # `buildInputs` are dependencies needed by the application at runtime. Providing a standard icon theme
         buildInputs = [
            pkgs.gnome.adwaita-icon-theme
            pkgs.qt5.qtsvg
            pkgs.qt5.qtimageformats
          ];

          # These are the commands to build NetAnim, as per its instructions.
          # Nix will run these in an isolated environment.
          buildPhase = ''
            qmake netanim.pro
            make -j$NIX_BUILD_CORES
          '';

          # This tells Nix how to "install" the program. We copy the compiled `NetAnim` executable into the standard `bin` directory of the package.
          installPhase = ''
            mkdir -p $out/bin
            cp NetAnim $out/bin/
          '';

          # Argument for the Qt Wrapper.
          # This is where GUI applications look for icons.
          # Tells the `wrapQtAppsHook` to modify the final startup script to prepend the path to Adwaita icon theme's data files to the `XDG_DATA_DIRS` environment variable. 
          qtWrapperArgs = [
            "--prefix" "XDG_DATA_DIRS" ":"
            "${pkgs.gnome.adwaita-icon-theme}/share"
          ];
        };

        # --- Bootstrap Script for ns-3.40 (C++ only) ---
        bootstrapCpp = pkgs.writeShellApplication {
          name = "bootstrap-cpp";
          runtimeInputs = commonNativeInputs ++ commonBuildInputs;
          text = ''
            set -euo pipefail
            NS3_VER=3.40
            SRCDIR="ns-allinone-''${NS3_VER}"
            BUILDDIR="''${SRCDIR}/ns-''${NS3_VER}/build"

            echo "${line}"
            echo "Bootstrap ns-3.''${NS3_VER} (C++ only) in $(pwd)"
            echo "${line}"

            if [ -x "''${BUILDDIR}/ns3" ]; then
              echo "✅ Existing C++ build detected at ''${BUILDDIR} — nothing to do."
              exit 0
            fi

            if [ ! -d "''${SRCDIR}" ]; then
              echo "➡️ Downloading ns-3.40..."
              curl -fSLO "https://www.nsnam.org/release/ns-allinone-''${NS3_VER}.tar.bz2"
              echo "➡️ Extracting..."
              tar -xjf "ns-allinone-''${NS3_VER}.tar.bz2"
            fi

            echo "➡️ Configuring with CMake (Python Bindings OFF)..."
            mkdir -p "''${BUILDDIR}"
            cd "''${BUILDDIR}"

            cmake -G Ninja .. \
              -DNS3_ENABLE_EXAMPLES=ON \
              -DNS3_ENABLE_TESTS=ON \
              -DNS3_ENABLE_PYTHON_BINDINGS=OFF

            echo "➡️ Building with Ninja..."
            ninja -j"$(sysctl -n hw.ncpu)"

            echo
            echo "✅ Done. ns-3 executable is at ''${BUILDDIR}/ns3"
          '';
        };

        # --- Bootstrap Script for ns-3.42 (Python Enabled) ---
        bootstrapPy = pkgs.writeShellApplication {
          name = "bootstrap-py";
          runtimeInputs = commonNativeInputs ++ commonBuildInputs;
          text = ''
            set -euo pipefail
            NS3_VER="3.42"
            SRCDIR="ns-allinone-''${NS3_VER}"
            NSDIR="''${SRCDIR}/ns-''${NS3_VER}"
            BUILDDIR="''${NSDIR}/build"
            VENV=".venv_ns3_py"

            echo "${line}"
            echo "Bootstrap ns-3.''${NS3_VER} (Python enabled for M1) in $(pwd)"
            echo "${line}"

            if [ -x "''${BUILDDIR}/ns3" ] && [ -d "''${VENV}" ]; then
              echo "✅ Existing Python build & venv detected — nothing to do."
              exit 0
            fi

            if [ ! -d "''${SRCDIR}" ]; then
              echo "➡️ Downloading ns-3.42..."
              curl -fSLO "https://www.nsnam.org/release/ns-allinone-''${NS3_VER}.tar.bz2"
              tar -xjf "ns-allinone-''${NS3_VER}.tar.bz2"
            fi

            if [ ! -d "''${VENV}" ]; then
              echo "➡️ Creating Python virtual environment..."
              python3 -m venv "''${VENV}"
              "''${VENV}/bin/python" -m pip install --upgrade pip
              "''${VENV}/bin/python" -m pip install "cppyy==3.1.2"
            fi

            # This is the critical patch for Apple Silicon (M1/M2)
            # It comments out the check that disables python bindings on arm64 macOS.
            if grep -q 'if (APPLE AND CMAKE_SYSTEM_PROCESSOR MATCHES "?(arm64|aarch64)"?)' "''${NSDIR}/bindings/python/CMakeLists.txt"; then
              echo "➡️ Patching ns-3 build system for M1/M2..."
              sed -E -i.bak \
                's/if\s*\(APPLE\s+AND\s+CMAKE_SYSTEM_PROCESSOR MATCHES "?(arm64|aarch64)"?\)/if (FALSE)/' \
                "''${NSDIR}/bindings/python/CMakeLists.txt"
              rm -f "''${NSDIR}/bindings/python/CMakeLists.txt.bak"
            fi

            echo "➡️ Configuring with CMake (Python Bindings ON)..."
            mkdir -p "''${BUILDDIR}"
            cd "''${BUILDDIR}"

            cmake -G Ninja .. \
              -DNS3_ENABLE_EXAMPLES=ON \
              -DNS3_ENABLE_TESTS=ON \
              -DNS3_ENABLE_PYTHON_BINDINGS=ON

            echo "➡️ Building with Ninja (this may take a while)..."
            ninja -j"$(sysctl -n hw.ncpu)"

            echo
            echo "✅ Done. To use Python bindings, activate the venv and set PYTHONPATH."
          '';
        };
        
      in {
        # --- Dev Shells ---
        devShells.ns3-cpp = pkgs.mkShell {
          # ### ADDED: `netanim` is now included in our shell's build inputs. ###
          nativeBuildInputs = commonNativeInputs ++ developerTools ++ [ netanim ];
          buildInputs = commonBuildInputs;

          shellHook = ''
            export CC=clang
            export CXX=clang++

            # This section replicates the environment from the Dockerfile.
            # It sets paths based on the ns-3.40 directory structure.
            # The `if` statement ensures these are only set *after* you have run the bootstrap script and the directories actually exist.
            export NS3_VERSION="3.40"
            export NS3_ALLINONE_DIR="''${PWD}/ns-allinone-''${NS3_VERSION}"
            export NS3_DIR="''${NS3_ALLINONE_DIR}/ns-''${NS3_VERSION}"
            export WORK="''${PWD}/workspace" # Your custom work directory

            # Create the workspace directory if it doesn't exist
            mkdir -p $WORK

            echo "${line}"
            echo " ns-3.40 C++ Shell (NetAnim included)"
            echo "${line}"
            if [ ! -f "$NS3_DIR/build/ns3" ]; then
              echo "ℹ️ ns-3 C++ not built yet. Run the one-time bootstrap:"
              echo "   nix run .#bootstrap-cpp"
            else
              echo "✅ ns-3 C++ build found. Setting environment variables..."
              # The `${VAR:+:}` syntax robustly prepends to a path, avoiding a leading ":"
              export LD_LIBRARY_PATH="''${NS3_DIR}/build/lib''${LD_LIBRARY_PATH:+:}''${LD_LIBRARY_PATH:-}"
              export PATH="''${NS3_DIR}/build:''${NS3_DIR}:''${PATH}"

              echo "   WORK=''$WORK'"
              echo "   NS3_DIR=''$NS3_DIR'"
              echo
              echo "   Run simulations with:"
              echo "     ./ns-allinone-3.40/ns-3.40/build/ns3 run <...>"
              echo "   Launch the GUI with:"
              echo "     NetAnim"
            fi
            echo "${line}"
          '';
        };

        devShells.ns3-python = pkgs.mkShell {
          # ### ADDED: `netanim` is also available in the Python shell. ###
          nativeBuildInputs = commonNativeInputs ++ developerTools ++ [ netanim ];
          buildInputs = commonBuildInputs;
          shellHook = ''
            export CC=clang
            export CXX=clang++

            # Same logic as the C++ shell, but for the ns-3.42 Python environment.
            # Crucially, this section also sets the PYTHONPATH.
            export NS3_VERSION="3.42"
            export NS3_ALLINONE_DIR="''${PWD}/ns-allinone-''${NS3_VERSION}"
            export NS3_DIR="''${NS3_ALLINONE_DIR}/ns-''${NS3_VERSION}"
            export WORK="''${PWD}/workspace"

            mkdir -p $WORK

            echo "${line}"
            echo " ns-3.42 Experimental Python Shell (NetAnim included)"
            echo "${line}"
            if [ ! -d "$NS3_DIR" ]; then
              echo "ℹ️ ns-3 Python not built yet. Run the one-time bootstrap:"
              echo "   nix run .#bootstrap-py"
            else
              echo "✅ ns-3 Python build found. To use it:"
              echo "   1. source .venv_ns3_py/bin/activate"
              # Set the PYTHONPATH here, as it's specific to the Python workflow
              echo "   2. export PYTHONPATH=\"''${NS3_DIR}/build/bindings/python:\$PYTHONPATH\""
              echo "   3. python3 <your-script.py>"
              echo
              echo "   Launch the GUI with: NetAnim"
            fi
            echo "${line}"
          '';
        };

        devShells.default = self.devShells.${system}.ns3-cpp;

        # --- Apps ---
        apps.bootstrap-cpp = {
          type = "app";
          program = "${bootstrapCpp}/bin/bootstrap-cpp";
        };

        apps.bootstrap-py = {
          type = "app";
          program = "${bootstrapPy}/bin/bootstrap-py";
        };
        apps.default = self.apps.${system}.bootstrap-cpp;
      }
    );
}