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

#include "motion_model.hpp"
//****************************************************************************
namespace sentinex::model
{
  //****************************************************************************

    /**
    * @brief Propagates the vehicle state forward in time using a kinematic
    *        bicycle model.
    *
    * The model assumes constant longitudinal velocity and steering angle
    * over the time step and neglects dynamic effects such as tire slip,
    * roll, or pitch.
    *
    * State update:
    *  - x, y  : global position [m]
    *  - psi   : heading angle [rad]
    *  - v     : longitudinal velocity [m/s]
    *
    * @param
    * @param dt                   Integration time step [s], must be > 0
    * @param motion_command       Longitudinal velocity command [m/s] & Front wheel steering angle [rad]
    * @param vehicle_state        Vehicle state
    */
  estimation::vehicle_state model::motion_model::propagate(const estimation::vehicle_state& state, const motion_command& cmd, double dt) const
  {
    assert(dt >= 0.0 && "Time step must be positive");
    assert(std::isfinite(cmd.velocity_cmd) && "Velocity must be finite");
    assert(std::isfinite(cmd.steering_cmd) && "Steering angle must be finite");

    estimation::vehicle_state next_state = state;

    // Slip angle at the vehicle center of mass
    const double beta = std::atan(0.5 * std::tan(cmd.steering_cmd));

    next_state.x += cmd.velocity_cmd * std::cos(state.psi + beta) * dt;
    next_state.y += cmd.velocity_cmd * std::sin(state.psi + beta) * dt;
    next_state.psi += (cmd.velocity_cmd / wheelbase_) * std::sin(beta) * dt;

    next_state.psi = normalize_angle(state.psi);

    next_state.v = cmd.velocity_cmd;

    return next_state;
  }

  /**
  * Normalize angle to [-π, π]
  *
  * @param angle Input angle [rad]
  * @return Normalized angle [rad]
  */

  
  double model::motion_model::normalize_angle(double angle)
  {
    return std::atan2(std::sin(angle), std::cos(angle));
  }
}