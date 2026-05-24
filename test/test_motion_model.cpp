////******** copyright © 2025 jishnu periya, jonathon bell. all rights reserved.
////*
////*
////*  version : $header:$
////*
////*
////*  purpose : Unit tests for the BicycleMotionModel used in
////*            the Sentinex model module.
////*
////*
////*  see also: https://github.com/doctest/doctest/blob/master/doc/markdown/main.md
////*            for more on the doctest main entry point.
////*                                                                     0-0
////*                                                                   (| v |)
////**********************************************************************w*w***
//
//#include <doctest/doctest.h>         // for doctest
//#include <rapidcheck.h>              // for rc 
//#include <bicycle_motion.hpp>        // for sentinex::model::BicycleMotionModel
//#include <motion_command.hpp>        // for sentinex::model::MotionCommands
//
////****************************************************************************
//
//using sentinex::model::BicycleMotionModel;
//using sentinex::estimation::VehicleState;
//using sentinex::model::MotionCommand;
//
////****************************************************************************
//
//// Baseline generator: unconstrained VehicleState
//template<>
//struct rc::Arbitrary<VehicleState>
//{
//  static rc::Gen<VehicleState> arbitrary()
//  {
//    auto gen_x = rc::gen::arbitrary<double>();
//    auto gen_y = rc::gen::arbitrary<double>();
//    auto gen_psi = rc::gen::arbitrary<double>();
//    auto gen_v = rc::gen::arbitrary<double>();
//
//    return rc::gen::construct<VehicleState>(
//      gen_x,
//      gen_y,
//      gen_psi,
//      gen_v
//    );
//  }
//};
//
//template<>
//struct rc::Arbitrary<MotionCommand>
//{
//  static rc::Gen<MotionCommand> arbitrary()
//  {
//    auto gen_velocity_cmd = rc::gen::arbitrary<double>();
//    auto gen_steering_cmd = rc::gen::arbitrary<double>();
//
//    return rc::gen::construct<MotionCommand>(
//      gen_velocity_cmd,
//      gen_steering_cmd
//    );
//  }
//};
//
//TEST_CASE("BicycleMotionModel: reset reproduces state exactly")
//{
//  rc::check("reset sets the internal state to the provided state",
//    [](const VehicleState& state)
//    {
//      BicycleMotionModel model{ 4.0 };
//      model.reset(state);
//      const auto new_state = model.getState();
//      RC_ASSERT(new_state.x == state.x);
//      RC_ASSERT(new_state.y == state.y);
//      RC_ASSERT(new_state.psi == state.psi);
//      RC_ASSERT(new_state.v == state.v);
//
//    });
//}
//
//TEST_CASE("BicycleMotionModel: zero velocity causes no motion ")
//{
//  rc::check("v=0 implied no change in pose",
//    [](VehicleState state, MotionCommand cmd)
//    {
//      BicycleMotionModel model{ 4.0 };
//      model.reset(state);
//
//      cmd.velocity_cmd = 0;
//      
//      model.update(1.0, cmd);
//      const auto new_state = model.getState();
//
//      RC_ASSERT(new_state.x == state.x);
//      RC_ASSERT(new_state.y == state.y);
//      RC_ASSERT(new_state.psi == state.psi);
//      RC_ASSERT(new_state.v == state.v);
//    });
//
//}
//
//TEST_CASE("BicycleMotionModel: straight line motion ")
//{
//  rc::check("zero steering cause y and psi constant",
//    [](VehicleState state, MotionCommand cmd, double dt)
//    {
//      RC_PRE(dt > 0.0);
//
//      BicycleMotionModel model{ 4.0 };
//      model.reset(state);
//
//      cmd.steering_cmd = 0;
//
//      model.update(dt, cmd);
//      const auto new_state = model.getState();
//
//      RC_ASSERT(new_state.y == state.y);
//      RC_ASSERT(new_state.psi == state.psi);
//      RC_ASSERT(new_state.v == doctest::Approx(new_state.v + dt * cmd.velocity_cmd));
//    });
//}
//
//
//TEST_CASE("BicycleMotionModel: time consistnecy ")
//{
//  rc::check("updating with dt1 + dt2 equals updating with dt1 then dt2",
//    [](VehicleState state, MotionCommand cmd, double dt1, double dt2)
//    {
//      RC_PRE(dt1 > 0.0);
//      RC_PRE(dt2 > 0.0);
//
//      BicycleMotionModel model1{ 4.0 };
//      model1.reset(state);
//      model1.update(dt1, cmd);
//      model1.update(dt2, cmd);
//
//      const auto state1 = model1.getState();
//      
//      BicycleMotionModel model2{ 4.0 };
//      model2.reset(state);
//      model2.update(dt1 + dt2, cmd);
//      const auto state2 = model2.getState();
//
//      RC_ASSERT(state1.x == state2.x);
//      RC_ASSERT(state1.y == state2.y);
//      RC_ASSERT(state1.psi == state2.psi);
//      RC_ASSERT(state1.v == state2.v);
//   
//    });
//}
//
//
//
//
//
//
//
//
//
//
