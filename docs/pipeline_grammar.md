# Kineflow Pipeline Grammar

A pipeline description file is a JSON string that meets the following rules,
described by this context-free grammar.

---

## Grammar Notation

```
::=   means "is defined as"
|     means "or"
[ ]   means "array of"
{ }   means "JSON object"
*     means "zero or more"
+     means "one or more"
```

Non-terminals are written in lowercase (filter, pin, connection).
Terminals are written in quotes ("filters", "id", "in", "out").

---

## The Grammar

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

direction   ::=  "in"
             |   "out"

name        ::=  C identifier — [A-Za-z][A-Za-z0-9_]*
natural     ::=  non-negative integer (JSON number, side condition: must be integer >= 0)
filter-type ::=  name — must be registered in the filter factory map
data-type   ::=  name — must be registered in the data type registry
```

---

## Side Conditions

These constraints cannot be expressed in the grammar itself.
They are checked separately after parsing:

```
1. natural     →  JSON number must be a non-negative integer (not a float)
2. name        →  must be a valid C identifier [A-Za-z][A-Za-z0-9_]*
3. filter-type →  must be registered in the filter factory map
4. data-type   →  must be registered in the data type registry
5. filter ids  →  must be unique within the pipeline
6. pin ids     →  must be unique within a filter
7. connections →  source and target filter/pin ids must exist
```

---

## Parsing Strategy

Parsing happens in three stages:

**Stage 1 — JSON parsing:**
nlohmann/json parses the raw string into an in-memory tree.
Throws an exception if the string is not valid JSON.

**Stage 2 — Grammar validation (parse_pipeline):**
Walk the JSON tree checking every field against this grammar.
Throw a descriptive error if anything is missing or wrong type.

**Stage 3 — Semantic validation:**
Check the side conditions listed above.
Throw a descriptive error if any constraint is violated.

---

## Example — Valid Pipeline

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

## Error Examples

```
"filter is missing required field 'id'"
"filter id must be a non-negative integer"
"filter name 'my_source' is not a valid C identifier"
"unknown filter type 'foo' — not registered in factory"
"pin direction must be 'in' or 'out', got 'output'"
"connection references filter id 99 which does not exist"
```

---

## References

- json.org — complete JSON grammar
- nlohmann/json — C++ JSON library used for parsing

