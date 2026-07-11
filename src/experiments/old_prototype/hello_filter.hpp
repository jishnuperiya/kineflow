#pragma once
#include <iostream>
#include "filter.hpp"

namespace kineflow::pipeline
{
class hello_filter: public filter
{
    public:
    void process(double timestamp_sec, double dt_sec) override
    {
        std::cout << "Hello, world! Time: " << timestamp_sec << "s, dt: " << dt_sec << "s\n";
    }
};
} // namespace kineflow::pipeline