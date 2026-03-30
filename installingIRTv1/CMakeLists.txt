cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project(detector_benchmarks LANGUAGES CXX)

cmake_policy(SET CMP0074 NEW)
include(GNUInstallDirs)

find_package(ROOT 6 REQUIRED COMPONENTS Core RIO Hist)
include(${ROOT_USE_FILE})

find_package(podio REQUIRED)
find_package(EDM4HEP REQUIRED)
find_package(EDM4EIC REQUIRED)

find_package(spdlog REQUIRED)
add_compile_definitions(SPDLOG_FMT_EXTERNAL)

add_subdirectory(benchmarks/rich)
