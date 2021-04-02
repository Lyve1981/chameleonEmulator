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

# ----------------- chameleon examples

set(DSPTHRU_DIR tools/Chameleon.sdk/src/examples/dspthru)
add_library(ch_example_dspthru STATIC ${DSPTHRU_DIR}/main.c ${DSPTHRU_DIR}/dsp/dsp_dma.h ${DSPTHRU_DIR}/dsp/dsp_interrupt.h ${DSPTHRU_DIR}/dsp/dsp_polling.h)

target_include_directories(ch_example_dspthru PRIVATE tools/Chameleon.sdk/include/rtems)
target_include_directories(ch_example_dspthru PRIVATE tools/Chameleon.sdk/include/chameleon)

# ----------------- plugins

# ----------------- VST2 Plugin

add_library(chameleonEmuVST2 MODULE)

target_sources(chameleonEmuVST2 PRIVATE source/pluginVST2/vstMain.cpp)

if(MSVC)
	target_sources(chameleonEmuVST2 PRIVATE source/vstsdk2.4.2/public.sdk/samples/vst2.x/win/vstplug.def)
endif()

target_include_directories(chameleonEmuVST2 PRIVATE source/vstsdk2.4.2/)

target_link_libraries(chameleonEmuVST2 cePluginVST2 ch_example_dspthru)

if(APPLE OR UNIX)
	target_link_libraries(chameleonEmuVST2 ${OS_LINK_LIBRARIES})
endif()
