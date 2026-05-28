# SENTINEX Architecture — Explained

## How SENTINEX maps to MIXR

MIXR is the reference architecture. SENTINEX follows the same structural thinking
but with three deliberate differences: pluggable physics, a multi-sink telemetry
pipeline, and a query engine layer.

---

## MIXR → SENTINEX Mapping

| MIXR Concept | SENTINEX Equivalent | Notes |
|---|---|---|
| Station | `main.cpp` + future Qt IOS | Controls & displays, scenario setup |
| Simulation (time, cycles, phases) | Simulation loop in `main.cpp` | `scenario_params` drives timing |
| Player | `platform_model` | One instance per simulated entity |
| Dynamics Model → JSBSim | Physics backends (pluggable) | Key difference — see below |
| Recorder | `telemetry_sink` pipeline | Key difference — see below |
| Interop (DIS / HLA) | `network_sink` (planned) | UDP stream to FlightGear / DIS |
| — | Query engine (planned) | No MIXR equivalent |

---

## Key Differences from MIXR

### 1. Pluggable Physics — not just JSBSim

MIXR's architecture diagram shows JSBSim as the dynamics backend.

SENTINEX makes the physics layer fully pluggable via `platform_model`.
Any backend can be slotted in — the simulation loop never changes:

```
platform_model  «abstract interface»
      │
      ├── ground_vehicle_model   ← kinematic bicycle model (no external deps)
      ├── helicopter_model       ← JSBSim Bo 105 / any JSBSim aircraft
      ├── fixed_wing_model       ← JSBSim Cessna, F-16, etc.
      ├── naval_vessel_model     ← ship kinematics
      └── chrono_vehicle_model   ← Chrono::Vehicle (full tyre dynamics)
```

The IOS selects the backend and aircraft model from the scenario XML:

```xml
<platform type="helicopter" model="bo105" />
<platform type="helicopter" model="f16"   />
<platform type="ground_vehicle"           />
```

Same simulation loop. Same telemetry output. Different physics behind the interface.

---

### 2. Multi-Sink Telemetry Pipeline

MIXR has a recorder module. SENTINEX replaces this with a sink pattern —
multiple destinations receive the same telemetry simultaneously:

```
telemetry_sample  (one per simulation step)
        │
        ├── console_sink    →  terminal (live monitoring)
        ├── csv_sink        →  mission_log.csv (post-mission analysis)
        ├── xml_sink        →  mission_log.xml (IOS report generation)
        └── network_sink    →  UDP stream → FlightGear / DIS network
```

Adding a new output destination means implementing one interface:

```cpp
class telemetry_sink {
  virtual void write(const telemetry_sample& s) = 0;
  virtual void flush() {}
};
```

The simulation loop and platform backends are completely unaffected.

---

### 3. Query Engine — no MIXR equivalent

MIXR has no built-in post-mission analysis layer.

SENTINEX adds a query engine built on C++23 `std::ranges` — lazy views
over the recorded telemetry log, evaluated only when iterated:

```cpp
// average speed during nominal operation
auto nominal_speed = log
  | std::views::filter([](const auto& s){ return s.active_failures.empty(); })
  | std::views::transform([](const auto& s){ return s.speed_ms; });

// first sample where engine failure became active
auto failure_onset = log
  | std::views::filter([](const auto& s){
      return std::ranges::contains(s.active_failures, std::string("engine"));
    })
  | std::views::take(1);
```

No intermediate copies. No manual loops. The IOS can ask questions like:
- "What was the average speed before and after the engine failure?"
- "At what time did the vehicle reach waypoint 3?"
- "How many seconds did the platform operate under degraded conditions?"

---

## Full Architecture

```
┌─────────────────────────────────────────────────────┐
│  IOS  (Instructor Operating Station)                │
│  scenario XML · failure injection · mission control │
│  future: Qt display · controls · OTW               │
└──────────────────────┬──────────────────────────────┘
                       │  scenario_params · failure_event
                       ▼
┌─────────────────────────────────────────────────────┐
│  Platform Layer   sentinex::platform                │
│                                                     │
│  platform_model  «abstract»                         │
│    ├── ground_vehicle_model  (bicycle kinematics)   │
│    ├── helicopter_model      (JSBSim)               │
│    ├── fixed_wing_model      (JSBSim)               │
│    └── naval_vessel_model    (ship kinematics)      │
└──────────────────────┬──────────────────────────────┘
                       │  telemetry_sample (per step)
                       ▼
┌─────────────────────────────────────────────────────┐
│  Telemetry Pipeline   sentinex::telemetry           │
│                                                     │
│  telemetry_sink  «abstract»                         │
│    ├── console_sink   →  terminal                   │
│    ├── csv_sink       →  mission_log.csv            │
│    ├── xml_sink       →  mission_log.xml            │
│    └── network_sink   →  FlightGear / DIS           │
└──────────────────────┬──────────────────────────────┘
                       │  mission_log.csv
                       ▼
┌─────────────────────────────────────────────────────┐
│  Query Engine   std::ranges · lazy views            │
│                                                     │
│  filter · transform · fold_left                     │
│  "avg speed before failure"                         │
│  "time to waypoint"                                 │
│  "duration under degraded conditions"               │
└─────────────────────────────────────────────────────┘
```

---

## Physics Layer (sentinex::model)

Independent of everything above. No knowledge of telemetry, IOS, or sinks.

```
vehicle_state  [x, y, psi, v]
motion_command [steering, velocity]
      │
      ▼
motion_model::propagate()
      │
      ▼
new vehicle_state
```

Ground vehicle backend wraps this directly.
Helicopter backend replaces it with JSBSim's `FGFDMExec`.
