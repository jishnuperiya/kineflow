#include <memory>


using namespace kineflow::pipeline;

namespace{
    

}

std::unique_ptr<kineflow::pipeline::number_source_filter> create_number_source_filter(const json_tree& config)
{
struct number_source_filter : filter
{

  number_source_filter(int value) : m_output_value(value) {}
  
    void process(double timestamp_sec, double dt_sec) override
  {
    // implement later
  }

  const int m_output_value = 0;
};


    assert(config.is_object() & config["type"].get<std::string>() == "number_source");

    if(config.has("value") && config["value"].is_number_integer())
    {
        int value = config["value"].get<int>();
        return std::make_unique<kineflow::pipeline::number_source_filter>(value);
    }
    else
    {
        throw std::runtime_error("number_source filter must have an integer 'value' property");
    }
    
}

