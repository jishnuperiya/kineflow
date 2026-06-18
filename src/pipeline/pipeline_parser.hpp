#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace kineflow::pipeline
{

  nlohmann::json parse_pipeline_json(std::istream& json_stream);

  std::string dump_pipeline(const nlohmann::json& tree);

  void print_pipeline(const nlohmann::json& tree);

} // namespace kineflow::pipeline

