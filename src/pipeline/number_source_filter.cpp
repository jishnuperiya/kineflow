
#include<iostream>

#include "filter.hpp"

namespace kineflow::pipeline
{
  struct number_source_filter : filter
  {
    void configure(const nlohmann::json& config)
    {
      if(config.contains("value"))
      {
        m_output_value = config["value"].get<int>();
      }
      m_pins = create_pins(config.at("pins"));
    }
    void process(double timestamp_sec, double dt_sec) override
    {
      std::cout << "number_source filter: outputting value " << m_output_value << std::endl;
    }
  
  private:
    int m_output_value = 0;
    //pin_vec m_pins; moved to base class //todo:new
  };
  
  std::unique_ptr<filter> create_number_source_filter(const nlohmann::json& config)
  {
    auto f = std::make_unique<number_source_filter>();
    f->configure(config);
    return f;
  }


} // namespace kineflow::pipeline
