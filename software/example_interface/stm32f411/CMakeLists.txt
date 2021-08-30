cmake_minimum_required(VERSION 3.13)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/stm32_gcc.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(example_interface C ASM) # add CXX for c++

set(STM32_TOOLCHAIN_PATH /usr/bin)
set(STM32_CUBE_F4_PATH /usr/local/STM32Cube_FW_F4_V1.26.0)
set(STM32_TARGET_TRIPLET arm-none-eabi)

add_definitions(-DLLCP_LITTLE_ENDIAN)

add_definitions(-Wall)
add_definitions(-Wextra)
add_definitions(-Wpedantic)
# add_definitions(-O0)
# add_definitions(-g)

find_package(CMSIS COMPONENTS STM32F4 REQUIRED)
find_package(HAL COMPONENTS STM32F4 RCC UART GPIO DMA CORTEX REQUIRED)

include_directories(
  include
  include/stm_core
  include/llcp
  include/mui
  include/gatherer_interface
)

add_executable(main
  src/main.c

  src/stm_core/stm32f4xx_it.c
  src/stm_core/stm32f4xx_hal_msp.c

  src/mui/mui.c
  src/minipix_interface_stm.c

  src/gatherer_interface/gatherer_interface.c
  src/gatherer_interface_stm.c

  src/llcp/llcp.c
  src/llcp/llcp_endian.c
  src/llcp/llcp_minipix_messages.c
)

target_compile_definitions(main PRIVATE MUI_SEND_STRING=1)
# target_compile_definitions(main PRIVATE MUI_SEND_CHAR=1)

target_compile_definitions(main PRIVATE GATHERER_SEND_STRING=1)
# target_compile_definitions(main PRIVATE GATHERER_SEND_CHAR=1)

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
