#pragma once

#include "filter.hpp"
#include "int_type.hpp"
#include <iostream>

namespace kineflow::pipeline
{

/**
 * number_source - Outputs a constant integer value
 * 
 * Properties:
 *   - "value" (int): The constant value to output
 * 
 * Pins:
 *   - output (int): The output pin carrying the constant value
 */
class number_source_filter : public filter
{
public:
  void configure(const json& config) override
  {
    // implement later
  }

  void process(double timestamp_sec, double dt_sec) override
  {
    // implement later
  }

  int get_output() const { return m_output_value; }

private:
  int m_output_value = 0;

};

} // namespace kineflow::pipeline
