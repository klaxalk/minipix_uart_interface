cmake_minimum_required(VERSION 3.1.2)
project(mui)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(-DLLCP_LITTLE_ENDIAN)

add_definitions(-Wall)
add_definitions(-Wextra)
add_definitions(-Wpedantic)

include_directories(
  include
  include/llcp
)

add_library(mui
  src/mui.c
  src/llcp/llcp.c
  src/llcp/llcp_endian.c
  src/llcp/llcp_minipix_messages.c
)
