#******************************************************************************
# Copyright © 2025 Jishnu Periya. All rights reserved.
#
# Purpose : CMake build script for the kineflow project.
#******************************************************************************
cmake_minimum_required(VERSION 3.20.2)

project(kineflow
  VERSION       0.2.0
  DESCRIPTION   "Kineflow: Open-source, lightweight alternative to ADTF for real-time data pipelines."
  HOMEPAGE_URL  "https://github.com/jishnuperiya/kineflow")

#******************************************************************************
# CMake Configuration
#******************************************************************************
set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra)
endif()

#******************************************************************************
# Core Library (header-only)
#******************************************************************************
add_library(kineflow-core INTERFACE)
target_include_directories(kineflow-core INTERFACE src)

#******************************************************************************
# Main Executable
#******************************************************************************
add_executable(kineflow src/pipeline/main.cpp)
target_link_libraries(kineflow PRIVATE kineflow-core)