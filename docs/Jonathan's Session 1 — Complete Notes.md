
**Date:** June 2026  
**Context:** Architecture review of Kineflow pipeline framework

# 1. Abstract Base Classes and Runtime Substitution

 The presence of an abstract base class allows substituting additional implementations at runtime. Code that only works with the base class won't know the difference when a new subclass is introduced.

Later, additional implementations can come by loading dynamically supplied modules. so the end user can extend the system with plugins. For now, I will be the main extender.

# 2. Do not parametrize pins on data types

The current design uses templates to parameterize pins on the data type:

`out_pin<T>, in_pin<T>`

This means type resolution and checking happens when templates are instantiated at compile time.

###  The new approach

Although we use the language of a static type checking system to describe data types, those data types are actually **values in the problem domain** we are modeling, not C++ types.

```
C++ type  →  known at compile time, can't be a plugin
Domain value →  known at runtime, can come from JSON, can be a plugin

out_pin<dot_sample>  // C++ type as template parameter

pin p;
p.data_type = new position_type();  // domain value, not C++ type
```

We are going to have an **abstract data type base class**. lets call it `data_type`, but its instances are not going to be C++ types in the traditional sense. There will be a base class with subclasses for specific data types.

When data types are passed around in the application, they will be passed by **base class pointer**.

All type checking and validation will be done without knowing the specific subclasses that have been introduced. because those subclasses will eventually be plugins too.

**Summary:** A pin will have as a data member an **instance of a data type object**, not a C++ template parameter.

## 3. The data_type Class

Example: a pin that represents the position of something in 3D space.

Position would be a vector with x, y, z coordinates. When you connect two filters, you don't want to connect three separate cables for x, y, z. you connect the position pins, and what flows down that pin is the triple (x, y, z).

The **pin base class** should have a function:

```
get_data_type() → data_type*
```

This returns a pointer to the data type base class. The caller doesn't know what specific subclass it is.

Then to check compatibility between two pins:

1. Ask pin A for its data type → get a `data_type*`
2. Ask pin B for its data type → get a `data_type*`
3. Call `compatible(other_data_type)` on one of them

**The `data_type` base class must have:**

```
virtual bool compatible(const data_type* other) const = 0
```

It is a virtual const member function that takes a const pointer (or const reference) to another data type and returns boolean.

It is the job of the concrete subclasses to figure out how compatibility works.

**Starting point:** Start with a single data_type class that has an enum or integer inside it, and check the integers for compatibility. Only move to a full subclass hierarchy if needed.

Start with:

```
using data_type = int  (simplest possible start)
```
after this simpleest possible start we can make datatype as a base class and add compatible() fucntion and make subclasses as plugins (later)
# 4. The Dual Hierarchy

**Hierarchy 1 : Visual/Edit side (Qt):**

- The class representing a filter in the UI is not the filter object itself- it is the **filter viewer**
- The filter view object is a Qt-enabled object with:
    - Position on the canvas
    - List of pins
    - Visual attributes
- Similar hierarchy exists for pins in the UI

**Hierarchy 2 :Execution side (Runtime):**

- Separate set of objects that actually do the processing
- These do not carry any Qt baggage
- They don't need to know anything about display

**The connection between the two:** When the user clicks "Run", something runs around the graph of GUI objects and says to each one: "give me the actual object that's going to do the computation." This process is a **Visitor** that builds the execution graph from the GUI graph.

**Important:** The execution objects don't need to know anything about Qt. The GUI objects don't need to know anything about execution.

## 5. JSON as the Intermediate Form

The JSON file is simultaneously:

1. The persistent data of the pipeline specification
2. The schema for what a pipeline can contain
3. The file format of the editor program
4. The intermediate form that isolates the UI from the execution engine

**Key insight:**

The naive approach would be to have a graph of objects that can compute, and the editor modifies the graph live. 

in our code:
The JSON specification becomes an **intermediate form** that completely isolates:

- Code maintaining the visual representation
- Code maintaining the execution engine

They can be completely separate, both driven from the same simple JSON file.

**reference:** This is similar to **Model-View-Controller (MVC)** introduced by Berkshire Meyer in Smalltalk. 

## 6. JSON Route vs Qt Route

**JSON route (start here):**

- You have a fully constructed specification
- JSON parser reads the file and builds the execution graph directly

**Qt route (later):**

- Interactive editor where user builds pipeline step by step
- Can change mind, add/remove filters, rewire connections
- Those objects (filter_viewer) are only about display- position on canvas, connections
- When user clicks Run- Visitor runs around GUI graph building execution objects

**Jonathan's advice:** Start with JSON. Write a sample JSON file as the first exercise before writing any code.

## 7. The Factory Pattern : Filter Registry

You need a **factory** to create filter objects from their string names in the JSON file.

Implementation:

```
STL map whose keys are strings (filter type names as they appear in JSON)
Values are lambdas that create an empty object on the heap with no arguments
```

Example:

```
"dot_source" → []{ return new dot_source(); }
"calculator" → []{ return new calculator(); }
```

When parsing the JSON file:

1. Read the filter type string
2. Look it up in the factory map
3. Call the lambda → get back a pointer to base filter class
4. Set properties on the filter from JSON

---

## 8. Filter Arguments :The set_property Pattern

**The problem:** Different filters have different constructor arguments. Some have 3 arguments, some have none, some have two strings and a double. We need a general mechanism that works for all filters.

**Jonathan's solution:**

Every filter is designed so it can be created with **no arguments** (default constructor). Then values are set through a virtual function:

```
virtual void set_property(string name, string value)
```

This takes:

- The name of the attribute as a string
- The value as a string (read from JSON)

Inside the filter, there is a map of attributes:

```
map<string, attribute*>
```

The `set_property` function looks up the attribute by name in the map, and tells the attribute object: "here is a string representation of your value, please parse it."

The attribute object uses a string stream to parse the string into the correct type (double, int, etc.).

**Result:** The complete design of the pipeline- filter types, attributes, connections, pins, can be described as strings in a JSON file. This completely solves the problem of specifying how to build a pipeline.

---

# 9. The attribute Class

There will be an **attribute base class** with subclasses:

- `double_attribute`
	- `int_attribute`
	- `string_attribute`
	- etc.
	
	Each attribute:
	
	- Has a default value
	- Has a virtual function to set its value from a string
	- Is stored in the filter's attribute map by name
	
	```
	virtual vector<string> get_attribute_names()
	virtual attribute* get_attribute(string name)
    ```

---

# 10. The Complete Class List

Jonathan described these base classes as the core library:

```
filter          →  base class for all filters
pin             →  base class for all pins  
data_type       →  base class for all data types
attribute       →  base class for all filter parameters
```

Plus supporting infrastructure:

```
factory map     →  string → lambda, creates filters
JSON parser     →  reads JSON, walks tree, builds objects
executor_engine →  ticks filters in order
visitor         →  (later, for GUI route) builds execution graph
```


## 11. The JSON Parser Flow

1. JSON library parses the file → builds an in-memory tree of JSON nodes
2. You walk the tree using the library's API (ask if node is object/array, get children, get string values)
3. For each filter node:
    - Read type string
    - Look up in factory map → get lambda → call it → get empty filter object
    - For each property → call set_property(name, value)
    - For each pin → create pin object, set data type
4. For each connection:
    - Get source pin from source filter
    - Get destination pin from destination filter
    - Check compatibility via data_type::compatible()
    - Connect them

---

## 12. References Jonathan Mentioned

- **OSGI**- Java framework, similar plugin/component architecture
- **JavaBeans**- similar set_property pattern for configurable objects