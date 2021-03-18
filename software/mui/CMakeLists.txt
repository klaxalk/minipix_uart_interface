cmake_minimum_required(VERSION 3.1.2)
project(mui)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

include_directories(
  include
  include/llcp
)

add_definitions(-DLLCP_LITTLE_ENDIAN)

add_library(mui
  src/mui.c
  src/llcp/llcp.c
  src/llcp/llcp_endian.c
  src/llcp/llcp_minipix_messages.c
)
