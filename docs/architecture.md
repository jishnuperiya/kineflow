# SENTINEX Architecture

```mermaid
graph TD

  subgraph IOS ["IOS / Simulation Loop  (main.cpp)"]
    MAIN[main.cpp\nscenario setup · failure injection · sim loop]
  end

  subgraph PLATFORM ["sentinex::platform"]
    SP[scenario_params]
    FE[failure_event]
    PM["platform_model\n«abstract interface»"]
    GVM[ground_vehicle_model\nbicycle model backend]
    HM["helicopter_model\nJSBSim backend  «planned»"]
    NVM["naval_vessel_model\n«planned»"]
    PF["platform_factory\n«planned»"]
  end

  subgraph MODEL ["sentinex::model"]
    VS[vehicle_state\nx · y · psi · v]
    MC[motion_command\nsteering · velocity]
    MM[motion_model\nbicycle kinematics]
    SV[simulated_vehicle\nstateful wrapper]
  end

  subgraph TELEMETRY ["sentinex::telemetry"]
    TS[telemetry_sample\nuniversal data contract]
    SINK["telemetry_sink\n«abstract interface»"]
    CS[console_sink]
    CSVS[csv_sink]
    NS["network_sink  «planned»\nUDP to FlightGear"]
    XS["xml_sink  «planned»"]
  end

  subgraph ANALYSIS ["Analysis  «planned»"]
    QE["query_engine\nstd::ranges · lazy views"]
  end

  subgraph EXTERNAL ["External / Future"]
    JSB[JSBSim\nflight dynamics engine]
    FG[FlightGear\nvisualisation]
    CH["Chrono::Vehicle\nground vehicle dynamics"]
    UI["Qt IOS\ninstructor interface"]
  end

  %% IOS drives platform
  MAIN -->|initialise / step / inject| PM
  MAIN -->|creates via| PF
  SP -->|passed to| PM
  FE -->|injected into| PM

  %% Platform implementations
  PM --> GVM
  PM --> HM
  PM --> NVM
  PF -.->|creates| PM

  %% Physics layer feeds ground vehicle
  MC --> MM
  VS --> MM
  MM --> GVM
  MM --> SV

  %% Platform produces telemetry
  GVM -->|read| TS
  HM -->|read| TS

  %% Telemetry fans out to sinks
  TS --> SINK
  SINK --> CS
  SINK --> CSVS
  SINK --> NS
  SINK --> XS

  %% Analysis reads CSV
  CSVS -->|mission_log.csv| QE

  %% External connections
  JSB -->|FGFDMExec| HM
  CH -->|vehicle dynamics| GVM
  NS -->|UDP telemetry| FG
  QE -->|analysis results| UI
  MAIN -->|mission control| UI

  %% Styles
  classDef done        fill:#2d6a4f,color:#fff,stroke:#1b4332
  classDef planned     fill:#495057,color:#adb5bd,stroke:#6c757d,stroke-dasharray:5
  classDef external    fill:#1a3a5c,color:#90caf9,stroke:#1565c0
  classDef abstract    fill:#4a1942,color:#e0b0d5,stroke:#6a0f6a
  classDef ios         fill:#7b4000,color:#ffe0b2,stroke:#e65100

  class VS,MC,MM,SV,GVM,CSVS,CS,TS done
  class SP,FE,PM,SINK abstract
  class HM,NVM,PF,NS,XS,QE planned
  class JSB,FG,CH,UI external
  class MAIN ios
```

## Layer Summary

| Layer | Namespace | Status |
|---|---|---|
| Physics | `sentinex::model` | Done |
| Platform abstraction | `sentinex::platform` | Done (ground vehicle) |
| Telemetry pipeline | `sentinex::telemetry` | Done (console + CSV) |
| Analysis | — | Planned (query engine) |
| Visualisation | — | Planned (FlightGear) |
| IOS UI | — | Planned (Qt) |

## Data Flow

```
scenario_params
      │
      ▼
platform_model::initialise()
      │
      ▼  (each step)
platform_model::step(dt)
      │
      ▼
platform_model::read()
      │
      ▼
telemetry_sample
      │
      ├──▶ console_sink   →  terminal
      ├──▶ csv_sink       →  mission_log.csv  →  query_engine
      ├──▶ network_sink   →  FlightGear (UDP)
      └──▶ xml_sink       →  mission_log.xml
```
