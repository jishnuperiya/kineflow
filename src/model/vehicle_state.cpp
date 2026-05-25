
#include "vehicle_state.hpp"

namespace sentinex::estimation{

std::ostream& operator<<(std::ostream& os, const vehicle_state& s)
{
  os << "VehicleState{"
    << "x=" << s.x
    << ", y=" << s.y
    << ", psi=" << s.psi
    << ", v=" << s.v
    << "}";
  return os;
}

}