# Sentinex | State Estimation & Sensor Simulation Lab

real-time system monitoring and data visualization toolkit with Qt6. It monitors CPU, memory, network, and disk I/O with a responsive multi-plot dashboard. The architecture separates data collection from visualization, making it easy to add new metrics or extend to other domains.

Generated documentation:
https://jishnuperiya.github.io/sentinex/

## Prerequisites

### macOS

```bash
sudo port install clang-20 cmake doxygen
```
## Building

    mkdir .bld
    cd .bld
    cmake ..
    make -j

Also:

    make help

to see a list of possible of build targets.

## Running

Run the test and experimentation harness:

    ./sentinex-test

Run the reference CLI executable:

    ./sentinex-cli


# Contributors

- jishnuperiya@gmail.com
- jonathon.bell@gmail.com
