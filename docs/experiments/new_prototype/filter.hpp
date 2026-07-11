#pragma once

#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <ostream>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace kineflow::pipeline
{

using json = nlohmann::json;

// Forward declarations
struct value;
struct type;
struct filter;

using value_ptr = std::unique_ptr<value>;
using type_ptr = std::unique_ptr<type>;

/**
 * Abstract base class for pipeline filters
 * Filters process data at each timestep in the pipeline
 */
struct filter
{
  virtual ~filter() = default;
  virtual void process(double timestamp_sec, double dt_sec) = 0;
  
  /// Configure the filter with JSON properties
  /// Called after instantiation with the full filter configuration
  virtual void configure(const json& config) {}
};

/**
 * Abstract base class representing a type in the system
 * Provides factory method to create values from string representations
 */
struct type
{
  virtual ~type() = default;
  
  /// Create a value of this type from a string representation
  virtual std::unique_ptr<value> new_value(std::string_view str) const = 0;
};

/**
 * Abstract base class representing a typed value
 * Can be serialized to an output stream
 */
struct value
{
  virtual ~value() = default;
  
  /// Write the value to an output stream
  virtual void insert(std::ostream& os) const = 0;
};

/**
 * A filter that can be dynamically instantiated from a JSON configuration
 * describing its types and properties
 * 
 * Expected JSON format:
 * {
 *   "id": 0,
 *   "type": "number_source",
 *   "name": "my_source",
 *   "properties": [
 *     {"name": "value", "type": "int", "value": "20"}
 *   ],
 *   "pins": [
 *     {"id": 0, "name": "output", "direction": "out", "type": "int"}
 *   ]
 * }
 */
struct json_filter : filter
{
  int id = -1;
  std::string name;
  json properties;
  json pins;
  
  void configure(const json& config) override
  {
    id = config.value("id", -1);
    name = config.value("name", "");
    properties = config.value("properties", json::array());
    pins = config.value("pins", json::array());
  }
  
  void process(double timestamp_sec, double dt_sec) override
  {
    // Default implementation - override in subclasses to use properties
  }
};

/**
 * Factory for creating filters from JSON configuration objects
 * Reads properties from JSON subtree and instantiates appropriate filter types
 * Throws exceptions on invalid configuration or missing required fields
 */
class filter_factory
{
public:
  /**
   * Create a filter from a JSON object (subtree from parsed pipeline)
   * @param config The JSON object containing filter configuration
   * @return A unique pointer to the created filter
   * @throws std::runtime_error if filter type is not recognized
   */
  std::unique_ptr<filter> create_filter(const json& config)
  {

    std::string filter_type = config["type"].get<std::string>();

    auto created_filter = create_filter_by_type(filter_type, config);

    if (!created_filter)
    {
      throw std::runtime_error("Unknown filter type: '" + filter_type + "'");
    }

    // Configure the filter with properties, pins, id, name
    created_filter->configure(config);

    return created_filter;
  }

  virtual ~filter_factory() = default;

protected:
  /**
   * Override this to register custom filter types
   * @param type The filter type string from JSON
   * @param config The full JSON configuration object
   * @return A filter instance or nullptr if type is not recognized
   */
  virtual std::unique_ptr<filter> create_filter_by_type(
      const std::string& type,
      const json& config)
  {
  
    // Default: create a basic json_filter for unregistered types
    if (type == "json_filter" || type == "generic")
    {
      return std::make_unique<json_filter>();
    }
    return nullptr;
  }
};

using filter_factory_function = std::function<std::unique_ptr<filter>(const json&)>;
extern const std::map<std::string_view, filter_factory_function> filter_factory_map;

} // namespace kineflow::pipeline

