#pragma once

#include "filter.hpp"
#include "number_source_filter.hpp"
#include "multiply_filter.hpp"
#include "console_printer_filter.hpp"

namespace kineflow::pipeline
{

/**
 * Concrete factory for calculator pipeline filters
 * Registers the three filter types used in the calculator example:
 *   - number_source
 *   - multiply
 *   - console_printer
 */
class calculator_filter_factory : public filter_factory
{
protected:
  std::unique_ptr<filter> create_filter_by_type(
      const std::string& type,
      const json& config) override
  {
    if (type == "number_source")
    {
      return std::make_unique<number_source_filter>();
    }
    else if (type == "multiply")
    {
      return std::make_unique<multiply_filter>();
    }
    else if (type == "console_printer")
    {
      return std::make_unique<console_printer_filter>();
    }
  }
};

} // namespace kineflow::pipeline
