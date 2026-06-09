//******** Copyright © 2026 Jishnu Periya. All rights reserved.
//*
//*
//*  Version : $Header:$
//*
//*
//*  Purpose : Telemetry sink that writes samples to a CSV file.
//*            One row per simulation step. Header written on first write().
//*
//*  See Also: telemetry_sink.hpp  — abstract interface
//*            console_sink.hpp    — terminal output sink
//*
//*
//****************************************************************************
#pragma once
//****************************************************************************

#include <fstream>
#include <stdexcept>
#include <string>
#include "telemetry_sink.hpp"

//****************************************************************************

namespace kineflow::telemetry
{

//****************************************************************************

/**
 * @brief Writes telemetry samples to a CSV file.
 * File is opened on construction and closed on flush() or destruction.
 */
class csv_sink final : public telemetry_sink
{
public:
  explicit csv_sink(const std::string& filepath)
    : file_(filepath)
  {
    if (!file_.is_open())
      throw std::runtime_error("csv_sink: cannot open file: " + filepath);

    write_header();
  }

  void write(const telemetry_sample& s) override
  {
    file_ << s.timestamp_sec  << ","
          << s.latitude_deg   << ","
          << s.longitude_deg  << ","
          << s.altitude_m     << ","
          << s.heading_deg    << ","
          << s.speed_ms       << ","
          << s.pitch_deg      << ","
          << s.roll_deg       << ","
          << s.yaw_rate_dps   << ","
          << s.mission_phase  << ","
          << s.waypoint_reached << ","
          << failures_to_string(s.active_failures)
          << "\n";
  }

  void flush() override
  {
    file_.flush();
  }

private:
  void write_header()
  {
    file_ << "timestamp_sec,"
          << "latitude_deg,"
          << "longitude_deg,"
          << "altitude_m,"
          << "heading_deg,"
          << "speed_ms,"
          << "pitch_deg,"
          << "roll_deg,"
          << "yaw_rate_dps,"
          << "mission_phase,"
          << "waypoint_reached,"
          << "active_failures"
          << "\n";
  }

  static std::string failures_to_string(const std::vector<std::string>& failures)
  {
    std::string result;
    for (const auto& f : failures) {
      if (!result.empty()) result += ";";
      result += f;
    }
    return result;
  }

  std::ofstream file_;
};

//****************************************************************************
} // namespace kineflow::telemetry
//****************************************************************************
