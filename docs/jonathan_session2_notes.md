# Jonathan's Session 2 — Complete Notes
**Date:** June 2026  
**Topics:** JSON Schema Design, Context-Free Grammars, Pipeline Parser

---

## Part 1 — JSON Schema Review and Improvements

### What Jonathan Said About My Original JSON

He read through the original JSON and gave specific feedback:

**What was good:**
- filters array — correct
- connections array — correct
- properties concept — correct
- pins concept — correct
- filter has an ID number — correct

**What needed changing:**

**1. Use zero-based IDs**
Start filter IDs from 0, not 1. Convention in most systems.

**2. Properties should be an array**
```json
// WRONG
"properties": {"value": "20"}

// CORRECT
"properties": [{"name": "value", "value": "20"}]
```
Properties is plural — it should be an array of property objects, not a single object.

**3. Filters need a name field**
A filter has a type (what class it is) AND a name (what this instance is called).
You might have several number_sources — how do you tell them apart? By name.

```json
{
  "id": 0,
  "type": "number_source",
  "name": "my_source"
}
```

**4. Pins need a name field**
A pin's full name is `filter_name.pin_name` — for example `my_source.output`.
This is how you'd refer to a specific pin in a visual editor.

**5. Connections — use source/target with nested objects**
```json
// WRONG — flat structure with 4 fields
{"from_node": 1, "from_port": 0, "to_node": 2, "to_port": 0}

// CORRECT — nested structure, cleaner
{
  "source": {"filter": 0, "pin": 0},
  "target": {"filter": 1, "pin": 0}
}
```

**6. Don't use the word "port"**
Port and pin mean the same thing — use "pin" everywhere consistently.

**7. "node" and "filter" mean the same thing**
Jonathan clarified — use "filter" consistently, not "node".

---

### The Corrected JSON

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

---

## Part 2 — What is a Context-Free Grammar?

### Why Jonathan Taught This

The JSON file is a tree structure. To parse it correctly, you need to know:
- What is a valid pipeline JSON?
- What should you check for?
- What should you reject?

A Context-Free Grammar (CFG) is the formal way to answer all three questions.

---

### Starting From Scratch — Alphabets and Strings

**Alphabet:**
A set of characters. For most programming languages, the alphabet is the printable ASCII characters — letters, digits, punctuation.

**String over an alphabet:**
A finite sequence of characters drawn from the alphabet.
Example: `hello123` is a string over the ASCII alphabet.

**Language:**
A subset of all possible strings. Not every sequence of characters is valid — for example `!!!x` might use valid characters but is not valid C++.

The job of a parser is to decide: is this string a valid member of the language?

---

### Regular Expressions — A Special Case

You already know regular expressions. Jonathan explained that regex is actually a special case of CFG.

Regular expression operators:
```
a           →  matches the literal character 'a'
re re       →  concatenation (one after another)
re | re     →  disjunction (either one or the other)
re *        →  Kleene star (zero or more)
re +        →  one or more (sugar for re re*)
re{0,3}     →  zero to three occurrences
( )         →  grouping
[A-Za-z]    →  character class (any letter)
[A-Za-z0-9] →  any letter or digit
```

Example — a C identifier in regex:
```
[A-Za-z][A-Za-z0-9]*
```
Means: a letter followed by zero or more letters or digits.

---

### Context-Free Grammars — The Full Power

A CFG is more powerful than regular expressions. It adds explicit recursion.

**A CFG consists of:**
1. An alphabet (the characters)
2. A set of productions (the rules)
3. A starting symbol (where to begin)

**A production looks like:**
```
non-terminal ::= sequence of non-terminals and terminals
```

- **Non-terminal** = a variable, a placeholder for a set of strings (written in lowercase)
- **Terminal** = an actual character or string from the alphabet

**Example — CFG for a C identifier:**

```
id     ::=  alpha alnums
alnums ::=           (empty string)
alnums ::=  alnums alnum
alnum  ::=  alpha
alnum  ::=  digit
alpha  ::=  [A-Za-z]
digit  ::=  [0-9]
```

**How to read this:**
- `id` is the starting symbol
- An `id` is an `alpha` followed by `alnums`
- `alnums` is either empty OR `alnums` followed by one more `alnum` (recursion!)
- `alnum` is either an `alpha` or a `digit`
- `alpha` is any letter
- `digit` is any digit

**Derivation example — proving "JX7" is a valid identifier:**
```
id
→ alpha alnums          (use: id ::= alpha alnums)
→ 'J' alnums            (use: alpha ::= 'J')
→ 'J' alnums alnum      (use: alnums ::= alnums alnum)
→ 'J' alnums alpha      (use: alnum ::= alpha)
→ 'J' alnums 'X'        (use: alpha ::= 'X')
→ 'J' alnums alnum 'X'  (use: alnums ::= alnums alnum)
→ 'J' alnums digit 'X'  (use: alnum ::= digit)
→ 'J' alnums '7' 'X'    (use: digit ::= '7')
→ 'J' '' '7' 'X'        (use: alnums ::= empty)
→ 'J' 'X' '7'           (collapse)
```

**Key insight about recursion:**
Because `alnums ::= alnums alnum` contains `alnums` on the right side, this is recursive. That's how a finite grammar describes an infinite language — an identifier can be any length.

---

### How Parsing Works (Reverse of Derivation)

Given the string "JX7", a parser works backwards:
- 'J' must be an `alpha` (only production that matches)
- 'X' must be an `alpha`
- '7' must be a `digit`
- `alpha digit` = `alnum alnum` = `alnums`
- `alpha alnums` = `id`

This reverse process is what a parser does. Jonathan mentioned two tools:
- **LEX** — takes regular expressions, produces a lexer (tokenizer)
- **YACC** — takes a CFG, produces a full parser

---

### json.org — The JSON Grammar

Jonathan pointed to **json.org** — a single web page showing the complete CFG for JSON using syntax diagrams. Worth reading.

Key point: JSON itself is defined by a CFG. nlohmann/json implements a parser for that CFG.

---

## Part 3 — The Complete CFG for Kineflow Pipelines

### The Full Grammar

```
pipeline    ::=  { "filters":     [ filter     ],
                   "connections":  [ connection ] }

filter      ::=  { "id":          natural,
                   "type":        filter-type,
                   "name":        name,
                   "properties":  [ property ],
                   "pins":        [ pin       ] }

pin         ::=  { "id":          natural,
                   "name":        name,
                   "direction":   direction,
                   "type":        data-type   }

connection  ::=  { "source":      filter-pin,
                   "target":      filter-pin  }

filter-pin  ::=  { "filter":      natural,
                   "pin":         natural     }

property    ::=  { "name":        name,
                   "type":        data-type,
                   "values":      [ value ]   }

value       ::=  string
             |   number
             |   bool

direction   ::=  "in" | "out"

name        ::=  C identifier (letters, digits, underscore)
natural     ::=  non-negative integer (side condition on JSON number)
filter-type ::=  name  (looked up in filter registry map)
data-type   ::=  name  (looked up in data type registry map)
```

---

### Two Kinds of Errors

**Syntactic errors** — caught by the CFG:
- Missing required field (no "id" on a filter)
- Wrong structure (connections not an array)
- Wrong type (id is a string instead of a number)

**Semantic errors** — checked separately:
- ID is a float instead of an integer (JSON doesn't distinguish)
- Name is not a valid C identifier
- Filter type not registered in the factory map
- Data type not registered in the type registry
- Value doesn't match the declared property type

**Jonathan's strategy:**
```
Step 1  →  nlohmann/json parses the string
            throws exception if not valid JSON

Step 2  →  walk the JSON tree checking CFG constraints
            throw error if anything is missing or wrong type

Step 3  →  semantic checks
            is id a non-negative integer?
            is name a valid C identifier?
            is filter type registered?
            is data type registered?

Step 4  →  if all passes → pipeline is valid
```

---

### Three Representations of a Pipeline

Jonathan was very clear about this — there are three distinct stages:

```
Stage 1: String in file
─────────────────────────────────────
{ "filters": [...], "connections": [...] }
Just text. No meaning yet.

Stage 2: JSON tree in memory
─────────────────────────────────────
nlohmann::json objects
Arrays, objects, strings, numbers, bools
No C++ filter class exists yet

Stage 3: C++ objects
─────────────────────────────────────
filter* objects created from factory
pins connected
ready to execute
```

**Key insight:** These are completely separate problems. Don't mix them.
- Parsing into the tree knows nothing about how filters work
- Building filter objects knows nothing about JSON syntax
- Each stage can be tested independently

---

### The Four Functions to Write

**1. parse_pipeline(istream& in) → json tree**
```
- call nlohmann::json::parse(in)
- walk the tree validating against the CFG
- throw descriptive errors for any violation
- return the validated JSON tree
```

**2. dump_pipeline(json tree) → string**
```
- serialize the JSON tree back to a string
- nlohmann/json probably provides this
- useful for testing and debugging
```

**3. make_pipeline(json tree) → C++ objects**
```
- walk the validated JSON tree
- for each filter: look up type in factory map, create object, set properties
- for each connection: find filters, connect pins
- return the ready-to-run pipeline
```

**4. print_pipeline(json tree)**
```
- recursively print the tree in a human-readable format
- useful for debugging
- write this first to understand the tree structure
```

---

### Testing Strategy — Idempotent Parse/Dump

Jonathan described a powerful property-based test:

```
parse(JSON string) → tree
dump(tree) → JSON string 2
parse(JSON string 2) → tree 2
dump(tree 2) → JSON string 3

Assert: JSON string 2 == JSON string 3
```

Parsing then dumping then parsing again should give the same result. This is called **idempotency**. If it doesn't hold, your parser or dumper has a bug.

Use **RapidCheck** to randomly generate valid pipeline JSON strings and verify this property holds.

---

### Installing nlohmann/json

Jonathan said to use CMake FetchContent — same pattern as other dependencies.

Steps:
1. Find the nlohmann/json GitHub page
2. Find the latest release URL and its hash
3. Add FetchContent block to CMakeLists.txt
4. CMake will download it automatically when building

Basic CMake pattern:
```cmake
include(FetchContent)
FetchContent_Declare(
  nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
  URL_HASH SHA256=<hash>
)
FetchContent_MakeAvailable(nlohmann_json)

target_link_libraries(kineflow PRIVATE nlohmann_json::nlohmann_json)
```

Then in code:
```cpp
#include <nlohmann/json.hpp>

std::ifstream file("pipeline.json");
auto tree = nlohmann::json::parse(file);
```

---

### How nlohmann/json Works

Jonathan described how the JSON library represents the tree:

```cpp
auto j = nlohmann::json::parse(stream);

// Check type
j.is_object()   // is it a JSON object {}?
j.is_array()    // is it a JSON array []?
j.is_string()   // is it a string?
j.is_number()   // is it a number?
j.is_boolean()  // is it a bool?

// Access values
j["filters"]         // get field from object
j["filters"][0]      // get first element of array
j["id"].get<int>()   // get value as specific type
j["name"].get<std::string>()

// Iterate array
for (auto& filter : j["filters"]) {
    // filter is a json object
}
```

Your `parse_pipeline()` will walk this tree checking every field matches the CFG.

---

### The Factory Map

Jonathan described the filter registry — a map from string names to lambdas:

```cpp
std::map<std::string, std::function<filter*()>> filter_registry;

// Register filters
filter_registry["number_source"] = []{ return new number_source(); };
filter_registry["multiply"]      = []{ return new multiply(); };
filter_registry["console_printer"] = []{ return new console_printer(); };

// Use in make_pipeline
auto type = filter_json["type"].get<std::string>();
auto it = filter_registry.find(type);
if (it == filter_registry.end())
    throw std::runtime_error("Unknown filter type: " + type);

filter* f = it->second();  // call lambda, create filter
```

Similarly a `type_registry` for data types:
```cpp
std::map<std::string, data_type*> type_registry;
type_registry["int"]    = new int_type();
type_registry["string"] = new string_type();
```

---

### Error Function

Jonathan suggested a helper for descriptive error messages:

```cpp
void pipeline_error(const std::string& message) {
    throw std::runtime_error("Pipeline error: " + message);
}

// Usage
if (!j.contains("id"))
    pipeline_error("filter is missing required field 'id'");

if (!j["id"].is_number_integer())
    pipeline_error("filter id must be a non-negative integer, got: " 
                   + j["id"].dump());
```

---

## Summary — What To Do Next (In Order)

```
1. Store CFG in docs/pipeline_grammar.md
2. Update examples/calculator_pipeline.json with corrected format
3. Install nlohmann/json via CMake FetchContent
4. Read nlohmann/json documentation
5. Write print_pipeline() — prints JSON tree (learn the API)
6. Write parse_pipeline() — validates against CFG
7. Write dump_pipeline() — serialize back to JSON
8. Write unit tests — parse → dump → parse idempotency
9. Write make_pipeline() — creates C++ objects (next session topic)
```

---

## Key Quotes from Jonathan

*"This exercise was very helpful because in working through a simple graph description, I think we've already identified all of the key elements that will be needed in any graph description."*

*"A pipeline description is valid if it is a valid JSON string that also meets this more refined, more specific context-free grammar and satisfies a number of additional constraints."*

*"The filter object that we actually want in order to implement processing and the JSON node are not the same thing. They don't need to be the same thing."*

*"You've got a secret weapon — you've got my telephone number."*

*"This is a good project. We're getting some real object-oriented design. We're going to see some language compiler basics."*

---

## References Jonathan Mentioned

- **json.org** — complete JSON grammar, read it
- **nlohmann/json** — C++ JSON library to install
- **LEX** — lexer generator tool
- **YACC** — parser generator tool
- **RapidCheck** — property-based testing, for idempotency tests
- **Automata and language theory** — the academic field behind all of this
- **OSGI / JavaBeans** — similar plugin/property patterns (from session 1)
- **Turing, Kleene, Gödel, Cantor** — mathematicians behind formal language theory
