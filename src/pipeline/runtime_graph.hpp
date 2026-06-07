//******** Copyright © 2026 Jishnu Periya. All rights reserved.
//*
//*
//*  Version : $Header:$
//*
//*
//*  Purpose : Live filter graph — wires and unwires C++ pins at runtime.
//*
//*            Unlike filter_graph (which is wired once at startup), runtime_graph
//*            supports adding/removing filters and connections while the simulation
//*            is running. It is the C++ counterpart of the QtNodes visual graph:
//*            every visual add/remove/wire event triggers a matching operation here.
//*
//*  How it works
//*            When a filter is added, the factory calls register_out_pin<T>() and
//*            register_in_pin<T>() to store type-erased accessors for each of its
//*            pins. The type string (e.g. "pose_sample") is stored alongside.
//*
//*            When connect(outNode, outPort, inNode, inPort) is called, the graph
//*            checks that both pin type strings match, then calls the stored
//*            connect_fn — a lambda that already knows the concrete type T and
//*            calls out_pin<T>::connect(in_pin<T>::make_receiver()). The returned
//*            handle is stored so disconnect() can remove exactly that wire.
//*
//*            Filters are ticked in insertion order (sources first = add them first).
//*
//*  See Also: filter_factory.hpp  — creates filters and registers their pins
//*            ui/main_window.cpp  — connects QtNodes signals to this graph
//*
//*
//****************************************************************************
#pragma once
//****************************************************************************

#include "filter.hpp"
#include "pin.hpp"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

//****************************************************************************

namespace kineflow::pipeline
{

//****************************************************************************

/**
 * @brief Live filter graph — add/remove filters and connections at any time.
 *
 * NodeId is the same unsigned int that QtNodes uses for its graph nodes,
 * so the visual graph and the C++ graph share the same keys.
 */
class runtime_graph
{
public:
  using NodeId  = unsigned int;
  using PortIdx = unsigned int;

  // ── Filter management ──────────────────────────────────────────────────────

  /** Add a filter for @p node_id. The graph owns the filter. */
  void add_filter(NodeId node_id, std::unique_ptr<filter> f)
  {
    filter* raw = f.get();
    owned_.emplace(node_id, std::move(f));
    tick_order_.push_back({node_id, raw});
  }

  /** Remove a filter (and all its registered pins). */
  void remove_filter(NodeId node_id)
  {
    owned_.erase(node_id);
    out_pins_.erase(node_id);
    in_pins_.erase(node_id);
    tick_order_.erase(
      std::remove_if(tick_order_.begin(), tick_order_.end(),
        [node_id](const auto& p){ return p.first == node_id; }),
      tick_order_.end());
  }

  filter* filter_at(NodeId id) const
  {
    auto it = owned_.find(id);
    return it != owned_.end() ? it->second.get() : nullptr;
  }

  std::size_t size() const { return owned_.size(); }

  // ── Pin registration (called by filter_factory) ────────────────────────────

  /**
   * @brief Register an output pin so connect() can wire it at runtime.
   * @tparam T       Sample type (e.g. pose_sample)
   * @param type_id  String matching NodeDataType::id in node_models.hpp
   */
  template<typename T>
  void register_out_pin(NodeId node_id, PortIdx port,
                         std::string type_id, out_pin<T>* pin)
  {
    OutPinEntry e;
    e.type_id      = std::move(type_id);
    e.connect_fn   = [pin](void* in_ptr) -> uint64_t {
      auto* typed = static_cast<in_pin<T>*>(in_ptr);
      return pin->connect(typed->make_receiver());
    };
    e.disconnect_fn = [pin](uint64_t h){ pin->disconnect(h); };
    out_pins_[node_id][port] = std::move(e);
  }

  /**
   * @brief Register an input pin so connect() can target it at runtime.
   */
  template<typename T>
  void register_in_pin(NodeId node_id, PortIdx port,
                        std::string type_id, in_pin<T>* pin)
  {
    InPinEntry e;
    e.type_id = std::move(type_id);
    e.pin_ptr = static_cast<void*>(pin);
    in_pins_[node_id][port] = std::move(e);
  }

  // ── Connection management ──────────────────────────────────────────────────

  /**
   * @brief Wire an output pin to an input pin.
   * @return true if both pins exist and their types match.
   */
  bool connect(NodeId out_node, PortIdx out_port,
               NodeId in_node,  PortIdx in_port)
  {
    auto* out_e = find_out(out_node, out_port);
    auto* in_e  = find_in (in_node,  in_port);
    if (!out_e || !in_e) return false;
    if (out_e->type_id != in_e->type_id) return false;

    const uint64_t handle = out_e->connect_fn(in_e->pin_ptr);
    connection_handles_[encode(out_node, out_port, in_node, in_port)] = {out_e, handle};
    return true;
  }

  /**
   * @brief Remove a live wire.
   */
  void disconnect(NodeId out_node, PortIdx out_port,
                  NodeId in_node,  PortIdx in_port)
  {
    const uint64_t key = encode(out_node, out_port, in_node, in_port);
    auto it = connection_handles_.find(key);
    if (it == connection_handles_.end()) return;
    it->second.entry->disconnect_fn(it->second.handle);
    connection_handles_.erase(it);
  }

  // ── Simulation ─────────────────────────────────────────────────────────────

  void tick(double timestamp_sec, double dt_sec)
  {
    for (auto& [id, f] : tick_order_)
      f->process(timestamp_sec, dt_sec);
  }

private:

  struct OutPinEntry {
    std::string                           type_id;
    std::function<uint64_t(void*)>        connect_fn;
    std::function<void(uint64_t)>         disconnect_fn;
  };

  struct InPinEntry {
    std::string type_id;
    void*       pin_ptr = nullptr;
  };

  struct ConnHandle {
    OutPinEntry* entry  = nullptr;
    uint64_t     handle = 0;
  };

  OutPinEntry* find_out(NodeId n, PortIdx p)
  {
    auto ni = out_pins_.find(n);
    if (ni == out_pins_.end()) return nullptr;
    auto pi = ni->second.find(p);
    return pi != ni->second.end() ? &pi->second : nullptr;
  }

  InPinEntry* find_in(NodeId n, PortIdx p)
  {
    auto ni = in_pins_.find(n);
    if (ni == in_pins_.end()) return nullptr;
    auto pi = ni->second.find(p);
    return pi != ni->second.end() ? &pi->second : nullptr;
  }

  static uint64_t encode(NodeId on, PortIdx op, NodeId in, PortIdx ip)
  {
    return (uint64_t(on) << 40) | (uint64_t(op) << 28) |
           (uint64_t(in) << 8)  |  uint64_t(ip);
  }

  std::unordered_map<NodeId, std::unique_ptr<filter>>            owned_;
  std::vector<std::pair<NodeId, filter*>>                        tick_order_;
  std::unordered_map<NodeId, std::unordered_map<PortIdx, OutPinEntry>> out_pins_;
  std::unordered_map<NodeId, std::unordered_map<PortIdx, InPinEntry>>  in_pins_;
  std::unordered_map<uint64_t, ConnHandle>                        connection_handles_;
};

//****************************************************************************
} // namespace kineflow::pipeline
//****************************************************************************
