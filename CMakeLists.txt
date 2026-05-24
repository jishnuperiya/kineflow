#********* Copyright © 2025 Jishnu Periya, Jonathon Bell. All rights reserved.
#**
#**
#**  Version : $Header:$
#**
#**
#**  Purpose : CMake build script for the sentinex project.
#**            Real-time multi-stream data visualization toolkit
#**
#**  See Also: https://cmake.org/cmake/help/latest/
#**            https://crascit.com/2015/03/28/enabling-cxx11-in-cmake/
#**            https://stackoverflow.com/a/59108287
#**
#**
#******************************************************************************

cmake_minimum_required(VERSION 3.20.2)

project(sentinex
  VERSION       0.2.0
  DESCRIPTION   "Real-time multi-stream data visualization toolkit"
  HOMEPAGE_URL  "https://github.com/jishnuperiya/sentinex")

#*****************************************************************************
# CMake Configuration
#*****************************************************************************

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(Qt6_DIR "C:/Qt/6.10.1/msvc2022_64/lib/cmake/Qt6")

# Qt Configuration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# C++ Standard
set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

# Enable testing
enable_testing()

# Compiler options
if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra)
endif()

#*****************************************************************************
# Dependencies
#*****************************************************************************

find_package(Doxygen 1.9.8 REQUIRED)
find_package(DocTest 2.4.11 REQUIRED)
find_package(RapidCheck REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

#*****************************************************************************
# Core Library - Header-Only Visualization Engine
#*****************************************************************************

add_library(sentinex-core INTERFACE)

target_include_directories(sentinex-core INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/src       
)

set_target_properties(sentinex-core PROPERTIES
  LINKER_LANGUAGE CXX
  OUTPUT_NAME sentinex-core
)

target_include_directories(sentinex-core INTERFACE src)

#*****************************************************************************
# System Monitor Library
#*****************************************************************************

add_library(sentinex-system-monitor STATIC
  src/system_monitor/system_metrics.hpp
  src/system_monitor/metrics_collector.hpp
  src/system_monitor/metrics_collector_windows.cpp
  src/system_monitor/metrics_collector_linux.cpp
)

target_link_libraries(sentinex-system-monitor PUBLIC sentinex-core)
target_include_directories(sentinex-system-monitor PUBLIC src)

# Platform-specific linking
if(MSVC)
  target_link_libraries(sentinex-system-monitor PRIVATE wbemuuid)
elseif(UNIX AND NOT APPLE)
  # Linux-specific dependencies if needed
endif()

#*****************************************************************************
# UI Library - Qt Visualization Components
#*****************************************************************************

add_library(sentinex-ui STATIC
  src/ui/main_window.hpp
  src/ui/main_window.cpp
  src/ui/plot_widget.hpp
  src/ui/plot_widget.cpp
  src/ui/dashboard_widget.hpp
  src/ui/dashboard_widget.cpp
)

target_link_libraries(sentinex-ui PUBLIC
  sentinex-core
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
)

target_include_directories(sentinex-ui PUBLIC src)

#*****************************************************************************
# Legacy Library (Optional - for backward compatibility)
#*****************************************************************************

add_library(sentinex-legacy STATIC
  src/legacy/vehicle_state.cpp
  src/legacy/motion_model.cpp
)

target_include_directories(sentinex-legacy PUBLIC src)

set_target_properties(sentinex-legacy PROPERTIES OUTPUT_NAME sentinex-legacy)

#*****************************************************************************
# System Monitor Application
#*****************************************************************************

add_executable(sentinex-gui
  src/ui/main.cpp
)

target_link_libraries(sentinex-gui
  sentinex-ui
  sentinex-system-monitor
  sentinex-core
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
)

set_target_properties(sentinex-gui PROPERTIES OUTPUT_NAME sentinex)

#*****************************************************************************
# CLI Application (Legacy)
#*****************************************************************************

add_executable(sentinex-cli
  ui/main.cpp
)

target_link_libraries(sentinex-cli
  sentinex-legacy
)

#*****************************************************************************
# Unit Tests
#*****************************************************************************

add_executable(sentinex-test
  #test/main.cpp
  #test/test_vehicle_state.cpp
  #test/test_motion_model.cpp
)

if(TARGET rapidcheck)
  if(MSVC)
    target_compile_options(rapidcheck PRIVATE /FIcstdint)
  else()
    target_compile_options(rapidcheck PRIVATE -include cstdint)
  endif()
endif()

target_link_libraries(sentinex-test
  sentinex-legacy
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
message(STATUS "  Version:          ${sentinex_VERSION}")
message(STATUS "  CMake Version:    ${CMAKE_VERSION}")
message(STATUS "  C++ Standard:     ${CMAKE_CXX_STANDARD}")
message(STATUS "  Generator:        ${CMAKE_GENERATOR}")
message(STATUS "  Qt6 Version:      ${Qt6_VERSION}")
message(STATUS "")
message(STATUS "Targets:")
message(STATUS "  sentinex          - Main system monitor GUI application")
message(STATUS "  sentinex-cli      - Legacy CLI application")
message(STATUS "  sentinex-test     - Unit tests")
message(STATUS "  doc               - API documentation (doxygen)")

