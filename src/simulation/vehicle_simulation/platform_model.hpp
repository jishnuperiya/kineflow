//******** Copyright © 2026 Jishnu Periya. All rights reserved.
//*
//*
//*  Version : $Header:$
//*
//*
//*  Purpose : Abstract interface for all SENTINEX platform backends.
//*            Every platform (helicopter, ground vehicle, naval vessel)
//*            implements this interface. The IOS, telemetry pipeline,
//*            and query engine never see the concrete backend — only this.
//*
//*  See Also: telemetry_sample.hpp     — data contract produced by read()
//*            ground_vehicle_model.hpp — first concrete implementation
//*            helicopter_model.hpp     — JSBSim backend (Phase 1)
//*
//*
//****************************************************************************
#pragma once
//****************************************************************************

#include <memory>     // for std::unique_ptr
#include <string>     // for std::string
#include <vector>     // for std::vector
#include "telemetry_sample.hpp"

//****************************************************************************

namespace kineflow::platform
{

//****************************************************************************

/**
 * @brief Initial conditions and environment parameters injected by the IOS.
 * Passed to platform_model::initialise() before the simulation loop starts.
 */
struct scenario_params
{
  //**************************************************************************
  /// @name Initial Conditions
  //**************************************************************************
  ///@{

  /** Starting WGS84 latitude [deg]. Default: Friedrichshafen area. */
  double latitude_deg  = 47.6710;

  /** Starting WGS84 longitude [deg]. Default: Friedrichshafen area. */
  double longitude_deg = 9.5115;

  /** Starting altitude above ground level [m]. */
  double altitude_m    = 500.0;

  /** Starting true heading [deg]. Range: [0, 360). */
  double heading_deg   = 270.0;

  /** Starting speed [m/s]. */
  double speed_ms      = 40.0;

  ///@}

  //**************************************************************************
  /// @name Environment
  //**************************************************************************
  ///@{

  /** Wind speed [m/s]. */
  double wind_speed_ms    = 0.0;

  /** Wind direction [deg]. Direction wind is blowing FROM. */
  double wind_heading_deg = 0.0;

  /** Meteorological visibility [m]. Used by EO/IR sensor model. */
  double visibility_m     = 5000.0;

  /** Turbulence intensity [normalised, 0-1]. 0.0 = calm, 1.0 = severe. */
  double turbulence_norm  = 0.0;

  ///@}

  //**************************************************************************
  /// @name Simulation Control
  //**************************************************************************
  ///@{

  /** Telemetry capture rate [Hz]. */
  double telemetry_rate_hz = 10.0;

  /** Total mission duration [s]. */
  double duration_sec      = 600.0;

  ///@}
};

//****************************************************************************

/**
 * @brief A failure event injected into the platform at a specified mission time.
 * @see platform_model::inject()
 */
struct failure_event
{
  /**
   * System identifier.
   * Standard values: "engine", "radar_alt", "nav", "eoir", "hydraulic".
   */
  std::string system;

  /**
   * Failure severity.
   * Standard values: "partial" (degraded), "full" (complete failure).
   */
  std::string severity;

  /** Mission elapsed time at which to inject the failure [s]. */
  double trigger_sec = 0.0;
};

//****************************************************************************

/**
 * @brief Abstract interface for all SENTINEX platform backends.
 *
 * Every backend (helicopter, ground vehicle, naval vessel) implements this.
 * The IOS, telemetry pipeline, and query engine interact only with this
 * interface — never with concrete backends directly.
 *
 * @par Lifecycle
 * @code
 * auto platform = platform_factory::create("ground_vehicle", params);
 * platform->initialise(params);
 *
 * while (running) {
 *   platform->step(dt);
 *   auto sample = platform->read();
 *   telemetry.push_back(sample);
 * }
 * @endcode
 *
 * @see platform_factory     Creates concrete instances
 * @see ground_vehicle_model First concrete implementation
 * @see helicopter_model     JSBSim-backed implementation
 */
class platform_model
{
public:
  virtual ~platform_model() = default;

  /** @brief Initialise the platform. Called once before the simulation loop. */
  virtual void initialise(const scenario_params& params) = 0;

  /**
   * @brief Advance the simulation by one time step.
   * @param dt_sec  Time step [s]. Must be > 0.
   */
  virtual void step(double dt_sec) = 0;

  /**
   * @brief Read the current platform state as a telemetry sample.
   * Called after every step().
   */
  virtual kineflow::telemetry::telemetry_sample read() const = 0;

  /**
   * @brief Inject a failure event into the platform.
   * The failure ID is added to telemetry_sample::active_failures on all
   * subsequent read() calls.
   */
  virtual void inject(const failure_event& event) = 0;

  /** @return Platform type string, e.g. "helicopter", "ground_vehicle". */
  virtual std::string platform_type() const = 0;
};

//****************************************************************************

/**
 * @brief Factory — creates the correct platform backend from a type string.
 *
 * | Type string      | Backend               | Physics        |
 * |------------------|-----------------------|----------------|
 * | "ground_vehicle" | ground_vehicle_model  | Bicycle model  |
 * | "helicopter"     | helicopter_model      | JSBSim Bo 105  |
 * | "naval_vessel"   | naval_vessel_model    | Ship kinematics|
 *
 * @see platform_model  Interface all backends implement
 */
class platform_factory
{
public:
  /**
   * @param type    Platform type string from scenario XML.
   * @return        Owning pointer to the created backend.
   * @throws        std::invalid_argument if type is not recognised.
   */
  static std::unique_ptr<platform_model> create(
    const std::string&     type,
    const scenario_params& params
  );
};


//****************************************************************************
} // namespace kineflow::platform
//****************************************************************************
