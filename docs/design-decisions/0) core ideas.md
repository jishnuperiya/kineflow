```
sample.hpp       — what flows through wires (pose_sample, dynamics_sample, etc.)
pin.hpp          — in_pin<T> / out_pin<T>  — the wire endpoints
filter.hpp       — abstract node: process(t, dt) called once per tick
filter_graph.hpp — static graph: wire once at startup, then run()
runtime_graph.hpp— live graph: add/remove filters and wires while running
```

**Key idea — pins and samples:**

- `out_pin<T>` holds a list of receivers (fan-out). You call `write(sample)` on it.
- `in_pin<T>` holds the latest sample. You call `read()` on it.
- `connect(out_pin, in_pin)` wires them — the out_pin calls the in_pin's `receive()` automatically on every `write()`.
- `runtime_graph` does the same thing but stores everything type-erased (`void*`) so it can connect pins whose types are only known at runtime (needed for the visual node editor in the UI).

### How a tick flows end-to-end

```
filter_graph::run(60s, 0.1s)
  └─ tick(t, dt)  — called 600 times
       ├─ ground_vehicle_source::process()
       │    ├─ model_.step(dt)          ← physics advances
       │    ├─ pose_out.write(pose)     → local_frame_filter::pose_in
       │    │                           → telemetry_observer::pose_in
       │    ├─ dynamics_out.write(dyn)  → low_pass_filter::dynamics_in
       │    └─ vehicle_out.write(veh)   → csv_sink_filter::vehicle_in
       │
       ├─ local_frame_filter::process()
       │    └─ local_pose_out.write()   → telemetry_observer::local_pose_in
       │
       ├─ low_pass_filter::process()
       │    └─ filtered_out.write()     → console_sink_filter::dynamics_in
       │
       └─ sinks::process()             ← read pins, write output
```

---

### The `runtime_graph` vs `filter_graph` distinction

- `filter_graph` — simple, wired once at startup in code. Used by the headless demo ([pipeline_demo/main.cpp](vscode-webview://0svc2hf70eo7g6g2st8hqcu3ld3av8b8fjsjir63digsba3s6i7r/src/pipeline_demo/main.cpp)).
- `runtime_graph` — wires/unwires at runtime via integer node IDs. Needed by the Qt visual node editor (UI layer) where the user can drag wires between nodes while the sim is running.


