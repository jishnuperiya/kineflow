#pragma once

#include "filter.hpp"
#include <iostream>

namespace kineflow::pipeline
{

/**
 * console_printer - Prints input values to console
 * 
 * Properties:
 *   (none)
 * 
 * Pins:
 *   - input (int): The value to print
 */
class console_printer_filter : public filter
{
public:
  void configure(const json& config) override
  {
  }
  void process(double timestamp_sec, double dt_sec) override
  {
    
  }

  void set_input(int value) 
  { 
    m_input = value; 
  }
  int get_input() const
  {
   return m_input; 
  }

  
private:
  int m_input = 0;

};

} // namespace kineflow::pipeline
