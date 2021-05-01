cmake_minimum_required(VERSION 3.10)
project(cExampleDspthru)

set(CH_SDK ../../../tools/Chameleon.sdk)
set(DSPTHRU_DIR ${CH_SDK}/src/examples/dspthru)

get_filename_component(test_file ${DSPTHRU_DIR}/dsp/dsp_dma.h ABSOLUTE)
if (NOT EXISTS ${test_file})
    message(FATAL_ERROR "Chameleon SDK was not built, please run Chameleon.sdk\\src\\makeall.bat")
endif ()

add_library(cExampleDspthru STATIC ${DSPTHRU_DIR}/main.c ${DSPTHRU_DIR}/dsp/dsp_dma.h ${DSPTHRU_DIR}/dsp/dsp_interrupt.h ${DSPTHRU_DIR}/dsp/dsp_polling.h)

target_include_directories(cExampleDspthru PRIVATE ${CH_SDK}/include/rtems)
target_include_directories(cExampleDspthru PRIVATE ${CH_SDK}/include/chameleon)
