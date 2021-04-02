cmake_minimum_required(VERSION 3.10)
project(cExampleDspthru)

set(CH_SDK ../../../tools/Chameleon.sdk)

set(DSPTHRU_DIR ${CH_SDK}/src/examples/dspthru)
add_library(cExampleDspthru STATIC ${DSPTHRU_DIR}/main.c ${DSPTHRU_DIR}/dsp/dsp_dma.h ${DSPTHRU_DIR}/dsp/dsp_interrupt.h ${DSPTHRU_DIR}/dsp/dsp_polling.h)

target_include_directories(cExampleDspthru PRIVATE ${CH_SDK}/include/rtems)
target_include_directories(cExampleDspthru PRIVATE ${CH_SDK}/include/chameleon)
