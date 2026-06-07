//******** Copyright © 2026 Jishnu Periya. All rights reserved.
//*
//*
//*  Version : $Header:$
//*
//*
//*  Purpose : Typed pin templates for the SENTINEX pipeline framework.
//*
//*            in_pin<T>   — a filter input; holds the latest received sample.
//*            out_pin<T>  — a filter output; delivers samples to all connected
//*                          in_pins (fan-out supported).
//*
//*            Each connection returns a uint64_t handle. Pass that handle to
//*            out_pin::disconnect() to remove a live connection — used by the
//*            runtime_graph when the user removes a wire in the visual editor.
//*
//*  See Also: filter_graph.hpp  — static graph (compile-time wiring)
//*            runtime_graph.hpp — live graph (wires/unwires at runtime)
//*            sample.hpp        — concrete sample types
//*
//*
//****************************************************************************
#pragma once
//****************************************************************************

#include <cstdint>
#include <functional>
#include <map>
#include <memory>

//****************************************************************************

namespace kineflow::pipeline
{

//****************************************************************************

/**
 * @brief Filter input — holds the most-recently received sample of type T.
 */
template<typename T>
class in_pin
{
public:
  bool has_data() const { return latest_ != nullptr; }
  std::shared_ptr<const T> read() const { return latest_; }
  void receive(std::shared_ptr<const T> s) { latest_ = std::move(s); }

  std::function<void(std::shared_ptr<const T>)> make_receiver()
  {
    return [this](std::shared_ptr<const T> s){ receive(std::move(s)); };
  }

private:
  std::shared_ptr<const T> latest_;
};

//****************************************************************************

/**
 * @brief Filter output — fan-out delivery with handle-based disconnect.
 *
 * connect() returns a uint64_t handle. Passing that handle to disconnect()
 * removes exactly that one receiver, leaving others intact.
 */
template<typename T>
class out_pin
{
public:

  /**
   * @brief Register a receiver. Returns a handle for later disconnect().
   */
  uint64_t connect(std::function<void(std::shared_ptr<const T>)> r)
  {
    const uint64_t h = next_handle_++;
    receivers_.emplace(h, std::move(r));
    return h;
  }

  /**
   * @brief Remove the receiver registered under @p handle.
   * No-op if the handle is unknown.
   */
  void disconnect(uint64_t handle) { receivers_.erase(handle); }

  /** Deliver a sample to every connected in_pin. */
  void write(std::shared_ptr<const T> s) const
  {
    for (const auto& [h, r] : receivers_) r(s);
  }

  bool connected() const { return !receivers_.empty(); }

private:
  std::map<uint64_t, std::function<void(std::shared_ptr<const T>)>> receivers_;
  uint64_t next_handle_ = 1;
};

//****************************************************************************
} // namespace sentinex::pipeline
//****************************************************************************
