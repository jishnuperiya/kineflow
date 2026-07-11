#pragma once


namespace kineflow::pipeline
{
struct sample
{
  virtual ~sample() = default;
  double timestamp_sec = 0.0;
};

struct dot_sample : public sample
{
  double x = 0.0;
  double y = 0.0;
};

} // namespace kineflow::pipeline