//******** Copyright � 2026 Jishnu Periya, Jonathon Bell. All rights reserved.
//*
//*
//*  Version : $Header:$
//*
//*
//*  Purpose : Implementation for class `harmony::pitch`.
//*
//*
//****************************************************************************

#include <fstream>                  //for std::ifstream
#include <iostream>                 //for std::cout      
#include <memory>                   //for std::unique_ptr
#include <vector>                   //for std::vector
#include <unordered_map>

#include "filter.hpp"               //for kineflow::pipeline::filter
#include "pipeline_parser.hpp"      //for kineflow::pipeline::parse_pipeline_json, kineflow::pipeline::dump_pipeline

int main()
{
  using namespace kineflow::pipeline;

  //todo: learn later

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

  std::ifstream file_stream("C:/Users/jishn/CLionProjects/kineflow/examples/calculator_pipeline.json");

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

  if (!tree.contains("filters") || !tree["filters"].is_array())
  {
    std::cerr << "Pipeline must contain an array field named filters" << std::endl;
    return 1;
  }
  
  std::vector<std::unique_ptr<filter>> filters;
  std::unordered_map<int, filter*> filters_by_id; //todo:new
  filters.reserve(tree["filters"].size());

  for (const auto& filter_json : tree["filters"])
  {
    filters.push_back(create_filter(filter_json));
    filters_by_id[filter_json["id"].get<int>()] = filters.back().get(); //todo:new
  }




  struct connection //todo: new
  {
    const filter* source_filter;
    const pin* source_pin;
    const filter* target_filter;
    const pin* target_pin;
  };
  std::vector<connection> connections;

  for (const auto& connection_json : tree["connections"]) //todo:new
  {
    const int source_filter_id = connection_json["source"]["filter"].get<int>();
    const int source_pin_id    = connection_json["source"]["pin"].get<int>();
    const int target_filter_id = connection_json["target"]["filter"].get<int>();
    const int target_pin_id    = connection_json["target"]["pin"].get<int>();

    const filter* source_filter = filters_by_id.at(source_filter_id);
    const filter* target_filter = filters_by_id.at(target_filter_id);

    const pin* source_pin = source_filter->get_pin(source_pin_id);
    const pin* target_pin = target_filter->get_pin(target_pin_id);

    connections.push_back(connection{source_filter, source_pin, target_filter, target_pin});

  }

/*
 * connection in json:
  * "connections": [
    {
      "source": {"filter": 0, "pin": 0},
      "target": {"filter": 1, "pin": 0}
    }
  ]
 */


 
  for (const auto& f : filters)
  {
    f->process(0.0, 0.1);
  }
  

  return 0;
}