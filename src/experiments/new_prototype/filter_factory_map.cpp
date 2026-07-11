#include "filter.hpp"
#include <map>

namespace kineflow::pipeline
{
  extern std::unique_ptr<filter> create_number_source_filter(const json& config);

    const map<std::string_view, filter_factory_function> filter_factory_map
    {
        {"number_source", create_number_source_filter}
    };
}
