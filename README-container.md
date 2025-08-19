# ns-3.40 Frozen Environment (Container + Devcontainer + CI)

This kit pins the course to **ns-3.40** and gives students a one-command environment.

## Quick Start (Docker)

```bash
# From the repo root (where this Dockerfile lives)
make docker-build
make shell      # drops you into a shell with ns-3.40 ready
# inside the container:
source scripts/setup_env.sh   # sets PYTHONPATH/LD_LIBRARY_PATH
python3 -c "import ns.core; print('ns-3 OK')"

# Run the Lab-00 Python script if present
make lab0

# Run smoke checks (CI-equivalent)
make check
```

## VS Code Dev Container

1. Install the "Dev Containers" extension.
2. Open the repo folder in VS Code.
3. Run **Reopen in Container**. The container builds ns-3.40 and mounts your repo at `/work`.

## CI

GitHub Actions workflow: `.github/workflows/ns3-3.40-ci.yml`  
It builds the container and runs `scripts/ci_smoke.sh` inside it.

## Notes

- **GUI tools (NetAnim):** optional; set `--build-arg BUILD_NETANIM=true` on `docker build`. Using NetAnim inside a container requires X forwarding or copying generated XML out to the host and opening it natively.
- **Python bindings:** Exposed via `PYTHONPATH=$NS3_DIR/build/bindings/python`. If you change the ns-3 build directory, update `scripts/setup_env.sh` accordingly.
- **Version guard:** `scripts/ns3-check.sh` confirms you’re actually on ns-3.40.