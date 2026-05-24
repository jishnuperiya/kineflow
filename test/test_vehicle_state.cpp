////******** copyright © 2025 jishnu periya, jonathon bell. all rights reserved.
////*
////*
////*  version : $header:$
////*
////*
////*  purpose : Unit tests for the VehicleState value type used in
////*            the Sentinex estimation module.
////*
////*
////*  see also: https://github.com/doctest/doctest/blob/master/doc/markdown/main.md
////*            for more on the doctest main entry point.
////*                                                                     0-0
////*                                                                   (| v |)
////**********************************************************************w*w***
//
//#include <doctest/doctest.h>         // for doctest
//#include <vehicle_state.hpp>         // for sentinex::estimation::VehicleState
//
////****************************************************************************
//
//using sentinex::estimation::VehicleState;
//
////****************************************************************************
//
//TEST_CASE("VehicleState: default initialization")
//{
//  VehicleState state;
//  CHECK(state.x   == 0.0);
//  CHECK(state.y   == 0.0);
//  CHECK(state.psi == 0.0);
//  CHECK(state.v   == 0.0);
//}