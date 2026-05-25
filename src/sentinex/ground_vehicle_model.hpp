//******** Copyright © 2026 Jishnu Periya. All rights reserved.
//*
//*
//*  Version : $Header:$
//*
//*
//*  Purpose : Ground vehicle platform backend for SENTINEX.
//*            Wraps the kinematic bicycle model behind the platform_model
//*            interface. First concrete backend — no external dependencies.
//*
//*  See Also: platform_model.hpp  — abstract interface this implements
//*            motion_model.hpp    — bicycle model physics
//*            vehicle_state.hpp   — internal state representation
//*
//*
//****************************************************************************
#pragma once
//****************************************************************************

#include <cmath>        // for std::cos, std::fmod
#include <numbers>      // for std::numbers::pi
#include <string>       // for std::string
#include <vector>       // for std::vector

#include "platform_model.hpp"
#include "../model/motion_model.hpp"
#include "../model/vehicle_state.hpp"
#include "../model/motion_command.hpp"

//****************************************************************************

namespace sentinex::platform
{

//****************************************************************************

/**
 * Ground vehicle backend — kinematic bicycle model.
 *
 * Converts local Cartesian state [m, rad] to WGS84 telemetry_sample
 * [deg] using a flat-earth approximation valid within ~100 km of origin.
 *
 * @see platform_model      Abstract interface this implements
 * @see motion_model        Bicycle model physics
 * @see helicopter_model    JSBSim backend (Phase 1)
 */
class ground_vehicle_model final : public platform_model
{
public:

  /**
   * @param wheelbase  Front-to-rear axle distance [m]. Default: 4.0 m.
   */
  explicit ground_vehicle_model(double wheelbase = 4.0)
    : model_(wheelbase)
  {}

  /**
   * Initialise vehicle from scenario parameters.
   * Converts WGS84 origin to local frame, sets initial state.
   */
  void initialise(const scenario_params& params) override
  {
    origin_lat_deg_ = params.latitude_deg;
    origin_lon_deg_ = params.longitude_deg;

    // Navigation heading [deg CW from N] → mathematical [rad CCW from E]
    const double heading_rad =
      (90.0 - params.heading_deg) * std::numbers::pi / 180.0;

    state_.x   = 0.0;
    state_.y   = 0.0;
    state_.psi = heading_rad;
    state_.v   = params.speed_ms;

    speed_ms_      = params.speed_ms;
    timestamp_sec_ = 0.0;
    active_failures_.clear();
  }

  /**
   * Advance bicycle model by dt_sec seconds.
   * @param dt_sec  Time step [s]. Must be > 0.
   */
  void step(double dt_sec) override
  {
    model::motion_command cmd;
    cmd.velocity_cmd = speed_ms_;
    cmd.steering_cmd = 0.0;         // straight ahead — autopilot in Phase 2

    state_ = model_.propagate(state_, cmd, dt_sec);
    timestamp_sec_ += dt_sec;
  }

  /**
   * Read current state as a telemetry_sample.
   * Converts local Cartesian → WGS84, radians → degrees.
   */
  telemetry::telemetry_sample read() const override
  {
    telemetry::telemetry_sample s;

    s.timestamp_sec  = timestamp_sec_;
    s.latitude_deg   = local_to_lat(state_.y);
    s.longitude_deg  = local_to_lon(state_.x);
    s.altitude_m     = 0.0;
    s.heading_deg    = math_to_nav_heading(state_.psi);
    s.speed_ms       = state_.v;
    s.pitch_deg      = 0.0;
    s.roll_deg       = 0.0;
    s.yaw_rate_dps   = 0.0;

    s.steering_angle_deg = 0.0;
    s.wheel_speed_rps    =
      state_.v / (2.0 * std::numbers::pi * wheel_radius_m_);

    s.mission_phase    = flight_phase_;
    s.waypoint_reached = false;
    s.active_failures  = active_failures_;

    return s;
  }

  /**
   * Inject a failure into the vehicle.
   * Records failure ID in active_failures_ for all subsequent read() calls.
   */
  void inject(const failure_event& event) override
  {
    active_failures_.push_back(event.system);

    if (event.system == "engine") {
      if (event.severity == "full")    speed_ms_ = 0.0;
      else if (event.severity == "partial") speed_ms_ *= 0.5;
    }
  }

  /** @return "ground_vehicle" */
  std::string platform_type() const override
  {
    return "ground_vehicle";
  }

private:

  /** Local y [m] → WGS84 latitude [deg]. Flat-earth approximation. */
  double local_to_lat(double y_m) const
  {
    constexpr double Re = 6'371'000.0;
    return origin_lat_deg_ + (y_m / Re) * (180.0 / std::numbers::pi);
  }

  /** Local x [m] → WGS84 longitude [deg]. Flat-earth approximation. */
  double local_to_lon(double x_m) const
  {
    constexpr double Re  = 6'371'000.0;
    const double lat_rad = origin_lat_deg_ * std::numbers::pi / 180.0;
    return origin_lon_deg_ +
      (x_m / (Re * std::cos(lat_rad))) * (180.0 / std::numbers::pi);
  }

  /**
   * Mathematical heading [rad CCW from E] → navigation heading [deg CW from N].
   * Output range: [0, 360).
   */
  static double math_to_nav_heading(double psi_math)
  {
    double deg = 90.0 - psi_math * (180.0 / std::numbers::pi);
    deg = std::fmod(deg, 360.0);
    if (deg < 0.0) deg += 360.0;
    return deg;
  }

  model::motion_model           model_;              ///< Bicycle model physics
  estimation::vehicle_state     state_;              ///< Current local Cartesian state
  double                        origin_lat_deg_ = 47.6710; ///< WGS84 origin latitude (Friedrichshafen)
  double                        origin_lon_deg_ = 9.5115;  ///< WGS84 origin longitude (Friedrichshafen) 
  double                        speed_ms_       = 0.0;     ///< Commanded speed [m/s]
  double                        timestamp_sec_  = 0.0;     ///< Mission elapsed time [s]
  std::string                   flight_phase_   = "cruise";///< Current mission phase
  std::vector<std::string>      active_failures_;           ///< Active failure IDs

  static constexpr double wheel_radius_m_ = 0.5; ///< Assumed wheel radius [m]
};

//****************************************************************************
} // namespace sentinex::platform
//****************************************************************************