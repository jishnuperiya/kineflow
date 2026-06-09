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

    dot_source(double speed) : speed_(speed) {}
    void process(double timestamp_sec, double dt_sec) override
    {
        dot_sample s;
        s.timestamp_sec = timestamp_sec;
        s.x = x_;
        s.y = y_;
        out_.write(std::make_shared<dot_sample>(s));
        x_ += dt_sec * speed_;
        y_ += dt_sec * speed_;
    }
    private:
      double x_     = 0.0;
      double y_     = 0.0;
      double speed_ = 1.0; // 1 m/s
};
} // namespace kineflow::pipeline