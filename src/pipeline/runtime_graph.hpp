#pragma once

#include <string>
#include <functional>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include "pin.hpp"
#include "filter.hpp"

namespace kineflow::pipeline{

class runtime_graph
{
  public:
    using NodeId  = unsigned int;
    using PortIdx = unsigned int; 
  
    void add_filter(NodeId node_id, std::unique_ptr<filter> f)
    {
      filter* raw = f.get();
      //add filter to owned
      owned_.emplace(node_id,std::move(f));
      
      //add to tick order
      tick_order_.push_back({node_id,raw});
    }
    
    template<typename T>
    void register_out_pin(NodeId node_id, PortIdx port,
                      std::string type_id, out_pin<T>* pin)
    {
      OutPinEntry e;
      e.type_id    = std::move(type_id);
      
      e.connect_fn = [pin](void* in_ptr) -> uint64_t
      {
        auto typed_in_pin = static_cast<in_pin<T>*>(in_ptr);
        return pin->connect(*typed_in_pin); //calls out_pin<T>::connect
      };

      e.disconnect_fn = [pin](uint64_t handle)
      {
        pin->disconnect(handle); //calls out_pin<T>::disconnect
      };

      out_pins_[node_id][port] = std::move(e); // copy/move the entry into the map. move is more efficient since it contains std::function which can be expensive to copy

    }

    template<typename T>
    void register_in_pin(NodeId node_id, PortIdx port,
                     std::string type_id, in_pin<T>* pin)
    {
      InPinEntry e;
      e.type_id = std::move(type_id);
      e.pin_ptr = pin;

      in_pins_[node_id][port] = std::move(e);
    }

    void remove_filter(NodeId node_id)
    {
      owned_.erase(node_id);

      std::erase_if(tick_order_, [node_id](const std::pair<NodeId,filter*>& p)
      {
        return p.first == node_id;
      }); 
    }

    bool connect(NodeId out_node, PortIdx out_port, NodeId in_node, PortIdx in_port)
    {
      auto& out_e = out_pins_[out_node][out_port];
      auto& in_e  = in_pins_[in_node][in_port];

      if(out_e.type_id != in_e.type_id)
          return false;

      const uint64_t handle = out_e.connect_fn(in_e.pin_ptr);
      const uint64_t key    = encode(out_node, out_port, in_node, in_port);
      connection_handles_[key] = {&out_e, handle};
      return true;
    }

    void disconnect(NodeId out_node, PortIdx out_port, NodeId in_node, PortIdx in_port)
    {
        const uint64_t key = encode(out_node, out_port, in_node, in_port);
        auto it = connection_handles_.find(key);
        if(it == connection_handles_.end()) return;  
        it->second.entry->disconnect_fn(it->second.handle);
        connection_handles_.erase(it);
    }
    
    void tick(double timestamp_sec, double dt_sec)
    {
      for(auto& p: tick_order_)
      {
        p.second->process(timestamp_sec,dt_sec);
      }
    }

  private:
    struct OutPinEntry
    {
      std::string type_id;
      std::function<uint64_t(void*)> connect_fn;
      std::function<void(uint64_t)> disconnect_fn;
    };

    struct InPinEntry
    {
      std::string type_id;
      void* pin_ptr = nullptr;
    };

    struct ConnHandle 
    {
      OutPinEntry* entry  = nullptr;
      uint64_t     handle = 0;
    };


    // Connection key encoding (16 bits each):
    // bits 63-48: out_node
    // bits 47-32: out_port  
    // bits 31-16: in_node
    // bits 15-0:  in_port
    static uint64_t encode(NodeId on, PortIdx op, NodeId in, PortIdx ip)
    {
        return (uint64_t(on) << 48) | (uint64_t(op) << 32) |
              (uint64_t(in) << 16) |  uint64_t(ip);
    }
   
    std::unordered_map<NodeId, std::unique_ptr<filter>> owned_;
    std::vector<std::pair<NodeId,filter*>> tick_order_; //filter* instead of unique_ptr- you dont want to own. just access
    std::unordered_map<NodeId, std::unordered_map<PortIdx, OutPinEntry>> out_pins_;
    std::unordered_map<NodeId, std::unordered_map<PortIdx, InPinEntry>>  in_pins_;     
    std::unordered_map<uint64_t, ConnHandle> connection_handles_; // maps encoded connection key to out_pin handle for disconnection 
};
}