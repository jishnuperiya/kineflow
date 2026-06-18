//******** Copyright © 2026 Jishnu Periya. All rights reserved.
//*
//*
//*  Version : $Header:$
//*
//*
//*  Purpose : Parses and validates Kineflow pipeline JSON against the
//*            pipeline Context-Free Grammar.
//*
//*
//****************************************************************************

#include "pipeline_parser.hpp"

#include <iostream>         //For std::cout
#include <set>              //For std::set
#include <stdexcept>        //For std::runtime_error

//****************************************************************************
namespace
{
//****************************************************************************

  //*
  //* Throws a runtime_error with a descriptive pipeline error message.
  //*
  void error(const std::string msg)
  {
      throw std::runtime_error("Pipeline error: " + msg);
  }

  //*
  //* Returns true if the string is a valid C identifier.
  //* A valid identifier starts with a letter or underscore,
  //* followed by letters, digits, or underscores.
  //*
  bool is_valid_identifier(const std::string& s)
  {
      if (s.empty()) return false;
      if (!std::isalpha(static_cast<unsigned char>(s[0])) && s[0] != '_')
          return false;
      for (char c : s)
          if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_')
              return false;
      return true;
  }

  //*
  //* Validates a single pin object against the CFG rule:
  //*   pin ::= { "id": natural, "name": name,
  //*             "direction": direction, "type": data-type }
  //*
  void validate_pin(const nlohmann::json& pin, int filter_id)
  {
      if (!pin.is_object())
          error("filter " + std::to_string(filter_id) + ": pin must be an object");

      if (!pin.contains("id") || !pin["id"].is_number_integer() || pin["id"].get<int>() < 0)
          error("filter " + std::to_string(filter_id) + ": pin must have a non-negative integer 'id'");

      if (!pin.contains("name") || !pin["name"].is_string())
          error("filter " + std::to_string(filter_id) + ": pin must have a string 'name'");

      if (!pin.contains("direction") || !pin["direction"].is_string())
          error("filter " + std::to_string(filter_id) + ": pin must have a string 'direction'");

      const auto dir = pin["direction"].get<std::string>();
      if (dir != "in" && dir != "out")
          error("filter " + std::to_string(filter_id) + ": pin direction must be 'in' or 'out'");

      if (!pin.contains("type") || !pin["type"].is_string())
          error("filter " + std::to_string(filter_id) + ": pin must have a string 'type'");
  }

  //*
  //* Validates the filters array against the CFG rule:
  //*   filter ::= { "id": natural, "type": filter-type, "name": name,
  //*                "properties": [property], "pins": [pin] }
  //*
  //* Also enforces semantic constraints:
  //*   - filter ids must be unique
  //*   - type and name must be valid C identifiers
  //*
  void validate_filters(const nlohmann::json& filters)
  {
      std::set<int> seen_ids;

      for (const auto& f : filters)
      {
          if (!f.is_object())
              error("each filter must be an object");

          if (!f.contains("id") || !f["id"].is_number_integer() || f["id"].get<int>() < 0)
              error("filter must have a non-negative integer 'id'");

          const int id = f["id"].get<int>();

          if (seen_ids.find(id) != seen_ids.end())
              error("duplicate filter id: " + std::to_string(id));
          seen_ids.insert(id);

          if (!f.contains("type") || !f["type"].is_string())
              error("filter " + std::to_string(id) + ": must have a string 'type'");
          if (!is_valid_identifier(f["type"].get<std::string>()))
              error("filter " + std::to_string(id) + ": 'type' must be a valid C identifier");

          if (!f.contains("name") || !f["name"].is_string())
              error("filter " + std::to_string(id) + ": must have a string 'name'");
          if (!is_valid_identifier(f["name"].get<std::string>()))
              error("filter " + std::to_string(id) + ": 'name' must be a valid C identifier");

          if (!f.contains("properties") || !f["properties"].is_array())
              error("filter " + std::to_string(id) + ": must have an array 'properties'");

          if (!f.contains("pins") || !f["pins"].is_array())
              error("filter " + std::to_string(id) + ": must have an array 'pins'");

          for (const auto& pin : f["pins"])
              validate_pin(pin, id);
      }
  }

  //*
  //* Validates the connections array against the CFG rule:
  //*   connection ::= { "source": filter-pin, "target": filter-pin }
  //*   filter-pin ::= { "filter": natural, "pin": natural }
  //*
  //* Also enforces semantic constraint:
  //*   - referenced filter ids must exist
  //*
  void validate_connections(const nlohmann::json& connections, const nlohmann::json& filters)
  {
      std::set<int> filter_ids;
      for (const auto& f : filters)
          filter_ids.insert(f["id"].get<int>());

      for (const auto& conn : connections)
      {
          if (!conn.is_object())
              error("each connection must be an object");

          if (!conn.contains("source"))
              error("connection missing 'source'");
          if (!conn.contains("target"))
              error("connection missing 'target'");

          for (const auto& side : {"source", "target"})
          {
              const auto& ref = conn[side];
              if (!ref.is_object())
                  error(std::string(side) + " must be an object");
              if (!ref.contains("filter") || !ref["filter"].is_number_integer())
                  error(std::string(side) + ": must have an integer 'filter'");
              if (!ref.contains("pin") || !ref["pin"].is_number_integer())
                  error(std::string(side) + ": must have an integer 'pin'");
              if (filter_ids.find(ref["filter"].get<int>()) == filter_ids.end())
                  error(std::string(side) + ": references unknown filter id " + std::to_string(ref["filter"].get<int>()));
          }
      }
  }

//****************************************************************************
} // anonymous namespace
//****************************************************************************

//****************************************************************************
namespace kineflow::pipeline
{
//****************************************************************************

  //*
  //* Parses and validates a pipeline JSON stream against the Kineflow CFG.
  //* Throws std::runtime_error with a descriptive message on any violation.
  //* Returns the validated JSON tree on success.
  //*
  nlohmann::json parse_pipeline_json(std::istream& json_stream)
  {
      nlohmann::json j = nlohmann::json::parse(json_stream);

      if (!j.is_object())
          error("pipeline root must be a JSON object");

      if (!j.contains("filters"))
          error("missing required field 'filters'");

      if (!j["filters"].is_array())
          error("'filters' must be an array");

      if (!j.contains("connections"))
          error("missing required field 'connections'");

      if (!j["connections"].is_array())
          error("'connections' must be an array");

      validate_filters(j["filters"]);
      validate_connections(j["connections"], j["filters"]);

      return j;
  }

  //*
  //* Serializes the pipeline JSON tree back to a formatted string.
  //* Useful for debugging, idempotency testing, and saving to file.
  //* Keys are sorted alphabetically (nlohmann uses std::map internally).
  //*
  std::string dump_pipeline(const nlohmann::json& tree)
  {
      return tree.dump(4);
  }

  //*
  //* Prints the pipeline JSON tree to stdout in human-readable format.
  //*
  void print_pipeline(const nlohmann::json& tree)
  {
      std::cout << dump_pipeline(tree) << std::endl;
  }

  // check if operapotr<< in library 
//****************************************************************************
} // namespace kineflow::pipeline
//****************************************************************************