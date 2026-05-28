//******** Copyright © 2026 Jishnu Periya. All rights reserved.
//*
//*
//*  Version : $Header:$
//*
//*
//*  Purpose : Abstract sink interface for telemetry output.
//*            Each sink receives one telemetry_sample per simulation step.
//*            Multiple sinks can be active simultaneously.
//*
//*  See Also: console_sink.hpp — prints samples to terminal
//*            csv_sink.hpp     — writes samples to CSV file
//*
//*
//****************************************************************************
#pragma once
//****************************************************************************

#include "telemetry_sample.hpp"

//****************************************************************************

namespace sentinex::telemetry
{

//****************************************************************************

/**
 * @brief Abstract base for all telemetry output destinations.
 *
 * Implement this interface to add a new output destination.
 * The simulation loop calls write() once per step for every active sink.
 */
class telemetry_sink
{
public:
  virtual ~telemetry_sink() = default;

  /** @brief Receive one telemetry sample. Called once per simulation step. */
  virtual void write(const telemetry_sample& s) = 0;

  /** @brief Flush and finalise output. Called once after the simulation ends. */
  virtual void flush() {}
};

//****************************************************************************
} // namespace sentinex::telemetry
//****************************************************************************
