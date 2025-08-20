# ns-3.40 Frozen Environment (Container + Devcontainer + CI)

## Docker

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

## Notes

- **GUI tools (NetAnim):** optional for docker container, off by default; set `--build-arg BUILD_NETANIM=true` on `docker build`. Using NetAnim inside a container requires X forwarding or copying generated XML out to the host and opening it natively. It's a hassle and it's better to use NetAnim natively.
- **Python bindings:** Exposed via `PYTHONPATH=$NS3_DIR/build/bindings/python`. If you change the ns-3 build directory, update `scripts/setup_env.sh` accordingly.
- **Version guard:** `scripts/ns3-check.sh` confirms you’re actually on ns-3.40.