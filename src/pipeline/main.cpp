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

  auto& source = graph.add(std::make_unique<dot_source>());
  auto& printer = graph.add(std::make_unique<dot_printer>());
  source.out_.connect(printer.in_);
  graph.run(5.0, 1.0);
  return 0;
}