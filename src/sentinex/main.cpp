//******** Copyright © 2026 Jishnu Periya. All rights reserved.
//*
//*
//*  Version : $Header:$
//*
//*
//*  Purpose : SENTINEX simulation loop — Milestone 1.
//*            Initialises a ground vehicle platform, runs the simulation
//*            at 10Hz, and prints telemetry to the terminal.
//*
//*            This is the first end-to-end slice:
//*              IOS params → PlatformModel → TelemetrySample → console
//*
//*            Next step: replace console output with XML writer.
//*
//*  See Also: ground_vehicle_model.hpp — platform backend
//*            platform_model.hpp       — abstract interface
//*            telemetry_sample.hpp     — data contract
//*
//*
//****************************************************************************

#include <iostream>     // for std::cout
#include <iomanip>      // for std::setw, std::fixed, std::setprecision
#include <memory>       // for std::make_unique
#include <chrono>       // for timing

#include "ground_vehicle_model.hpp"
#include "platform_model.hpp"
#include "telemetry_sample.hpp"

//****************************************************************************

/**
 * @brief Print a telemetry sample to the terminal.
 *
 * Formatted output matching the telemetry XML schema field names.
 * Used for Milestone 1 console verification before XML writer is added.
 *
 * @param s  Telemetry sample to print.
 */
void print_sample(const sentinex::telemetry::telemetry_sample& s)
{
  std::cout
    << std::fixed << std::setprecision(4)
    << "t="          << std::setw(8) << s.timestamp_sec  << "s"
    << "  lat="      << std::setw(10) << s.latitude_deg
    << "  lon="      << std::setw(10) << s.longitude_deg
    << "  alt="      << std::setw(7) << s.altitude_m     << "m"
    << "  hdg="      << std::setw(7) << s.heading_deg    << "deg"
    << "  spd="      << std::setw(6) << s.speed_ms       << "m/s"
    << "  phase="    << s.mission_phase;

  // Print active failures if any
  if (!s.active_failures.empty()) {
    std::cout << "  FAILURES:[";
    for (const auto& f : s.active_failures)
      std::cout << f << " ";
    std::cout << "]";
  }

  // Print ground vehicle specific fields
  if (s.steering_angle_deg.has_value())
    std::cout << "  steer=" << s.steering_angle_deg.value() << "deg";

  if (s.wheel_speed_rps.has_value())
    std::cout << "  wheel=" << std::setprecision(2)
              << s.wheel_speed_rps.value() << "rps";

  std::cout << "\n";
}

//****************************************************************************

int main()
{
  std::cout << "\n";
  std::cout << "================================================\n";
  std::cout << "  SENTINEX — Simulation Pipeline  Milestone 1  \n";
  std::cout << "  Platform: Ground Vehicle (Bicycle Model)      \n";
  std::cout << "================================================\n\n";

  // ── 1. Define scenario parameters (normally read from XML) ──────────────
  sentinex::platform::scenario_params params;
  params.latitude_deg      = 47.6710;   // Friedrichshafen area
  params.longitude_deg     = 9.5115;
  params.altitude_m        = 0.0;
  params.heading_deg       = 90.0;      // heading east
  params.speed_ms          = 15.0;      // 15 m/s (~54 km/h)
  params.telemetry_rate_hz = 10.0;      // 10 Hz
  params.duration_sec      = 30.0;      // 30 second mission

  // ── 2. Create platform backend ──────────────────────────────────────────
  auto platform = std::make_unique<sentinex::platform::ground_vehicle_model>();
  platform->initialise(params);

  std::cout << "Platform : " << platform->platform_type() << "\n";
  std::cout << "Origin   : " << params.latitude_deg
            << ", "          << params.longitude_deg << "\n";
  std::cout << "Heading  : " << params.heading_deg   << " deg\n";
  std::cout << "Speed    : " << params.speed_ms      << " m/s\n";
  std::cout << "Duration : " << params.duration_sec  << " s\n";
  std::cout << "Rate     : " << params.telemetry_rate_hz << " Hz\n\n";

  // ── 3. Define a failure event (normally read from XML) ──────────────────
  sentinex::platform::failure_event engine_fail;
  engine_fail.system      = "engine";
  engine_fail.severity    = "partial";
  engine_fail.trigger_sec = 15.0;     // inject at T+15s

  // ── 4. Simulation loop ──────────────────────────────────────────────────
  const double dt      = 1.0 / params.telemetry_rate_hz;  // 0.1s
  const double end_sec = params.duration_sec;

  std::cout << "--- Telemetry stream ---\n";

  double sim_time = 0.0;
  int    step_num = 0;

  while (sim_time < end_sec)
  {
    // Inject failures at the right time
    if (engine_fail.trigger_sec > 0.0 &&
        sim_time >= engine_fail.trigger_sec &&
        sim_time <  engine_fail.trigger_sec + dt)
    {
      std::cout << "\n>>> FAILURE INJECTED: "
                << engine_fail.system
                << " (" << engine_fail.severity << ") "
                << "at T+" << sim_time << "s <<<\n\n";
      platform->inject(engine_fail);
      engine_fail.trigger_sec = -1.0;  // mark as injected
    }

    // Step physics
    platform->step(dt);

    // Read telemetry
    auto sample = platform->read();

    // Print every sample (10Hz = 1 line per 0.1s)
    // For readability print every 10th sample (1Hz) to console
    if (step_num % 10 == 0)
      print_sample(sample);

    sim_time += dt;
    ++step_num;
  }

  std::cout << "\n--- Mission complete ---\n";
  std::cout << "Total steps : " << step_num << "\n";
  std::cout << "Total time  : " << sim_time << " s\n\n";

  std::cout << "Next: XML telemetry writer\n";

  return 0;
}

//****************************************************************************