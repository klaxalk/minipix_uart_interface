cmake_minimum_required(VERSION 3.1.2)
project(minipix_dummy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(-DLLCP_LITTLE_ENDIAN)

add_definitions(-Wall)
add_definitions(-Wextra)
add_definitions(-Wpedantic)
add_definitions(-O0)
add_definitions(-g)

include_directories(
  include
  include/llcp
)

find_package(Threads) # only for linux

add_executable(minipix_dummy
  src/main.cpp
  src/serial_port.cpp
  src/minipix_dummy_linux.cpp
  src/minipix_dummy.cpp

  src/llcp/llcp.c
  src/llcp/llcp_endian.c
  src/llcp/llcp_minipix_messages.c

  src/pixel_encoder.c
)

target_link_libraries(minipix_dummy
  Threads::Threads # only for linux
)
