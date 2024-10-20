cmake_minimum_required(VERSION 3.15)

project(loggerDB LANGUAGES C VERSION 0.0.0)

add_compile_definitions(NAME="${CMAKE_PROJECT_NAME}")
add_compile_definitions(VERSION="${CMAKE_PROJECT_VERSION}")

add_subdirectory(src)