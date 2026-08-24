//******** Copyright © 2026 Jishnu Periya. All rights reserved.
//*
//*
//*  Version : $Header:$
//*
//*
//*  Purpose : Telemetry sink that prints samples to the terminal.
//*
//*  See Also: telemetry_sink.hpp — abstract interface
//*            csv_sink.hpp      — file output sink
//*
//*
//****************************************************************************
#pragma once
//****************************************************************************

#include <iostream>
#include <iomanip>
#include "telemetry_sink.hpp"

//****************************************************************************

namespace kineflow::telemetry
{

//****************************************************************************

/**
 * @brief Prints telemetry samples to stdout.
 *
 * @param print_every  Print one sample every N steps. Default: 10 (1 Hz at 10 Hz sim).
 */
class console_sink final : public telemetry_sink
{
public:
  explicit console_sink(int print_every = 10)
    : print_every_(print_every)
  {}

  void write(const telemetry_sample& s) override
  {
    if (step_count_++ % print_every_ != 0)
      return;

    std::cout
      << std::fixed << std::setprecision(4)
      << "t="     << std::setw(8)  << s.timestamp_sec << "s"
      << "  lat=" << std::setw(10) << s.latitude_deg
      << "  lon=" << std::setw(10) << s.longitude_deg
      << "  alt=" << std::setw(7)  << s.altitude_m    << "m"
      << "  hdg=" << std::setw(7)  << s.heading_deg   << "deg"
      << "  spd=" << std::setw(6)  << s.speed_ms      << "m/s"
      << "  phase=" << s.mission_phase;

    if (!s.active_failures.empty()) {
      std::cout << "  FAILURES:[";
      for (const auto& f : s.active_failures)
        std::cout << f << " ";
      std::cout << "]";
    }

    if (s.steering_angle_deg.has_value())
      std::cout << "  steer=" << s.steering_angle_deg.value() << "deg";

    if (s.wheel_speed_rps.has_value())
      std::cout << "  wheel=" << std::setprecision(2)
                << s.wheel_speed_rps.value() << "rps";

    std::cout << "\n";
  }

private:
  int print_every_ = 10;
  int step_count_  = 0;
};

//****************************************************************************
} // namespace kineflow::telemetry
//****************************************************************************
