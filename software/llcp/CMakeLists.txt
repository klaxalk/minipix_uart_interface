cmake_minimum_required(VERSION 3.1.2)
project(llcp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

include_directories(
  include
)

add_definitions(-DLLCP_LITTLE_ENDIAN)

add_library(llcp
  src/llcp.c
  src/llcp_endian.c
  src/llcp_minipix_messages.c
)
