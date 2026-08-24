Read connection_json["source"]["filter"] → get 0 (just a number).
Look it up: filters_by_id.at(0) → get the real filter* for my_source.
Read connection_json["source"]["pin"] → get 0 (just a number).
Ask that filter: source_filter->get_pin(0) → get the real pin* for my_source's "output" pin.
Do the same four steps for "target" → real filter* and pin* for my_multiplier's "input" pin.


You have std::vector<std::unique_ptr<filter>> filters — actual, live filter objects sitting in memory, each one built from create_filter(filter_json).
The JSON says "filter 0" — but that's just a number. Nothing about it tells the program which actual filter object in memory that number refers to. That's the whole reason you're building filters_by_id: it's a lookup table so that when you read "filter": 0 out of a connection, you can ask "which real filter object is this?" and get back an actual filter* you can call methods on.
Same problem one level deeper: "pin": 0 is just a number too. Once you know which filter (via filters_by_id), you still need to ask that specific filter, "do you have a pin with id 0?" — that's exactly what get_pin(0) answers. It hands back the real pin* (or nullptr if that filter has no such pin).