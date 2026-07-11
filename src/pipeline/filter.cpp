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
    // TODO(#16): static/closed registry - revisit for plugin based registration
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
    //TODO(#17) : custom exception classes  
    throw std::invalid_argument("Unknown filter type: " + type + ". Available types: number_source, multiply");  //TODO - get the available tyoes from map itself 
  }

} // namespace kineflow::pipeline