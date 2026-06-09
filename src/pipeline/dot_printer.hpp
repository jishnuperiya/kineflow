#pragma once
#include "filter.hpp"
#include "pin.hpp"
#include "sample.hpp"
#include <iostream>

namespace kineflow::pipeline
{

class dot_printer : public filter
{
public:
    in_pin<dot_sample> in_;

    void process(double timestamp_sec, double dt_sec) override
    {
        if(in_.has_data())
        {
            auto s = in_.read();
            std::cout << "Dot at time " << timestamp_sec << "s: ("
                      << s->x << ", " << s->y << ")\n";
        }
    }
};

} // namespace kineflow::pipeline