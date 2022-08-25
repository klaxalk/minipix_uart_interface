cmake_minimum_required(VERSION 3.13)

project(minipix_uart_interface)

add_subdirectory(./llcp)
add_subdirectory(./mui)
add_subdirectory(./dummy/linux)
add_subdirectory(./dummy/oneweb)
add_subdirectory(./example_interface/linux)
add_subdirectory(./gatherer)
add_subdirectory(./example_oneweb)
# add_subdirectory(./example_interface/stm32f411)
