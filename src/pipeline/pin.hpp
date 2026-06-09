#pragma once
#include <memory>

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
      connected_pin_ = &in;
    }
    void disconnect()
    {
      connected_pin_ = nullptr;
    }
    void write(std::shared_ptr<const T> sample)
    {
      if (connected_pin_)
        connected_pin_->receive(sample);
    }
  private:
    in_pin<T>* connected_pin_ = nullptr;        
};
} // namespace kineflow::pipeline