//******** Copyright © 2026 Jishnu Periya, Jonathon Bell. All rights reserved.
//*
//*
//*  Project : Sentinex
//*
//*  Purpose : Sensor fusion and simulation suite for robust vehicle
//*            state estimation and system-level validation.
//*
//****************************************************************************
#pragma once
//****************************************************************************

/**
 * @mainpage Sentinex
 *
 * @section intro_sec Overview
 *
 * Sentinex is a C++20 framework for **vehicle state propagation, simulation,
 * and state-estimation experiments**, with an emphasis on **deterministic
 * behavior, explicit contracts, and testable invariants**.
 *
 * The project currently provides a minimal but well-defined foundation:
 * a kinematic bicycle motion model that propagates a planar vehicle state
 * forward in time from an arbitrary initial condition. This ground-truth
 * motion model serves as the reference against which future estimation
 * and sensor-simulation components can be evaluated.
 *
 * Sentinex is designed as a *laboratory*, not a monolithic simulator:
 * components are small, composable, and intentionally explicit about
 * their assumptions and limitations.
 *
 * @section goals_sec Design Goals
 *
 * The primary design goals of Sentinex are:
 *
 * - **Clarity over cleverness**
 *   APIs are designed to be easy to reason about, with minimal hidden behavior.
 *
 * - **Explicit contracts and invariants**
 *   Model assumptions (e.g. time step positivity, frame conventions) are
 *   documented and enforced at clear boundaries.
 *
 * - **Deterministic and reproducible behavior**
 *   Given the same inputs, the system produces the same outputs, enabling
 *   reliable validation and benchmarking.
 *
 * - **Testability first**
 *   Property-based testing is used to express and verify invariants rather
 *   than relying solely on hand-picked example cases.
 *
 * - **Incremental extensibility**
 *   The architecture is intended to grow naturally toward sensor simulation
 *   and estimation algorithms without requiring large refactors.
 *
 * @section features_sec Core Features
 *
 * Current features include:
 *
 * - **VehicleState**
 *   - Plain value type representing planar vehicle state
 *   - World-frame position (`x`, `y`), heading (`psi`), and longitudinal velocity (`v`)
 *
 * - **Kinematic Bicycle Motion Model**
 *   - Header-only implementation
 *   - Forward Euler time integration
 *   - Separation between state and command inputs
 *   - No implicit clamping or normalization
 *
 * - **Property-based testing infrastructure**
 *   - Built using doctest and RapidCheck
 *   - Tests focus on invariants such as:
 *     - reset identity
 *     - zero-velocity invariance
 *     - straight-line motion
 *     - time-step consistency
 *
 * @section architecture_sec Architecture
 *
 * Sentinex follows a layered architecture that separates concerns clearly:
 *
 * - **State representation**
 *   - Passive data structures with no embedded policy or invariants
 *
 * - **Motion models**
 *   - Responsible for state propagation
 *   - Enforce model-level preconditions (e.g. valid time step)
 *
 * - **Testing and validation**
 *   - Express system laws as properties rather than fixed scenarios
 *
 * Future extensions are expected to introduce:
 * - sensor models (e.g. GPS, IMU, LiDAR)
 * - noise and bias modeling
 * - state estimation algorithms (e.g. EKF-style filters)
 *
 * These components will build on the same principles of explicit contracts
 * and deterministic behavior.
 *
 * @section audience_sec Intended Audience
 *
 * Sentinex is intended for:
 *
 * - Engineers working on vehicle state estimation
 * - Simulation and validation engineers
 * - Researchers and students studying sensor fusion
 * - Developers interested in modern C++ system and estimation design
 *
 * A working knowledge of C++, basic kinematics, and numerical methods
 * is assumed.
 */