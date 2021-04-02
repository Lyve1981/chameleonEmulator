cmake_minimum_required(VERSION 3.10)
project(cePluginVST2)

add_library(cePluginVST2 STATIC)

target_sources(cePluginVST2 PRIVATE vstAudioEffect.cpp vstAudioEffect.h)

target_link_libraries(cePluginVST2 PUBLIC vstsdk2)

target_include_directories(cePluginVST2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../vstsdk2.4.2/)
target_include_directories(cePluginVST2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/Chameleon.sdk/include/rtems)
target_include_directories(cePluginVST2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/Chameleon.sdk/include/chameleon)
