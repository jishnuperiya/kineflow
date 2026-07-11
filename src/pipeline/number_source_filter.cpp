
#include<iostream>

#include "filter.hpp"


namespace kineflow::pipeline
{
  struct number_source_filter : filter
  {
  // public: no need of public or private

    void configure(const nlohmann::json& config)
    {
      if(config.contains("value"))
      {
        m_output_value = config["value"].get<int>();
      }
      m_pins = create_pins(config["pins"]); // - return vec of pin pr
    }
    void process(double timestamp_sec, double dt_sec) override
    {
      std::cout << "number_source filter: outputting value " << m_output_value << std::endl;
    }
  
  // private:
    int m_output_value = 0;
    pins m_pins; //prob a vec

  };

  std::unique_ptr<filter> create_number_source_filter(const nlohmann::json& config)
  {
    auto f = std::make_unique<number_source_filter>();
    f->configure(config);
    return f;
  }

// todo - o jave tp tp dp tjos
//todo - move to cpp file
  // current - deafult ctor + configure 


} // namespace kineflow::pipeline
