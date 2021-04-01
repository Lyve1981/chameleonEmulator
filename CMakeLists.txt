cmake_minimum_required(VERSION 3.15)

# lets build a fat binary that runs on both intel and the new Apple M1 chip
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "Minimum OS X deployment version")

project(chameleonEmu VERSION 0.0.1)

include(base.cmake)

add_definitions("-DCHAMELEON_MODEL=01")

# --------------- chameleon examples

add_library(ch_example_dspthru STATIC tools/Chameleon.sdk/src/examples/dspthru/main.c)

target_include_directories(ch_example_dspthru PRIVATE tools/Chameleon.sdk/include/rtems)
target_include_directories(ch_example_dspthru PRIVATE tools/Chameleon.sdk/include/chameleon)
