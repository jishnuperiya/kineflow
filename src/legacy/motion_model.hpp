//******** Copyright   2026 Jishnu Periya, Jonathon Bell. All rights reserved.
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
#include "vehicle_state.hpp"          // for sentinex::estimation::VehicleState
#include "motion_command.hpp"         // for sentinex::model::MotionCommands

//****************************************************************************
namespace sentinex::model{
//****************************************************************************

 /**
 * Represents a stateless mathematical model for planar vehicle motion.
 *
 */
  class motion_model
  {
  public:
    /**
     * Construct a bicycle model.
     *
     * @param wheelbase Distance between front and rear axle [m]
     */
    explicit motion_model(double wheelbase = 4.0)
      : wheelbase_(wheelbase)
    {
    }

    motion_model(const motion_model&) = default;
    motion_model& operator=(const motion_model&) = delete;
    motion_model(motion_model&&) = delete;
    motion_model& operator=(motion_model&& ) = delete;

    /**
    * @brief Propagates the vehicle state forward in time using a kinematic
    *        bicycle model.
    */
    estimation::vehicle_state propagate(const estimation::vehicle_state& state, const motion_command& cmd, double dt) const;

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