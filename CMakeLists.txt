#********* Copyright © 2025 Jishnu Periya, Jonathon Bell. All rights reserved.
#**
#**
#**  Version : $Header:$
#**
#**
#**  Purpose : CMake build script for the sentinex project.
#**
#**  See Also: https://cmake.org/cmake/help/latest/
#**
#**
#******************************************************************************

cmake_minimum_required(VERSION 3.20.2)

project(sentinex
  VERSION       0.2.0
  DESCRIPTION   "SENTINEX simulation platform"
  HOMEPAGE_URL  "https://github.com/jishnuperiya/sentinex")

#*****************************************************************************
# CMake Configuration
#*****************************************************************************

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

enable_testing()

if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra)
endif()

#*****************************************************************************
# Dependencies
#*****************************************************************************

find_package(Doxygen 1.9.8)            # optional — only needed for 'doc' target
find_package(DocTest 2.4.11 REQUIRED)
find_package(RapidCheck REQUIRED)

#*****************************************************************************
# Core Library
# Compiled units: vehicle_state + motion_model (bicycle physics)
#*****************************************************************************

add_library(sentinex-core STATIC
  src/model/vehicle_state.cpp
  src/model/motion_model.cpp
)

target_include_directories(sentinex-core PUBLIC src)

#*****************************************************************************
# Simulation CLI  (Milestone 1)
# End-to-end slice: scenario params -> platform_model -> telemetry -> console
#*****************************************************************************

add_executable(sentinex-sim
  src/sentinex/main.cpp
)

target_include_directories(sentinex-sim PRIVATE src/sentinex)
target_link_libraries(sentinex-sim PRIVATE sentinex-core)

#*****************************************************************************
# Unit Tests
#*****************************************************************************

add_executable(sentinex-test
  test/main.cpp
  test/test_vehicle_state.cpp
  test/test_motion_model.cpp
)

if(TARGET rapidcheck)
  if(MSVC)
    target_compile_options(rapidcheck PRIVATE /FIcstdint)
  else()
    target_compile_options(rapidcheck PRIVATE -include cstdint)
  endif()
endif()

target_link_libraries(sentinex-test
  sentinex-core
  rapidcheck
  doctest::doctest
)

add_test(NAME sentinex.tests COMMAND sentinex-test)

#*****************************************************************************
# Documentation
#*****************************************************************************

if(DOXYGEN_FOUND)
  set(DOXYGEN_IN  ${CMAKE_SOURCE_DIR}/Doxyfile.in)
  set(DOXYGEN_OUT ${CMAKE_BINARY_DIR}/Doxyfile)

  configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

  add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
  )
endif()

#*****************************************************************************
# Summary
#*****************************************************************************

message(STATUS "Sentinex Configuration Summary:")
message(STATUS "  Version:       ${sentinex_VERSION}")
message(STATUS "  CMake:         ${CMAKE_VERSION}")
message(STATUS "  C++ Standard:  ${CMAKE_CXX_STANDARD}")
message(STATUS "  Generator:     ${CMAKE_GENERATOR}")
message(STATUS "")
message(STATUS "Targets:")
message(STATUS "  sentinex-sim   - Simulation CLI (Milestone 1)")
message(STATUS "  sentinex-test  - Unit tests (ctest)")
if(DOXYGEN_FOUND)
  message(STATUS "  doc            - API documentation (doxygen)")
endif()
