#pragma once
#include "filter.hpp"
#include "pin.hpp"
#include "sample.hpp"

namespace kineflow::pipeline
{
class dot_source : public filter
{
  public:
    out_pin<dot_sample> out_;
    void process(double timestamp_sec, double dt_sec) override
    {
        dot_sample s;
        double speed = 1.0; // 1 m/s
        s.timestamp_sec = timestamp_sec;
        s.x = timestamp_sec * speed;
        s.y = timestamp_sec * speed;
        out_.write(std::make_shared<dot_sample>(s));
    }
};
} // namespace kineflow::pipeline