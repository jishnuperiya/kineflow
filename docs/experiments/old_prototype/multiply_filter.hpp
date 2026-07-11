#pragma once

#include "filter.hpp"
#include "int_type.hpp"

namespace kineflow::pipeline
{

/**
 * multiply - Multiplies an input value by a constant factor
 * 
 * Properties:
 *   - "factor" (int): The multiplication factor
 * 
 * Pins:
 *   - input (int): The input value
 *   - output (int): The result (input * factor)
 */
class multiply_filter : public filter
{

};
} // namespace kineflow::pipeline
