cmake_minimum_required(VERSION 3.1.2)
project(llcp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(-DLLCP_LITTLE_ENDIAN)

add_definitions(-Wall)
add_definitions(-Wextra)
add_definitions(-Wpedantic)
# add_definitions(-O0)
# add_definitions(-g)

include_directories(
  include
)

add_library(llcp
  src/llcp.c
  src/llcp_endian.c
  src/llcp_minipix_messages.c
)
