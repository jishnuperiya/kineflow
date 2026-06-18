#pragma once
#include<map>

namespace kineflow::pipeline
{

  using value_ptr = std::unique_ptr<value>;
using type_ptr = std::unique_ptr<type>;


struct filter
{
  virtual ~filter() = default;
  virtual void process(double timestamp_sec, double dt_sec) = 0;
};

// a filter that can be dynamically instantiated from a textual description of its types and properties
struct json_filter : filter
{
  
};
//types names for k and v
struct value;
/*
given a type, i can use it to instantiate a value of that type from a text representation of the value
*/
struct type 
{
 virtual ~type() = default;

//  virtual std::string get_type_name() const = 0;
 virtual std::unique_ptr<value> new_value(std::string_view) const  = 0; 
};


struct value
{
  virtual ~value() = default;
  virtual void insert(std::ostream&) const = 0; 

};

//-------belwo move into plugins or cpp files for now

//-----


/*

{
      "id": 0,
      "type": "number_source",
      "name": "my_source",
      "properties": [
        {"name": "value", "type": "int", "values": "20"}
      ],
      "pins": [
        {"id": 0, "name": "output", "direction": "out", "type": "int"}
      ]
    }

*/

} // namespace kineflow::pipeline

