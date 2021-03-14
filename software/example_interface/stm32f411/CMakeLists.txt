cmake_minimum_required(VERSION 3.13)

set(STM32_TOOLCHAIN_PATH /usr/bin)
set(STM32_CUBE_F4_PATH /usr/local/STM32Cube_FW_F4_V1.26.0)
set(STM32_TARGET_TRIPLET arm-none-eabi)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/stm32_gcc.cmake)

project(example_interface C ASM) # add CXX for c++

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")

find_package(CMSIS COMPONENTS STM32F4 REQUIRED)
find_package(HAL COMPONENTS STM32F4 RCC UART GPIO DMA CORTEX REQUIRED)

add_executable(main
  src/main.c
  src/stm_core/stm32f4xx_it.c
  src/stm_core/stm32f4xx_hal_msp.c
  src/minipix_interface_stm.c
)

include_directories(
  include
  include/stm_core
)

target_link_libraries(main
  # STM libraries
  HAL::STM32::F4::RCC
  HAL::STM32::F4::DMA
  HAL::STM32::F4::GPIO
  HAL::STM32::F4::CORTEX
  HAL::STM32::F4::UART
  CMSIS::STM32::F411RE
  STM32::NoSys
)
