
Update Todos

- [ ] 
    
    Teach pipeline/sample.hpp line by line (~15 min)
    
- [ ] 
    
    Teach pipeline/pin.hpp line by line (~20 min)
    
- [ ] 
    
    Teach pipeline/filter.hpp line by line (~10 min)
    
- [ ] 
    
    Teach pipeline/filter_graph.hpp line by line (~20 min)
    
- [ ] 
    
    Teach pipeline/runtime_graph.hpp line by line (~45 min)
    
- [ ] 
    
    Teach model/vehicle_state.hpp + .cpp line by line (~20 min)
    
- [ ] 
    
    Teach model/motion_model.hpp + .cpp line by line (~25 min)
    
- [ ] 
    
    Teach model/simulated_vehicle.hpp line by line (~15 min)
    
- [ ] 
    
    Teach sentinex/platform_model.hpp + ground_vehicle_model.hpp line by line (~25 min)
    
- [ ] 
    
    Teach sentinex/telemetry_sample + sinks line by line (~20 min)
    
- [ ] 
    
    Teach filters/ground_vehicle_source.hpp line by line (~15 min)
    
- [ ] 
    
    Teach filters/local_frame_filter.hpp line by line (~15 min)
    
- [ ] 
    
    Teach filters/low_pass_filter.hpp line by line (~15 min)
    
- [ ] 
    
    Teach filters/telemetry_observer + sink filters line by line (~15 min)
    
- [ ] 
    
    Teach Qt basics before UI files (~20 min)
    
- [ ] 
    
    Teach ui/main.cpp + main_window.hpp/.cpp line by line (~35 min)
    
- [ ] 
    
    Teach ui/node_models.hpp line by line (~30 min)
    
- [ ] 
    
    Teach ui/filter_factory.hpp line by line (~20 min)
    
- [ ] 
    
    Teach ui/plot_widget.hpp/.cpp line by line (~25 min)
    
- [ ] 
    
    Teach ui/palette_widget + sentinex_graphics_view line by line (~20 min)
    

Here's the full plan:

---

## Session 1 — Pipeline core (~50 min)

- ~~`sample.hpp`~~ ✓ done
- `pin.hpp` — 20 min
- `filter.hpp` — 10 min
- `filter_graph.hpp` — 20 min

## Session 2 — Advanced pipeline (~45 min)

- `runtime_graph.hpp` — 45 min _(hardest file in the codebase)_

## Session 3 — Model layer (~1h)

- `vehicle_state.hpp/.cpp` — 20 min
- `motion_model.hpp/.cpp` — 25 min
- `simulated_vehicle.hpp` — 15 min

## Session 4 — Sentinex domain (~45 min)

- `platform_model.hpp` + `ground_vehicle_model.hpp` — 25 min
- `telemetry_sample.hpp` + sinks — 20 min

## Session 5 — Filters (~1h)

- `ground_vehicle_source.hpp` — 15 min
- `local_frame_filter.hpp` — 15 min
- `low_pass_filter.hpp` — 15 min
- `telemetry_observer` + sink filters — 15 min

## Session 6 — UI (~2h 10min)

- Qt basics primer — 20 min
- `main.cpp` + `main_window` — 35 min
- `node_models.hpp` — 30 min
- `filter_factory.hpp` — 20 min
- `plot_widget` — 25 min
- `palette_widget` + `sentinex_graphics_view` — 20 min

---

**Total: ~6 hours across 6 sessions**

Ready to continue with `pin.hpp`?

should i learn all the 6 sessions today

Honestly — **no.**

6 hours of line-by-line C++ in one day won't stick. You'll finish exhausted and retain maybe 30% of it.

---

## What actually works for technical learning

Your brain consolidates new information during sleep and rest between sessions. Trying to compress it all into one day is like trying to fill a glass faster than it can absorb.

---

## Realistic schedule

|Day|Sessions|Focus|
|---|---|---|
|**Today**|1|`pin.hpp`, `filter.hpp`, `filter_graph.hpp` — the core framework|
|**Tomorrow**|2|`runtime_graph.hpp` alone — it needs fresh eyes|
|**Day 3**|3 + 4|Model layer + Sentinex domain|
|**Day 4**|5|All filters — fast now because patterns repeat|
|**Day 5**|6|UI layer|

**5 days, 1–1.5 hours per day.**

---

## Why this order matters

Each session builds on the previous one. By the time you hit the filters on Day 4, you'll already understand pins, samples, and the graph — so the filters will feel obvious rather than confusing.

If you try to do runtime_graph today right after pin.hpp your brain won't have time to settle the template concepts before hitting type erasure.

---

## One more thing

After each session — **close the code and try to explain it out loud** like you're in an interview. That's the real test of whether it stuck.