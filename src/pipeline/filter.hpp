#pragma once

namespace kineflow::pipeline
{
class filter
{
public:
  virtual ~filter() = default;
  virtual void process(double timestamp_sec, double dt_sec) = 0;
};

} // namespace kineflow::pipeline

