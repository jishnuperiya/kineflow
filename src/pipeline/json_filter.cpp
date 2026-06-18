#include "filter.hpp"

template<std::regular cpp_type>
struct type_of : type
{

  std::unique_ptr<value> new_value(std::string_view s) const override
  {
    cpp_type v;
    if(std::istringstream(s) >>v )
    return std::make_unique<cpp_type>(v);
    throw std::runtime_error("bad string value"); // failure heper function that take varaidic arugments and format string to nice error
  }

};

template<std::regular v>
struct value_of : value
{
  value_of(const v& value_x) : v_{value_x}{}
  v v_;
  virtual void insert(std::ostream& o) const 
  {
    o<<v_;
  }
};

using name = std::string;

const std::map<std::string_view, type_ptr> the_types 
{
  {"int",    std::make_unique<type_of<int>>()} , 
  {"double", std::make_unique<type_of<double>>()}
};

value_ptr parse(std::string_view type, std::string_view s)
{
  auto i = the_types.find(type);
  if (i == the_types.end()) throw std::runtime_error("bad type");
  return i->second->new_value(s);
}

struct my_filter : json_filter
{
  my_filter(std::span<const value_ptr>);

  void process(double, double) override;
  double a_;
  int b_;
  /* data */
};

// using type_factory = std::function<std::unique_

// extern std::map<std::stritypeng_view, std::function<std::unique_ptr<json_filter>(...)>> filter_map;
// extern std::map<std::string_view, std::function<std::unique_ptr<property_type>(...)>> property_type_map;
// extern std::map<std::string_view, std::function<std::unique_ptr<conn_type>(...)>> conn_type_map;
