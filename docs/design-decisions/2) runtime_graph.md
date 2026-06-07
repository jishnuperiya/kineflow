**what is runtime graph in one sentence?**

its a filter graph where the user decide what filters exist and how they are connect3d at runtime, not in code

in filter_graph.cpp, you write this in code:

```cpp
auto& src = graph.add(std::make_unique<ground_vehiucle_source>(params));
auto& sink =graph.add(std::make_unique<csv_sink>);
graph.connect(src.pose_out, sink.pose_in);
```

all compile time resolved.

in runtime_graph, the user drags nodes on canvas. all you get is this:
**connect node 3, port 1 -> node 7, port 0**
just 4 integers.. no c++ ojbects in hand.

**Question:**
If all you have is four integers — out_node, out_port, in_node, in_port — what is the very first problem you need to solve before you can wire anything?

Take a guess

You have node 3, port 1 — but what type is that pin? Is it `out_pin<pose_sample>`? `out_pin<dynamics_sample>`? You don't know just from integers.

**so you need a registry.**
Before any wiring can happen, every filter must **register** its pins when it's created.

For each pin you need to store:

1. **What node and port it belongs to** — so you can look it up
2. **What type it carries** — so you can check compatibility before connecting
3. **The actual pin object** — so you can call connect on it

**The registry as a map**

```
out_pins_[node_id][port] = { type, pin object }
in_pins_[node_id][port]  = { type, pin object }
```

When the user connects node 3 port 1 → node 7 port 0:

1. Look up `out_pins_[3][1]` — get type + pin
2. Look up `in_pins_[7][0]` — get type + pin
3. Check types match
4. Wire them together

Step 3 says check types match. How would you check that `out_pins_[3][1]` and `in_pins_[7][0]` carry the same type — given that you only have integers at this point, no C++ type information?



you check the map and equate types

Exactly — but how is the type stored in the map?

You can't store `out_pin<pose_sample>` and `out_pin<dynamics_sample>` in the same map — they are different C++ types.

So the type must be stored as something **uniform** that every pin entry has regardless of its actual C++ type.

**What simple data type could represent "this pin carries pose_sample" in a way that's easy to compare?**

A **string**.

```cpp
out_pins_[3][1].type_id = "pose_sample";
out_pins_[7][0].type_id = "pose_sample";
```

Then checking compatibility is just:

```cpp
if (out_entry.type_id != in_entry.type_id) return false;  // type mismatch
```

Simple string comparison. That's it.

When a filter is created and registered:

```
ground_vehicle_source, port 0  →  type_id = "pose_sample"
ground_vehicle_source, port 1  →  type_id = "dynamics_sample"
csv_sink, port 0               →  type_id = "pose_sample"
```

When user connects node 3 port 0 → node 7 port 0:

```
out type_id = "pose_sample"
in  type_id = "pose_sample"
match ✓ — proceed to wire
```

When user connects node 3 port 1 → node 7 port 0:

```
out type_id = "dynamics_sample"
in  type_id = "pose_sample"
mismatch ✗ — reject, show error in UI
```


So your registry entry for every pin needs at minimum two things:

```cpp
struct PinEntry {
    std::string type_id;   // "pose_sample", "dynamics_sample" etc.
    ???         pin;       // the actual pin object
};
```

**Question:** what do you store for the pin itself? Remember — you can't store `out_pin<pose_sample>` and `out_pin<dynamics_sample>` in the same struct.


You need something that can hold **any pointer** regardless of type.

In C++ there is one type that can point to anything:

```cpp
void*
```

A `void*` can store a pointer to `out_pin<pose_sample>`, `out_pin<dynamics_sample>`, or anything else. The type is erased — forgotten. Just a raw address.

---

## So the registry entry looks like

```cpp
struct InPinEntry {
    std::string type_id;    // "pose_sample"
    void*       pin_ptr;    // points to the actual in_pin<T>, type forgotten
};
```

---

## But now you have a problem

You stored the pin as `void*` — type forgotten. Later when you want to actually **call connect on it**, how do you use it?

You need to cast it back:

```cpp
static_cast<in_pin<pose_sample>*>(pin_ptr)->make_receiver();
```

But at the point of connecting, all you have is the `void*` and the type string `"pose_sample"`. You'd need a big if/else:

```cpp
if (type_id == "pose_sample")
    static_cast<in_pin<pose_sample>*>(pin_ptr)->...
else if (type_id == "dynamics_sample")
    static_cast<in_pin<dynamics_sample>*>(pin_ptr)->...
```

That's ugly and breaks every time you add a new sample type.

---

**Question:** how would you avoid that if/else chain and make it work for any type automatically?

## The solution — store a lambda instead

Instead of storing the `void*` and figuring out the cast later, you store a **lambda that already knows the type** and does the cast inside itself.

```cpp
struct OutPinEntry {
    std::string                         type_id;
    std::function<uint64_t(void*)>      connect_fn;    // ← lambda stored here
    std::function<void(uint64_t)>       disconnect_fn;
};
```

The lambda is created at registration time — when T is still known:

```cpp
template<typename T>
void register_out_pin(NodeId node_id, PortIdx port, std::string type_id, out_pin<T>* pin)
{
    OutPinEntry e;
    e.type_id    = type_id;
    e.connect_fn = [pin](void* in_ptr) -> uint64_t {
        auto* typed = static_cast<in_pin<T>*>(in_ptr);  // T is known here ✓
        return pin->connect(typed->make_receiver());
    };
}
```

The lambda **captures T inside its closure**. After registration T disappears from the outside — but the lambda still remembers it internally.

---

## Why this is clever

||When T is known|When T is forgotten|
|---|---|---|
|Registration time|✓ lambda created with T baked in|—|
|Connect time|—|just call `connect_fn(in_ptr)` — no if/else needed|

---

## The full flow

```
register_out_pin<pose_sample>(3, 0, "pose_sample", &filter.pose_out)
    → lambda created, T=pose_sample baked inside
    → stored in out_pins_[3][0]

user connects node 3 port 0 → node 7 port 0
    → look up out_pins_[3][0] → get connect_fn
    → look up in_pins_[7][0]  → get void* pin_ptr
    → call connect_fn(pin_ptr)
        → inside lambda: cast void* → in_pin<pose_sample>*  ✓
        → call pin->connect(make_receiver())
        → return handle
```

No if/else. Works for any type. Adding a new sample type requires zero changes to `runtime_graph`.

---

Does that click? Take a moment and tell me in your own words what the lambda is doing


## What you already understand ✓

- Why `runtime_graph` exists — runtime wiring via node/port integers
- The registry — `out_pins_` and `in_pins_` maps
- Type checking via string comparison
- `void*` for storing any pin pointer
- **Type erasure via lambda** — the hardest concept, you got it











