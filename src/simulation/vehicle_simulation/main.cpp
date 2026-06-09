//******** Copyright © 2026 Jishnu Periya. All rights reserved.
//*
//*
//*  Version : $Header:$
//*
//*
//*  Purpose : SENTINEX simulation loop — Milestone 1.
//*            Initialises a ground vehicle platform, runs the simulation
//*            at 10 Hz, and writes telemetry to console and CSV.
//*
//*            Pipeline:
//*              IOS params → platform_model → telemetry_sample → sinks
//*
//*  See Also: ground_vehicle_model.hpp — platform backend
//*            platform_model.hpp       — abstract interface
//*            telemetry_sink.hpp       — sink interface
//*            console_sink.hpp         — terminal sink
//*            csv_sink.hpp             — CSV file sink
//*
//*
//****************************************************************************

#include <iostream>
#include <memory>
#include <vector>

#include "ground_vehicle_model.hpp"
#include "platform_model.hpp"
#include "console_sink.hpp"
#include "csv_sink.hpp"

//****************************************************************************

int main()
{

  // ── 1. Scenario parameters ───────────────────────────────────────────────
  kineflow::platform::scenario_params params;
  params.latitude_deg      = 47.6710;
  params.longitude_deg     = 9.5115;
  params.altitude_m        = 0.0;
  params.heading_deg       = 90.0;
  params.speed_ms          = 15.0;
  params.telemetry_rate_hz = 10.0;
  params.duration_sec      = 30.0;

  // ── 2. Platform backend ──────────────────────────────────────────────────
  auto platform = std::make_unique<kineflow::platform::ground_vehicle_model>();
  platform->initialise(params);

  std::cout << "Platform : " << platform->platform_type() << "\n";
  std::cout << "Origin   : " << params.latitude_deg << ", " << params.longitude_deg << "\n";
  std::cout << "Heading  : " << params.heading_deg  << " deg\n";
  std::cout << "Speed    : " << params.speed_ms     << " m/s\n";
  std::cout << "Duration : " << params.duration_sec << " s\n\n";

  // ── 3. Telemetry sinks ───────────────────────────────────────────────────
  std::vector<std::unique_ptr<kineflow::telemetry::telemetry_sink>> sinks;
  sinks.push_back(std::make_unique<kineflow::telemetry::console_sink>(10));
  sinks.push_back(std::make_unique<kineflow::telemetry::csv_sink>("mission_log.csv"));

  // ── 4. Failure event ─────────────────────────────────────────────────────
  kineflow::platform::failure_event engine_fail;
  engine_fail.system      = "engine";
  engine_fail.severity    = "partial";
  engine_fail.trigger_sec = 15.0;

  // ── 5. Simulation loop ───────────────────────────────────────────────────
  const double dt      = 1.0 / params.telemetry_rate_hz;
  const double end_sec = params.duration_sec;
  double sim_time = 0.0;
  int    step_num = 0;

  std::cout << "--- Telemetry stream ---\n";

  while (sim_time < end_sec)
  {
    if (engine_fail.trigger_sec > 0.0 &&
        sim_time >= engine_fail.trigger_sec &&
        sim_time <  engine_fail.trigger_sec + dt)
    {
      std::cout << "\n>>> FAILURE INJECTED: "
                << engine_fail.system << " (" << engine_fail.severity << ")"
                << " at T+" << sim_time << "s <<<\n\n";
      platform->inject(engine_fail);
      engine_fail.trigger_sec = -1.0;
    }

    platform->step(dt);
    auto sample = platform->read();

    for (auto& sink : sinks)
      sink->write(sample);

    sim_time += dt;
    ++step_num;
  }

  for (auto& sink : sinks)
    sink->flush();

  std::cout << "\n--- Mission complete ---\n";
  std::cout << "Total steps : " << step_num << "\n";
  std::cout << "Total time  : " << sim_time << " s\n";
  std::cout << "Log written : mission_log.csv\n\n";

  return 0;
}

//****************************************************************************
