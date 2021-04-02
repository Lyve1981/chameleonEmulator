cmake_minimum_required(VERSION 3.10)
project(ceLib)

add_library(ceLib STATIC)

set(SOURCES
	cePlugin.cpp cePlugin.h
	ceRtems.cpp ceRtems.h
)
	
target_sources(ceLib PRIVATE ${SOURCES})
source_group("source" FILES ${SOURCES})

target_include_directories(ceLib PRIVATE ../../tools/Chameleon.sdk/include/rtems)
target_include_directories(ceLib PRIVATE ../../tools/Chameleon.sdk/include/chameleon)

