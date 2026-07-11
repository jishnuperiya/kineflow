#pragma once
#include <memory>
#include <map>
#include <cstdint>

namespace kineflow::pipeline
{
template<typename T>
class in_pin
{

  public:
    std::shared_ptr<const T> read()
    {
      return latest_sample_;
    }

    void receive(std::shared_ptr<const T> sample)
    {
      latest_sample_ = sample;
    }
    
    bool has_data()
    {
      return latest_sample_!= nullptr;
    }

  private:
    std::shared_ptr<const T> latest_sample_;
};


template<typename T>
class out_pin
{
  public:
    uint64_t connect(in_pin<T>& in)
    {
      next_handle_++;
      connected_pins_.emplace(next_handle_, &in);
      return next_handle_;
    }

    void disconnect(uint64_t handle)
    {
      connected_pins_.erase(handle);
    }

    void write(std::shared_ptr<const T> sample)
    {
      for(auto& [handle, pin]: connected_pins_)
      {
        pin->receive(sample);
      }
    }
  private:
    std::map<uint64_t,in_pin<T>*> connected_pins_;
    std::uint64_t next_handle_ = 0;
};

} // namespace kineflow::pipeline