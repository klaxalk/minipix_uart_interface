cmake_minimum_required(VERSION 3.1.2)
project(example_interface)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

include_directories(
  include
)

add_executable(example_interface
  src/main.cpp
  src/serial_port.cpp
)
