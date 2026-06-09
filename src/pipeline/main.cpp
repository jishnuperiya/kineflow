#include <iostream>
#include "hello_filter.hpp"
#include "filter_graph.hpp"
#include "dot_source.hpp"
#include "dot_printer.hpp"
#include <memory>

using namespace kineflow::pipeline;
int main()
{
  std::cout << "Hello, kineflow!\n";
  filter_graph graph;

  auto& slow_dot = graph.add(std::make_unique<dot_source>(0.5));
  auto& fast_dot = graph.add(std::make_unique<dot_source>(1.0));
  
  auto& printer1 = graph.add(std::make_unique<dot_printer>());
auto& printer2 = graph.add(std::make_unique<dot_printer>());
slow_dot.out_.connect(printer1.in_);
fast_dot.out_.connect(printer2.in_);
  
  graph.run(5.0, 1.0);
  return 0;
}