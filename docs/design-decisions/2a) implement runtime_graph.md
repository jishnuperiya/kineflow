
# Question 1

When `runtime_graph` is created and a filter is added — what two things does it need to store about that filter?

When you add a filter you need:

1. **The node ID** — to find it later
2. **The filter object itself** — to own it and tick it
What C++ type would you use to own the filter object? (You need something that **owns** the filter and automatically cleans it up when removed)

So the ownership map is:

```cpp
std::unordered_map<NodeId, std::unique_ptr<filter>> owned_;
```

# Question 2:

`tick()` needs to call `process()` on every filter in insertion order. But `unordered_map` has no guaranteed order. How do you solve that?

my ans: std::map - not correct. because map sort the entries in the nodeId order. not insertion order

my question: why process() is called in insertion order?

The difference between frameworks is **how** they enforce the order:

|Approach|How|
|---|---|
|Sentinex (current)|User adds filters in correct order — insertion order|
|Advanced systems|Auto-sort by analyzing pin connections — topological sort|

**Topological sort is a future feature** you mentioned earlier — the graph figures out the correct order automatically even if filters are added randomly.

For now insertion order is correct and simple.

Q: **what data structure preserve insertion order in c++ ?**

A vector!
a pair of node ID + raw pointer to the filter. raw pinter because owned_ aready owns the filter via unique_ptr- tick order_just needs to reference it for ticking.

```cpp
std::vector<std::pair<NodeId, filter*>> tick_order_;
```

So far you have two member variables:

```cpp
std::unordered_map<NodeId, std::unique_ptr<filter>> owned_;      // owns filters
std::vector<std::pair<NodeId, filter*>>              tick_order_; // tick sequence
```


question: why i need that map at all? i could have just used a vector.

## The two containers do different jobs

|Container|Purpose|Access pattern|
|---|---|---|
|`tick_order_` vector|Tick filters in order|Iterate all|
|`owned_` map|Own + look up a specific filter by ID|Look up by NodeId|

---

## When you need the map

When the user calls `remove_filter(7)` — you need to:

1. Destroy the filter with node ID 7 — **fast lookup by ID** → `owned_.erase(7)` → O(log n)
2. Remove it from tick order — iterate vector, find and remove


note: 
For a small number of filters — 10, 20, even 100 — just the vector would work fine. The map is about correctness and good practice, not performance at this scale.

