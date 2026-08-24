//******** Copyright � 2026 Jishnu Periya. All rights reserved.
//*
//*
//*  Version : $Header:$
//*
//*
//*  Purpose : Represents the vehicle state used for Kalman filtering.
//*
//*
//****************************************************************************
#pragma once
//****************************************************************************

#include <compare>    // for operator <==>
#include <ostream>    // for ostream

//****************************************************************************


namespace kineflow::estimation
{
  //****************************************************************************

  /**
   * Represents the state of a vehicle for state estimation.
   *
   * The state is expressed in a global/world reference frame and follows
   * a fixed ordering which must be consistent with the associated
   * covariance matrix.
   *
 * State vector:
 * \f[
 * \mathbf{x} = [x, y, \psi, v]^T
 * \f]
   *
   * Units:
   *  - x, y : meters
   *  - psi  : radians
   *  - v    : meters per second
   */
  struct vehicle_state
  {
    /** Position along world X-axis [m] */
    double x = 0.0;

    /** Position along world Y-axis [m] */
    double y = 0.0;

    /** Heading / yaw angle [rad] */
    double psi = 0.0;

    /** Longitudinal velocity [m/s] */
    double v = 0.0; // quaternion class -> unit q repreenting curr dir. 

    auto operator<=>(const vehicle_state&) const = default;
  };

  std::ostream& operator<<(std::ostream& os, const vehicle_state& s);


  //****************************************************************************
} // namespace kineflow::estimation
//****************************************************************************


