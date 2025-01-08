cmake_minimum_required(VERSION 3.15)

set(LDB_VERSION_MAJOR 0)
set(LDB_VERSION_MINOR 1)
set(LDB_VERSION_PATCH 0)

set(LDB_VERSION "${LDB_VERSION_MAJOR}.${LDB_VERSION_MINOR}.${LDB_VERSION_PATCH}")

project(loggerDB LANGUAGES C VERSION ${LDB_VERSION})

option(LDB_ENABLE_BENCH "enable simple benchmarks" OFF)

add_subdirectory(loggerDB)
