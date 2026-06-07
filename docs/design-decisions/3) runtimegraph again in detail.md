
**What problem does `runtime_graph` solve that `filter_graph` cannot?**

runtime graph help the nodes to be dragged and wired with other notes at runtime in the UI


# Registry
**When the user drags a wire from node 3 port 1 to node 7 port 0 — what is the very first thing `runtime_graph` needs to do before it can wire anything?**

check whether the pins type match

 **what does it need to have stored beforehand?**

a string type storing what type it is

**Exactly. And where is that string stored — what data structure holds it?**

std::string

That's the type — but I mean the container that holds all the pin entries.

**You need to look up pin entries by node ID and port number. What data structure did we say is perfect for that kind of lookup?**

When you look up a pin entry you get back a **struct** containing everything you need for that pin:

For an out pin entry you need:

1. `type_id` string — for type checking
2. `connect_fn` lambda — for actually wiring
3. `disconnect_fn` lambda — for removing the wire

For an in pin entry you need:

1. `type_id` string — for type checking
2. `void*` — raw pointer to the actual `in_pin<T>`

---

So the lookup is:

```
out_pins_[node_id][port]  →  returns OutPinEntry struct
in_pins_[node_id][port]   →  returns InPinEntry struct
```

---

**Now — what data structure gives you this two-level lookup by node ID then port number?**

```cpp
std::unordered_map<NodeId, std::unordered_map<PortIdx, OutPinEntry>>
```

So now you have the full registry:

```cpp
std::unordered_map<NodeId, std::unordered_map<PortIdx, OutPinEntry>> out_pins_;
std::unordered_map<NodeId, std::unordered_map<PortIdx, InPinEntry>>  in_pins
```

now we understood the registry.





So you have:

```cpp
out_pins_[node_id][port]  →  OutPinEntry
in_pins_[node_id][port]   →  InPinEntry
```


do you understand what's **inside** each of those structs? Let's check:

`OutPinEntry` has 3 fields. Can you name them and say what each one is for?

```cpp
struct OutPinEntry {
    std::string                           type_id;
    std::function<uint64_t(void*)>        connect_fn;
    std::function<void(uint64_t)>         disconnect_fn;
};
```

look at its signature: `std::function<uint64_t(void*)>`. That means it's a function that takes a `void*` and returns a `uint64_t`.

what do you think it retuns and what you think the void* argument is?

- `void*` — pointer to the input pin being connected
- `uint64_t` — a handle (a unique ID for that specific connection)

The handle is important because later when you want to **disconnect**, you need to say "remove _this specific wire_",

now disconnect function: `std::function<void(uint64_t)>`.- it takes the handle and remove the wire

```cpp
 struct InPinEntry {
    std::string type_id;
    void*       pin_ptr = nullptr;
  };
```

inpinentry has only type_id and pointer to the pin.
. why the InPinEntry has no connect() function and disconnect() function?

becauese the the output pin is the one that "pushes " the data. so it owns the connection logic. input pin is there to be found and handed over. output pin does the actual wiring.

now we have the clear picture of the registry

# Connect()

let start from the basics
```cpp
int x = 5;
auto add_x = [x](int y) { return x + y; };
```

Can you tell me what `add_x(3)` returns?

add_x(3) calls the lambda and return value 8.
key thing here is that x is caputre. it means the lambda remember the value of x . so even when x goes out of scope, lambda still has it.


