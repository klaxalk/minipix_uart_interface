cmake_minimum_required(VERSION 3.1.2)
project(gatherer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(-DLLCP_LITTLE_ENDIAN)

add_definitions(-Wall)
add_definitions(-Wextra)
add_definitions(-Wpedantic)

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED) # only for linux

include_directories(
  include
  include/llcp
  ${OpenCV_INCLUDE_DIRS}
)

add_executable(gatherer
  src/main.cpp
  src/serial_port.cpp

  src/llcp/llcp.c
  src/llcp/llcp_endian.c
  src/llcp/llcp_minipix_messages.c

  src/pixel_decoder.c
)

target_link_libraries(gatherer
  ${OpenCV_LIBS}
  Threads::Threads # only for linux
)
