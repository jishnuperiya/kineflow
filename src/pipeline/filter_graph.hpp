//******** Copyright © 2026 Jishnu Periya. All rights reserved.
//*
//*
//*  Version : $Header:$
//*
//*
//*  Purpose : Filter graph — owns filters, wires pins, runs the tick scheduler.
//*
//*            The graph is the top-level object in the pipeline framework.
//*            Filters are added in topological order (sources first, sinks
//*            last). connect() wires an out_pin to an in_pin. tick() calls
//*            process() on every filter in insertion order once per step.
//*
//*  Usage:
//*            filter_graph graph;
//*            auto& src  = graph.add(std::make_unique<ground_vehicle_source>(params));
//*            auto& sink = graph.add(std::make_unique<csv_sink_filter>("out.csv"));
//*            graph.connect(src.pose_out, sink.pose_in);
//*            graph.run(60.0, 0.1);
//*
//*  See Also: filter.hpp — abstract filter base
//*            pin.hpp    — in_pin<T> / out_pin<T>
//*
//*
//****************************************************************************
#pragma once
//****************************************************************************

#include "filter.hpp"
#include "pin.hpp"

#include <memory>
#include <vector>

//****************************************************************************

namespace kineflow::pipeline
{

//****************************************************************************

/**
 * @brief Owns a set of filters and drives their execution.
 *
 * Filters are ticked in insertion order, so add sources before sinks.
 * Thread safety: single-threaded only.
 */
class filter_graph
{
public:

  /**
   * @brief Add a filter to the graph.
   * @tparam F  Concrete filter type (must derive from filter).
   * @return    Reference to the added filter so callers can access its pins.
   */
  template<typename F>
  F& add(std::unique_ptr<F> f)
  {
    F& ref = *f;
    filters_.push_back(std::move(f));
    return ref;
  }

  /**
   * @brief Wire an out_pin to an in_pin.
   *
   * The connection is type-safe: both pins must carry the same sample type T.
   * One out_pin can be connected to multiple in_pins (fan-out).
   */
  template<typename T>
  uint64_t connect(out_pin<T>& src, in_pin<T>& dst)
  {
    return src.connect(dst.make_receiver());
  }

  /**
   * @brief Advance all filters by one time step.
   * @param timestamp_sec  Current simulation time [s]
   * @param dt_sec         Step duration [s]
   */
  void tick(double timestamp_sec, double dt_sec)
  {
    for (auto& f : filters_)
      f->process(timestamp_sec, dt_sec);
  }

  /**
   * @brief Run the simulation for a fixed duration at a fixed rate.
   * @param duration_sec  Total simulation time [s]
   * @param dt_sec        Tick interval [s]  (e.g. 0.1 for 10 Hz)
   */
  void run(double duration_sec, double dt_sec)
  {
    const auto steps = static_cast<int>(duration_sec / dt_sec);
    for (int i = 0; i < steps; ++i)
      tick(i * dt_sec, dt_sec);
  }

  /** @return Number of filters in the graph. */
  std::size_t size() const { return filters_.size(); }

private:
  std::vector<std::unique_ptr<filter>> filters_;
};

//****************************************************************************
} // namespace kineflow::pipeline
//****************************************************************************
