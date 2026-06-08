#include <iostream>
#include "hello_filter.hpp"
#include "filter_graph.hpp"

using namespace kineflow::pipeline;
int main()
{
  std::cout << "Hello, kineflow!\n";
  filter_graph graph;
  graph.add(std::make_unique<hello_filter>());
  graph.tick(0.0, 0.1);
  graph.tick(0.1, 0.1);
  graph.tick(0.2, 0.1);

  return 0;
}