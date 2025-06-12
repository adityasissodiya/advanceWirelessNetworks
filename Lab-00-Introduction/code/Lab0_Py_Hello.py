#!/usr/bin/env python3
# Minimal "Hello, Simulator!" example using ns-3 Python bindings

import ns.core


def main():
    # Set nanosecond resolution for Time
    ns.core.Time.SetResolution(ns.core.Time.NS)

    # Schedule a print event at t=1.0s
    ns.core.Simulator.Schedule(
        ns.core.Seconds(1.0),
        lambda: print("Hello from Python after 1 second")
    )

    # Stop simulation at t=2.0s
    ns.core.Simulator.Stop(ns.core.Seconds(2.0))

    # Run simulation
    ns.core.Simulator.Run()
    ns.core.Simulator.Destroy()


if __name__ == '__main__':
    main()