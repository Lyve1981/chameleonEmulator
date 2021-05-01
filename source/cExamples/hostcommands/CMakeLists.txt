cmake_minimum_required(VERSION 3.10)
project(cExampleHostcommands)

set(CH_SDK ../../../tools/Chameleon.sdk)

set(HOSTCOMMANDS_DIR ${CH_SDK}/src/examples/hostcommands)
get_filename_component(test_file ${HOSTCOMMANDS_DIR}/dsp/dsp_code.h ABSOLUTE)

if (NOT EXISTS ${test_file})
    message(FATAL_ERROR "Chameleon SDK was not built, please run Chameleon.sdk\\src\\makeall.bat")
endif ()

add_library(cExampleHostcommands STATIC ${HOSTCOMMANDS_DIR}/main.c ${HOSTCOMMANDS_DIR}/dsp/dsp_code.h)

target_include_directories(cExampleHostcommands PRIVATE ${CH_SDK}/include/rtems)
target_include_directories(cExampleHostcommands PRIVATE ${CH_SDK}/include/chameleon)
