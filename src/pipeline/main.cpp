#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

#include "filter.hpp"
#include "pipeline_parser.hpp"

int main()
{
  using namespace kineflow::pipeline;

  // if(std::ifstream file_stream("/mnt/c/git-repo/kineflow/examples/calculator_pipeline.json");file_stream.is_open())
//another way- helper fun
// try
// {
//   {/* code */}
// }
// catch(const std::exception& e)
// {
//   std::cerr << e.what() << '\n';
// }

  std::ifstream file_stream("/mnt/c/git-repo/kineflow/examples/calculator_pipeline.json");
  
  if (!file_stream.is_open())
  {
    std::cerr << "Failed to open pipeline JSON file" << std::endl;
    return 1;
  }

  nlohmann::json tree;
  try
  {
    tree = parse_pipeline_json(file_stream);
  }
  catch (const std::exception& e)
  {
    std::cerr << "Pipeline parse failed: " << e.what() << std::endl;
    return 1;
  }

  std::cout << dump_pipeline(tree) << std::endl;

  if (!tree.contains("filters") || !tree["filters"].is_array())
  {
    std::cerr << "Pipeline must contain an array field named filters" << std::endl;
    return 1;
  }
  
  std::vector<std::unique_ptr<filter>> filters;
  filters.reserve(tree["filters"].size());

  for (const auto& filter_json : tree["filters"])
  {
    std::cout << filter_json["type"] << "\n";
    filters.push_back(filter_factory::create_filter(filter_json));
  }

  /*
  go back over the tree filling in the connections
  */
 
  for (const auto& f : filters)
  {
    f->process(0.0, 0.1);
  }
  

  return 0;
}