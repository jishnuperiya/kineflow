#include <stdexcept>
#include <string>

#include "filter.hpp"

namespace kineflow::pipeline
{
  using filter_factory_function = std::function<std::unique_ptr<filter>(const nlohmann::json&)>;
  
  // extern is redundant here since functions has external linkage by default. but good practice.
  extern std::unique_ptr<filter> create_number_source_filter(const nlohmann::json& config);
  extern std::unique_ptr<filter> create_multiply_filter(const nlohmann::json& config);  

  std::unique_ptr<filter> create_filter(const nlohmann::json& config)
  {
    // think: static/closed registry - revisit for plugin based registration
    static const std::unordered_map<std::string_view, filter_factory_function> filter_factory_map  
    {
      {"number_source", create_number_source_filter}, 
      {"multiply", create_multiply_filter}
    }; 

    if(!config.is_object())
    {
      throw std::invalid_argument("filter config must be a json object");
    }

    if(!config.contains("type") || !config["type"].is_string())
    {
      throw std::invalid_argument("filter config must contain a string field named `type`");
    }
  
    const auto type = config["type"].get<std::string>();

    if(auto it= filter_factory_map.find(type); it!=filter_factory_map.end())
    {
       return it->second(config);
    }
    //do: custom exception classes
    throw std::invalid_argument("Unknown filter type: " + type + ". Available types: number_source, multiply");  // get the available tyoes from map itself
  }

  namespace
  {
    pin::direction parse_direction(const std::string& direction)   //todo:new
    {
      if (direction == "in" ) return pin::direction::in;
      if (direction == "out" ) return pin::direction::out;
      throw std::invalid_argument("Unknown direction: " + direction);
    }

    pin::type parse_type(const std::string& type)   //todo:new
    {
      if (type == "int") return pin::type::integer;
      if (type == "real") return pin::type::real;
      throw std::invalid_argument("Unknown type: " + type);
    }

  }

  pin_ptr create_pin(const nlohmann::json& config)   //todo:new
  {
   return std::make_unique<pin>
     (
       config["id"].get<int>(),
       config["name"].get<std::string>(),
       parse_direction(config["direction"].get<std::string>()),
       parse_type(config["type"].get<std::string>())
     );
  }

  pin_vec create_pins(const nlohmann::json& config)   //todo:new
  {
    pin_vec pins;
    for (const auto& pin_json : config)
    {
      pins.push_back(create_pin(pin_json));
    }
    return pins;
  }

} // namespace kineflow::pipeline