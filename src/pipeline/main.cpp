#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "pipeline_parser.hpp"

using namespace kineflow::pipeline;
int main()
{
  std::string s = R"({"name": "Jishnu", "age": 30})";
  std::istringstream stream(s);

  std::ifstream file_stream("/mnt/c/git-repo/kineflow/examples/calculator_pipeline.json");

  if(!file_stream.is_open())
  {
    std::cerr << "Failed to open pipeline JSON file" << std::endl;
    return 1;
  }
  
  nlohmann::json j;
  try
  {
    j = parse_pipeline_json(file_stream);
  }
  catch(const std::runtime_error& e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  std::string test = dump_pipeline(j);
  std::cout << test<< std::endl;
  return 0;
}