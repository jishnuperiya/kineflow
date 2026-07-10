
#include "filter.hpp"
#include <iostream>

namespace kineflow::pipeline
{
  class multiply_filter : public filter
  {
  public:

    void configure(const nlohmann::json& config)
    {
      if(config.contains("value"))
      {
        m_multiplication_factor = config["value"].get<int>();
      }
    }
    void process(double timestamp_sec, double dt_sec) override
    {
      std::cout << "multiply filter: multiplying by " << m_multiplication_factor << std::endl;
    }
  
  private:
    int m_multiplication_factor = 0;

  };

  std::unique_ptr<filter> create_multiply_filter(const nlohmann::json& config)
  {
    auto f = std::make_unique<multiply_filter>();
    f->configure(config);
    return f;
  }
} // namespace kineflow::pipeline
