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

#include "vehicle_state.hpp"          // for sentinex::estimation::VehicleState
#include "motion_command.hpp"         // for sentinex::model::MotionCommands
#include "motion_model.hpp"           // for sentinex::model::motion_model

//****************************************************************************
namespace sentinex::model {
//****************************************************************************
class SimulatedVehicle
{
public:


  /**
   * Reset the vehicle state.
   *
   * @param state New vehicle state
   */
  void reset(const estimation::vehicle_state& s)
  {
    state_ = s;
  }

  /**
  *@brief Propagates the vehicle state forward in time using a kinematic
  *        bicycle model.
  */
  void update(const model::motion_command& cmd, double dt)
  {
    state_ = model_.propagate(state_, cmd, dt);
  }

 /**
  * Get the current vehicle state.
  *
  * @return Current vehicle state
  */
  VehicleState getState() const
  {
    return state_;
  }

private:
  estimation::motion_model model_;
  estimation::vehicle_state state_;
};
