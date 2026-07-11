
#include<iostream>

#include "filter.hpp"

//todo : move the filter to only cpp. no header needed
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
      //todoit create and store poins from the config json  
      //m_pins = create_pins(config["pins"]); // - return vec of pin pr
    }
    void process(double timestamp_sec, double dt_sec) override
    {
      std::cout << "number_source filter: outputting value " << m_output_value << std::endl;
    }
  
  // private:
    int m_output_value = 0;
    //todo: the filter owns the pins - probably a vector of unique_ptrs to pins?
    // pin_vec m_pins;

  };
  
  std::unique_ptr<filter> create_number_source_filter(const nlohmann::json& config)
  {
    auto f = std::make_unique<number_source_filter>();
    f->configure(config);
    return f;
  }


} // namespace kineflow::pipeline
