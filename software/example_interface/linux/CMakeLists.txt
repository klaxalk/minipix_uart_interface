cmake_minimum_required(VERSION 3.1.2)
project(example_interface)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(-DLLCP_LITTLE_ENDIAN)

add_definitions(-Wall)
add_definitions(-Wextra)
add_definitions(-Wpedantic)
# add_definitions(-O0)
# add_definitions(-g)

include_directories(
  include
  include/llcp
  include/mui
  include/gatherer_interface
)

add_executable(example_interface
  src/main.cpp

  src/minipix_interface_linux.cpp
  src/gatherer_interface_linux.cpp
  src/serial_port.cpp

  src/gatherer_interface/gatherer_interface.c

  src/mui/mui.c

  src/llcp/llcp.c
  src/llcp/llcp_endian.c
  src/llcp/llcp_minipix_messages.c
)

target_compile_definitions(example_interface PRIVATE MUI_SEND_STRING=1)
# target_compile_definitions(example_interface PRIVATE MUI_SEND_CHAR=1)

target_compile_definitions(example_interface PRIVATE GATHERER_SEND_STRING=1)
# target_compile_definitions(example_interface PRIVATE GATHERER_SEND_CHAR=1)

target_compile_definitions(example_interface PRIVATE LLCP_DEBUG_PRINT=1)
