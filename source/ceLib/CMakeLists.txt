cmake_minimum_required(VERSION 3.10)
project(ceLib)

add_library(ceLib STATIC)

set(SOURCES
	ceDsp.cpp ceDsp.h
	ceDspPeripherals.cpp ceDspPeripherals.h
	ceMidi.cpp ceMidi.h
	cePanel.cpp cePanel.h
	cePlugin.cpp cePlugin.h
	ceRtems.cpp ceRtems.h
)

target_sources(ceLib PRIVATE ${SOURCES})
source_group("source" FILES ${SOURCES})

target_include_directories(ceLib PRIVATE ../../tools/Chameleon.sdk/include/rtems)
target_include_directories(ceLib PRIVATE ../../tools/Chameleon.sdk/include/chameleon)

target_link_libraries(ceLib PUBLIC dsp56kEmu)