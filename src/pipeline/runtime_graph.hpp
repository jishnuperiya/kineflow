#pragma once

#include <string>
#include <functional>
#include <cstdint>
#include <memory>

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

    
};
}