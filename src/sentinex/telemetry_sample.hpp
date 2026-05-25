//******** Copyright © 2026 Jishnu Periya. All rights reserved.
//*
//*
//*  Version : $Header:$
//*
//*
//*  Purpose : Universal telemetry data contract for SENTINEX.
//*            Produced by every platform backend via platform_model::read().
//*            Consumed by the telemetry pipeline, query engine, and report
//*            generator. Platform-specific fields use std::optional — empty
//*            when not relevant to the current platform.
//*
//*  See Also: platform_model.hpp  — produces this struct via read() // read() or extract()?
//*            telemetry_writer.hpp — serialises this struct to XML
//*            query_engine        — consumes this struct for analysis
//*
//*
//****************************************************************************
#pragma once
//****************************************************************************

#include <optional>   // for std::optional
#include <string>     // for std::string
#include <vector>     // for std::vector

//****************************************************************************

namespace sentinex::telemetry
{

//****************************************************************************

/**
 * @brief Universal telemetry sample — one record per simulation step.
 *
 * All platform backends produce this struct via platform_model::read() or extract()?
 * Platform-specific fields use std::optional — empty when not applicable.
 *
 * @see platform_model   Produces this struct via read() or extract()
 * @see telemetry_writer Serialises this struct to XML
 */
struct telemetry_sample
{
  //**************************************************************************
  /// @name Universal Fields — populated by every platform backend.
  //**************************************************************************
  ///@{

  /** Mission elapsed time [s]. Monotonically increasing from 0. */
  double timestamp_sec = 0.0;

  /** WGS84 geodetic latitude [deg]. Range: [-90, 90]. Positive north. */
  double latitude_deg = 0.0;

  /** WGS84 geodetic longitude [deg]. Range: [-180, 180]. Positive east. */
  double longitude_deg = 0.0;

  /**
   * Altitude above ground level [m].
   * Air platforms: AGL from radar altimeter model.
   * Ground vehicles: terrain height (typically 0).
   * Naval vessels: 0 (surface).
   */
  double altitude_m = 0.0;

  /** True heading [deg]. Range: [0, 360). Clockwise from true north. */
  double heading_deg = 0.0;

  /** Speed over ground [m/s]. */
  double speed_ms = 0.0;

  /** Pitch angle [deg]. Positive nose-up. */
  double pitch_deg = 0.0;

  /** Roll angle [deg]. Positive right-wing-down. */
  double roll_deg = 0.0;

  /** Yaw rate [deg/s]. Positive clockwise viewed from above. */
  double yaw_rate_dps = 0.0;

  ///@}

  //**************************************************************************
  /// @name Helicopter-Specific Fields — populated only by helicopter backend.
  //**************************************************************************
  ///@{

  /** Main rotor RPM [rev/min]. Nominal Bo 105: ~395 RPM. */
  std::optional<double> rotor_rpm;

  /** Engine torque [% of max]. Values above 100 indicate torque exceedance. */
  std::optional<double> engine_torque_pct;

  /** Fuel flow rate [US gal/hr]. */
  std::optional<double> fuel_flow_gph;

  ///@}

  //**************************************************************************
  /// @name Ground Vehicle-Specific Fields — populated only by ground vehicle backend.
  //**************************************************************************
  ///@{

  /** Front wheel steering angle [deg]. Positive: steer left. Range: [-35, 35]. */
  std::optional<double> steering_angle_deg;

  /** Wheel speed [rev/s]. */
  std::optional<double> wheel_speed_rps;

  ///@}

  //**************************************************************************
  /// @name Naval Vessel-Specific Fields — populated only by naval backend.
  //**************************************************************************
  ///@{

  /** Douglas sea state [0-9]. 0 = calm, 9 = phenomenal (>14 m waves). */
  std::optional<double> sea_state;

  /** Rudder angle [deg]. Positive: starboard. Range: [-35, 35]. */
  std::optional<double> rudder_angle_deg;

  ///@}

  //**************************************************************************
  /// @name Synthesised Sensor Outputs -  not used at the moment - brainstorming for future work  
  //**************************************************************************
  ///@{

  /**
   * Radar altimeter reading [ft]. Empty above 2500 ft AGL or on sensor failure.
   * Gaussian noise model: sigma = 0.6 m.
   */
  std::optional<double> radar_alt_ft;

  /** EO/IR target range [m]. Empty when no target in FOV or sensor failed. 
   * EO = electro-optical (day camera), IR = infrared (thermal). 
   * The sensor points at a target and measures how far away it is. 
   * Empty when no target is in the camera's field of view, or when the sensor is failed.
  */
  std::optional<double> eoir_target_range_m;

  /**
   * EO/IR target lock status.
   * true = locked, false = detected but not locked, empty = no target.
   */
  std::optional<bool> eoir_target_locked;

  ///@}

  //**************************************************************************
  /// @name Evaluation Context — set by the simulation loop, not the physics model.
  //**************************************************************************
  ///@{

  /**
   * Current mission phase.
   * A ground vehicle has mission phases (transit, hold, objective)
   * A helicopter has mission phases (takeoff, cruise, landing)
   * A naval vessel has mission phases (departure, patrol, return)
   * 
   * notes for me: The sim loop (or a waypoint manager) sets this string as the mission progresses. 
   * It lets the query engine later ask questions like "what was the average speed during approach?" or 
   * "did the heading stay stable during hover?"
   * Right now in your ground_vehicle_model it's hardcoded to "cruise" — a placeholder until you build a waypoint manager that updates it.
   */
  std::string mission_phase = "initialise";

  /**
   * Waypoint reached flag.
   * True for exactly one sample when a waypoint is reached, then resets.
   */
  bool waypoint_reached = false;

  /** Active failure IDs injected via failure_event. Empty = nominal. 
   * Every time inject() is called, the failure ID gets added to this vector and stays there for all subsequent samples. 
   * So the query engine can later filter: "show me all samples where engine failure was active."
  */
  std::vector<std::string> active_failures;

  ///@}
};

//****************************************************************************
} // namespace sentinex::telemetry
//****************************************************************************
