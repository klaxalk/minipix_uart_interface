cmake_minimum_required(VERSION 3.1.2)
project(example_interface)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

include_directories(
  include
  include/llcp
  include/mui
)

add_definitions(-DLLCP_LITTLE_ENDIAN)

add_executable(example_interface
  src/main.cpp

  src/minipix_interface_linux.cpp
  src/serial_port.cpp

  src/mui/mui.c

  src/llcp/llcp.c
  src/llcp/llcp_endian.c
  src/llcp/llcp_minipix_messages.c
)
