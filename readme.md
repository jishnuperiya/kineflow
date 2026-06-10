# Kineflow

<p align="center">
  <img src="docs/logo.svg" alt="Kineflow" width="400"/>
</p>

<p align="center">
  <strong>Learning and building an Open-source, lightweight middleware for real-time data pipelines.</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/license-Apache%202.0-blue.svg" alt="License"/>
  <img src="https://img.shields.io/badge/C%2B%2B-20-blue.svg" alt="C++20"/>
  <img src="https://img.shields.io/badge/status-early%20development-orange.svg" alt="Status"/>
</p>

> **⚠️ Notice:** Sentinex is being redesigned and rebranded as **Kineflow** — an open-source, lightweight C++20 alternative to ADTF for real-time data pipelines in automotive and robotics. This repository will be archived once Kineflow reaches its first stable release. Follow the progress at [github.com/jishnuperiya/kineflow](https://github.com/jishnuperiya/kineflow).

---

## Prerequisites

### macOS

```bash
sudo port install clang-20 cmake doxygen
```

---

## Building

```bash
mkdir .bld
cd .bld
cmake ..
make -j
```

Also:

```bash
make help
```

to see a list of possible build targets.

---

## Running

Run the test and experimentation harness:

```bash
./sentinex-test
```

Run the reference CLI executable:

```bash
./sentinex-cli
```

---

## Contributors

- jishnuperiya@gmail.com
