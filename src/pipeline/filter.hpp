#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <functional>
#include <unordered_map>
#include <string_view>

namespace kineflow::pipeline
{
  struct pin
  {
    enum class direction { in, out };
    enum class type      { integer, real };

    const int id;
    const std::string name;
    const direction dir;
    const type pin_type;
  };

  using pin_ptr = std::unique_ptr<pin>;
  using pin_vec = std::vector<pin_ptr>;

  struct filter
  {
    virtual ~filter() = default;
    virtual void process(double timestamp_sec, double dt) = 0;

    [[nodiscard]] const pin* get_pin(const int id) const noexcept  //todo:new
    {
      for (const auto& pin : m_pins) {
        if (pin->id == id) return pin.get();
      }
      return nullptr;
    }
    protected:
      pin_vec m_pins;    //todo:new
  };


  using filter_factory_function = std::function<std::unique_ptr<filter>(const nlohmann::json&)>;
  using filter_ptr = std::unique_ptr<filter>;

  filter_ptr create_filter(const nlohmann::json& config);
  pin_ptr create_pin(const nlohmann::json& config);
  pin_vec create_pins(const nlohmann::json& config);

}