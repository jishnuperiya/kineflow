//******** Copyright   2026 Jishnu Periya. All rights reserved.
//*
//*
//*  Version : $Header:$
//*
//*
//*  Purpose : Implements a simplified kinematic bicycle model used to propagate
//*            a vehicle state forward in time based on steering and velocity commands.
//*
//*
//****************************************************************************
#pragma once
//****************************************************************************

#include <cassert>                    // for assert
#include <cmath>                      // for std::atan, std::tan, std::cos, std::sin, std::isfinite
#include "vehicle_state.hpp"          // for kineflow::estimation::vehicle_state
#include "motion_command.hpp"         // for kineflow::model::motion_command

//****************************************************************************
namespace kineflow::model{
//****************************************************************************

 /**
 * Represents a stateless mathematical model for planar vehicle motion.
 *
 */
  class bicycle_model
  {
  public:
    /**
     * Construct a bicycle model.
     *
     * @param wheelbase Distance between front and rear axle [m]
     */
    explicit bicycle_model(double wheelbase = 4.0)
      : wheelbase_(wheelbase)
    {
    }

    bicycle_model(const bicycle_model&) = default;
    bicycle_model& operator=(const bicycle_model&) = delete;
    bicycle_model(bicycle_model&&) = delete;
    bicycle_model& operator=(bicycle_model&& ) = delete;

    /**
    * @brief Propagates the vehicle state forward in time using a kinematic
    *        bicycle model.
    */
    kineflow::estimation::vehicle_state propagate(const kineflow::estimation::vehicle_state& state, const motion_command& cmd, double dt) const;

  private:
    /**
    * Normalize angle to [-π, π]
    *
    * @param angle Input angle [rad]
    * @return Normalized angle [rad]
    */
    static double normalize_angle(double angle);
    

    //estimation::vehicle_state state_;
    const double wheelbase_;
  };
}


/*
takes non trivial funtions out of loine
get state inline -cheapter

*/