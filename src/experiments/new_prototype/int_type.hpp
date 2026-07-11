#pragma once

#include "filter.hpp"
#include <sstream>

namespace kineflow::pipeline
{

/**
 * Concrete implementation of value for integer type
 */
class int_value : public value
{
private:
  int m_value = 0;

public:
  explicit int_value(int val = 0) : m_value(val) {}

  int get() const { return m_value; }
  void set(int val) { m_value = val; }

  void insert(std::ostream& os) const override
  {
    os << m_value;
  }
};

/**
 * Concrete implementation of type for integer type
 * Creates int_value instances from string representations
 */
class int_type : public type
{
public:
  std::unique_ptr<value> new_value(std::string_view str) const override
  {
    try
    {
      int val = std::stoi(std::string(str));
      return std::make_unique<int_value>(val);
    }
    catch (const std::exception& e)
    {
      throw std::invalid_argument(std::string("Failed to parse int from '") + std::string(str) + "': " + e.what());
    }
  }
};

} // namespace kineflow::pipeline
