#pragma once
#include <memory>
#include <vector>
#include "filter.hpp"
#include "pin.hpp"

namespace kineflow::pipeline
{
class filter_graph
{
public:

  template<typename F>
  F& add(std::unique_ptr<F> filter)
  {
    auto* raw = filter.get(); //save the address while still valid
    filters_.push_back(std::move(filter));
    return *raw;
  }
 
  template<typename T>
  void connect(out_pin<T>& out, in_pin<T>& in)
  {
    out.connect(in);
  }

  template<typename T>
  void disconnect(out_pin<T>& out, in_pin<T>& in)
  {
    out.disconnect(in);
  }

  void run(double duration_sec, double dt_sec)
  {
    double timestamp_sec = 0.0;
    while (timestamp_sec < duration_sec) {
      tick(timestamp_sec, dt_sec);
      timestamp_sec += dt_sec;
    }
  }
  
  void tick(double timestamp_sec, double dt_sec)
  {
    for(auto& f: filters_)
    {
      f->process(timestamp_sec, dt_sec);
    }
  }
private:
  std::vector<std::unique_ptr<filter>> filters_;
};
} // namespace kineflow::pipeline