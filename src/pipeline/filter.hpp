#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <functional>
#include <unordered_map>
#include <string_view>

namespace kineflow::pipeline
{
  
  //todo - clean up the pin struct
  
  struct pin
  {
    enum class direction
    {
    in,
    out
    };

    enum class type
    {
      integer,
      real
    };

    
    const int id;
    const std::string name;
    const direction dir;
    const type pin_type;
  };

  struct filter
  {
    virtual ~filter() = default;
    virtual void process(double timestamp_sec, double dt) = 0;

    //todo: get pins-- note: span cant have unique pinter. so may be vector? or can i treally own ? may be an observer pattern? learn this
    //virtual std::span<pin_ptr> get_pins() const noexcept = 0; //pins - someting iterator based - range , span etc 
    
    
    //note for later
    /*
    perhaps get_input_pin(name)
    get output pin
    then no need to expose shape of pin arrray
    */
  };


  using filter_factory_function = std::function<std::unique_ptr<filter>(const nlohmann::json&)>;
  

  using filter_ptr = std::unique_ptr<filter>;
  using pin_ptr = std::unique_ptr<pin>;
  using pin_vec = std::vector<pin_ptr>;

  filter_ptr create_filter(const nlohmann::json& config);
  pin_ptr create_pin(const nlohmann::json& config);
  pin_vec create_pins(const nlohmann::json& config);

}