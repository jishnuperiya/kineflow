# Kineflow

<p align="center">
  <img src="docs/logo.svg" alt="Kineflow" width="400"/>
</p>

<p align="center">
  <strong>Open-source, lightweight C++ middleware for real-time data pipelines.</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/license-Apache%202.0-blue.svg" alt="License"/>
  <img src="https://img.shields.io/badge/C%2B%2B-23-blue.svg" alt="C++23"/>
  <img src="https://img.shields.io/badge/status-early%20development-orange.svg" alt="Status"/>
</p>

---

## What is Kineflow?

Kineflow is an open-source C++23 pipeline framework inspired by [ADTF](https://www.digitalwerk.net/adtf/) — the Automotive Data and Time-Triggered Framework used by Bosch, Audi, and BMW. Unlike ADTF, Kineflow is free, open-source, and not limited to automotive use cases.

Kineflow lets you build data pipelines by connecting **filters** — processing nodes that read from input pins, transform data, and write to output pins. Pipelines are defined in **JSON configuration files** and executed by a runtime engine. Filters can be loaded as plugins at runtime, with no recompilation needed.

**Use cases:**
- Automotive sensor data processing (LiDAR, camera, CAN, radar)
- Robotics data pipelines (ROS2-style, without ROS2)
- Signal processing and measurement tools
- Simulation and ADAS evaluation frameworks
- Any domain requiring real-time typed data pipelines

---

## Why Kineflow?

| Feature | ADTF | Kineflow |
|---|---|---|
| License | Commercial (expensive) | Apache 2.0 (free) |
| Language | C++ | C++23 |
| Config format | Proprietary | JSON (formally specified) |
| Plugin system | Yes | Yes (planned) |
| Visual editor | ADTF Studio | Qt visual editor (planned) |
| Domain | Automotive only | Generic |
| Open source | No | Yes |

---

## Architecture

Kineflow is built around four base classes:

```
filter      →  a processing node with input and output pins
pin         →  a typed connection point on a filter
data_type   →  describes what kind of data flows through a pin
attribute   →  a configurable parameter of a filter
```

Pipelines are defined in JSON files. The runtime engine reads the JSON, validates it against a formal grammar, instantiates filters via a factory pattern, connects their pins, and ticks them in order.

```
JSON file
    ↓
parse_pipeline()    →  validates against formal grammar
    ↓
make_pipeline()     →  creates C++ filter objects via factory
    ↓
executor_engine     →  ticks filters in order
    ↓
Output
```

The edit-time representation (JSON/Qt UI) and the runtime representation (C++ objects) are completely separate. This means:
- The visual editor does not need to know how filters work internally
- The execution engine does not need to know anything about the UI
- Both are driven from the same simple JSON specification

---

## Pipeline Configuration

Pipelines are defined in JSON files. The format is formally specified by a context-free grammar — meaning the parser can validate not just that the JSON is well-formed, but that it describes a valid Kineflow pipeline.

See [docs/pipeline_grammar.md](docs/pipeline_grammar.md) for the complete specification.

### Example — calculator pipeline

```json
{
  "filters": [
    {
      "id": 0,
      "type": "number_source",
      "name": "my_source",
      "properties": [
        {"name": "value", "type": "int", "values": ["20"]}
      ],
      "pins": [
        {"id": 0, "name": "output", "direction": "out", "type": "int"}
      ]
    },
    {
      "id": 1,
      "type": "multiply",
      "name": "my_multiplier",
      "properties": [
        {"name": "factor", "type": "int", "values": ["5"]}
      ],
      "pins": [
        {"id": 0, "name": "input",  "direction": "in",  "type": "int"},
        {"id": 1, "name": "output", "direction": "out", "type": "int"}
      ]
    },
    {
      "id": 2,
      "type": "console_printer",
      "name": "my_printer",
      "properties": [],
      "pins": [
        {"id": 0, "name": "input", "direction": "in", "type": "int"}
      ]
    }
  ],
  "connections": [
    {
      "source": {"filter": 0, "pin": 0},
      "target": {"filter": 1, "pin": 0}
    },
    {
      "source": {"filter": 1, "pin": 1},
      "target": {"filter": 2, "pin": 0}
    }
  ]
}
```

This pipeline computes `20 × 5 = 100` and prints the result.

---

## Formally Specified Config Language

Kineflow's JSON pipeline format is defined by a **context-free grammar (CFG)** — the same formalism used to specify programming languages like C++ and JSON itself.

This means:
- The parser knows exactly what a valid pipeline looks like
- Invalid pipelines are rejected with descriptive error messages
- The grammar serves as the authoritative specification for contributors
- Tools like visual editors and code generators can be built against the spec

```
pipeline    ::=  { "filters": [filter], "connections": [connection] }
filter      ::=  { "id": natural, "type": filter-type, "name": name,
                   "properties": [property], "pins": [pin] }
pin         ::=  { "id": natural, "name": name,
                   "direction": direction, "type": data-type }
connection  ::=  { "source": filter-pin, "target": filter-pin }
filter-pin  ::=  { "filter": natural, "pin": natural }
property    ::=  { "name": name, "type": data-type, "values": [value] }
```

See [docs/pipeline_grammar.md](docs/pipeline_grammar.md) for the complete grammar and validation rules.

---

## Writing a Filter

Every filter inherits from the `filter` base class and implements `process()`:

```cpp
class multiply_filter : public kineflow::pipeline::filter
{
public:
    multiply_filter() = default;

    void set_property(const std::string& name, const std::string& value) override
    {
        if (name == "factor")
            factor_ = std::stoi(value);
    }

    void process(double timestamp_sec, double dt_sec) override
    {
        if (input_.has_data()) {
            auto val = input_.read()->value * factor_;
            output_.write(std::make_shared<int_sample>(val));
        }
    }

    pin input_  {"input",  direction::in,  "int"};
    pin output_ {"output", direction::out, "int"};

private:
    int factor_ = 1;
};
```

Register it in the factory:
```cpp
filter_registry["multiply"] = []{ return new multiply_filter(); };
```

Now it can be used in any JSON pipeline file — no recompilation needed.

---

## Plugin System (Planned)

Filters will be loadable as shared libraries at runtime:

```cpp
// In a .so / .dll plugin
extern "C" filter* create_filter() {
    return new my_custom_filter();
}
```

The factory map will look up plugins by name and load them dynamically.

---

## Prerequisites

### Linux / WSL

```bash
sudo apt install cmake g++ ninja-build
```

### macOS

```bash
sudo port install clang-20 cmake doxygen
```

---

## Building

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

---

## Running

Run the calculator pipeline example:

```bash
./kineflow examples/calculator_pipeline.json
```

---

## Project Status

```
✓ Core pipeline framework     →  filter, pin, filter_graph
✓ Runtime dynamic wiring      →  runtime_graph
✓ JSON pipeline format        →  formally specified with CFG
  JSON parser                 →  in progress
  Filter instantiation        →  planned
  Plugin system               →  planned
  Qt visual editor            →  planned
  Threading                   →  planned
```

---

## Contributing

Contributions welcome. The easiest way to contribute is to write a new filter:

1. Inherit from `filter` base class
2. Implement `process()` and `set_property()`
3. Declare pins as public members
4. Register in the factory map
5. Add a JSON example pipeline

See [docs/pipeline_grammar.md](docs/pipeline_grammar.md) for the pipeline config specification.

---

## Contributors

- jishnuperiya@gmail.com
