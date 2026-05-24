//******** Copyright   2026 Jishnu Periya, Jonathon Bell. All rights reserved.
//*
//*
//*  Version : $Header:$
//*
//*
//*  Purpose : Motion command value object used by the motion models.
//*
//*
//****************************************************************************
 #pragma once
//****************************************************************************

//****************************************************************************
namespace sentinex::model{
//****************************************************************************

   /**
   * Contains the two scalar control inputs required by the bicycle motion
   * model:
   * - `steering_cmd` : steering input (typically radians or a normalized
   *                    steering value depending on the model).
   * - `velocity_cmd` : longitudinal velocity command (units depend on the
   *                    rest of the system; typically meters per second).
   */
  struct motion_command
  { 
    double steering_cmd = 0.0;
    double velocity_cmd = 0.0;
  };
}