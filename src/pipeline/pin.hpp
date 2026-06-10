#pragma once
#include <memory>
#include <vector>

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
    void connect(in_pin<T>& in)
    {
      connected_pins_.push_back(&in);
    }

    void disconnect(in_pin<T>& in)
    {
      std::erase(connected_pins_,&in);
    }

    void write(std::shared_ptr<const T> sample)
    {
      for(auto* pin: connected_pins_)
      {
        pin->receive(sample);
      }
    }
  private:
    std::vector<in_pin<T>*> connected_pins_;
};

} // namespace kineflow::pipeline