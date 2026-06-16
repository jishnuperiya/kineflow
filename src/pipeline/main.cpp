#include <iostream>
#include "hello_filter.hpp"
#include "filter_graph.hpp"
#include "dot_source.hpp"
#include "dot_printer.hpp"
#include <memory>
#include "runtime_graph.hpp"
#include <string>
#include "nlohmann/json.hpp"

using namespace kineflow::pipeline;
int main()
{
  std::cout << "Hello, kineflow!\n";

  // static graph construction
  filter_graph graph;

  std::cout << "static graph\n";


  auto& slow_dot = graph.add(std::make_unique<dot_source>(0.5));
  auto& fast_dot = graph.add(std::make_unique<dot_source>(1.0));
  //auto& string_f = graph.add(std::make_unique<std::string>());
  
  auto& printer1 = graph.add2(std::make_unique<dot_printer>());
  auto& printer2 = graph.add2(std::make_unique<dot_printer>());


  // runtime graph construction

  // runtime_graph rgraph;
  // std::cout << "runtime graph\n";
  // auto* src_filter = new dot_source(1.0);
  // auto* prn_filter = new dot_printer();

  // rgraph.add_filter(1, std::unique_ptr<filter>(src_filter));
  // rgraph.add_filter(2, std::unique_ptr<filter>(prn_filter));

  // rgraph.register_out_pin(1, 0, "dot_sample", &src_filter->out_);
  // rgraph.register_in_pin(2, 0, "dot_sample", &prn_filter->in_);

  // rgraph.connect(1, 0, 2, 0);

  // rgraph.tick(0.0, 1.0);

      // Test nlohmann/json
    auto j = nlohmann::json::parse(R"({"hello": "kineflow", "version": 1})");
    
    std::cout << "JSON works!\n";
    std::cout << "hello: " << j["hello"] << "\n";
    std::cout << "version: " << j["version"] << "\n";
  return 0;
}