# in_pin


# out_pin

data member: receivers_
its a map
`std::map<uint64_t, std::function<void(std::shared_ptr<const T>)>> receivers_;`

its a fan out list

One `out_pin` can feed **multiple** downstream filters. For example:

```
ground_vehicle_source.pose_out ──► kalman_filter.pose_in
                               ──► csv_sink.pose_in
                               ──► visualizer.pose_in
```

When `out_pin::write(sample)` is called, it loops over every entry in `receivers_` and calls each one — so all three get the sample at the same time. That's fan-out.

## Why a `map` and not a `vector`?

[pin.hpp:85](vscode-webview://0svc2hf70eo7g6g2st8hqcu3ld3av8b8fjsjir63digsba3s6i7r/src/pipeline/pin.hpp#L85) — `disconnect(handle)` needs to remove **one specific** receiver without disturbing the others. With a map, that's just `receivers_.erase(handle)` — O(log n), clean.

With a vector you'd have to find the entry first, which is messier.7

## The handle

Each call to `connect()` assigns the next integer handle (`next_handle_++`) as the map key and returns it to the caller. That handle is the only way to disconnect later:

```
connect → returns handle 1   (csv_sink wired)
connect → returns handle 2   (visualizer wired)

disconnect(1)  → removes csv_sink only, visualizer still connected
```

---

So in short: `receivers_` is the map of **"who is listening to me right now"**, keyed by handle so individual connections can be removed cleanly.

