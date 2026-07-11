#include <stdexcept>
#include <string>

#include "filter.hpp"

namespace kineflow::pipeline
{

  using filter_factory_function = std::function<std::unique_ptr<filter>(const nlohmann::json&)>;

  extern filter_factory_function create_number_source_filter;
  extern filter_factory_function create_multiply_filter;
  
  // const std::unordered_map<std::string_view, filter_factory_function> filter_factory_map;
  // const std::unordered_map<std::string_view, filter_factory_function> filter_factory_map
  // {
  //     {"number_source", create_number_source_filter},
  //     {"multiply", create_multiply_filter}
  // };

 
  std::uniue_ptr<pin> create_filter(const nlohmann::json& config);
  std::unique_ptr<filter> filter_factory::create_filter(const nlohmann::json& config)
  {
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
    
    throw std::invalid_argument("unknown filter type: " + type);
   
  }

} // namespace kineflow::pipeline