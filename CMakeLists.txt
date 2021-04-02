cmake_minimum_required(VERSION 3.15)

# lets build a fat binary that runs on both intel and the new Apple M1 chip
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "Minimum OS X deployment version")

project(chameleonEmu VERSION 0.0.1)

include(base.cmake)

add_definitions("-DCHAMELEON_MODEL=01")
add_definitions("-DDEBUG")

# libraries
add_subdirectory(source/pluginVST2)
add_subdirectory(source/vstsdk2.4.2)
add_subdirectory(source/ceLib)
add_subdirectory(source/cExamples/dspthru)
add_subdirectory(source/dsp56300/source)

# ----------------- plugins

# ----------------- VST2 Plugin

add_library(chameleonEmuVST2 MODULE)

target_sources(chameleonEmuVST2 PRIVATE source/pluginVST2/vstMain.cpp)

if(MSVC)
	target_sources(chameleonEmuVST2 PRIVATE source/vstsdk2.4.2/public.sdk/samples/vst2.x/win/vstplug.def)
endif()

target_include_directories(chameleonEmuVST2 PRIVATE source/vstsdk2.4.2/)

target_link_libraries(chameleonEmuVST2 cePluginVST2 cExampleDspthru)

if(APPLE OR UNIX)
	target_link_libraries(chameleonEmuVST2 ${OS_LINK_LIBRARIES})
endif()
