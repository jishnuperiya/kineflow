//******** Copyright © 2026 Jishnu Periya. All rights reserved.
//*
//*
//*  Version : $Header:$
//*
//*
//*  Purpose : Typed sample definitions for the kineflow pipeline framework.
//*            Each sample type carries one logical stream of data with a
//*            timestamp. Replaces the monolithic telemetry_sample struct.
//*
//*            Stream layout
//*            ─────────────
//*            pose_sample     — position + heading (all platforms)
//*            dynamics_sample — speed, angles, rates (all platforms)
//*            vehicle_sample  — ground vehicle specifics
//*            event_sample    — mission phase, failures, waypoints
//*
//*  See Also: pin.hpp          — in_pin<T> / out_pin<T> that carry these
//*            filter.hpp       — filters that produce/consume these
//*
//*
//****************************************************************************
#pragma once
//****************************************************************************

#include <string>
#include <vector>

//****************************************************************************

namespace kineflow::pipeline
{

//****************************************************************************

/**
 * @brief Base for all pipeline samples.
 * Every concrete sample type inherits from this and adds its own fields.
 */
struct sample
{
  double timestamp_sec = 0.0;   ///< Mission elapsed time [s]
  virtual ~sample() = default;
};

//****************************************************************************

/**
 * @brief Position and heading — produced by every platform backend.
 */
struct pose_sample : sample
{
  double latitude_deg  = 0.0;   ///< WGS84 latitude  [deg]
  double longitude_deg = 0.0;   ///< WGS84 longitude [deg]
  double altitude_m    = 0.0;   ///< Altitude AGL    [m]
  double heading_deg   = 0.0;   ///< True heading    [deg, 0-360 CW from N]
};

//****************************************************************************

/**
 * @brief Rigid-body dynamics — produced by every platform backend.
 */
struct dynamics_sample : sample
{
  double speed_ms     = 0.0;    ///< Speed over ground [m/s]
  double pitch_deg    = 0.0;    ///< Pitch angle        [deg, +nose-up]
  double roll_deg     = 0.0;    ///< Roll angle         [deg, +right-wing-down]
  double yaw_rate_dps = 0.0;    ///< Yaw rate           [deg/s, +CW from above]
};

//****************************************************************************

/**
 * @brief Ground vehicle-specific channels.
 * Emitted by ground_vehicle_source; ignored by other platform filters.
 */
struct vehicle_sample : sample
{
  double steering_angle_deg = 0.0;  ///< Front wheel steer [deg, +left]
  double wheel_speed_rps    = 0.0;  ///< Wheel speed        [rev/s]
};

//****************************************************************************

/**
 * @brief Local Cartesian position — produced by local_frame_filter.
 * Converts WGS84 lat/lon to East/North metres from the scenario origin
 * using a flat-earth approximation (valid within ~100 km of origin).
 */
struct local_pose_sample : sample
{
  double x_m        = 0.0;   ///< East  displacement from origin [m]
  double y_m        = 0.0;   ///< North displacement from origin [m]
  double heading_deg = 0.0;  ///< True heading [deg, 0-360 CW from N]
};

//****************************************************************************

/**
 * @brief Mission context — set by the simulation loop, not the physics model.
 */
struct event_sample : sample
{
  std::string              mission_phase;         ///< "cruise", "approach", etc.
  bool                     waypoint_reached = false;
  std::vector<std::string> active_failures;       ///< Empty = nominal
};

//****************************************************************************
} // namespace kineflow::pipeline
//****************************************************************************
