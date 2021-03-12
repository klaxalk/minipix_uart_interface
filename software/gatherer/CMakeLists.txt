cmake_minimum_required(VERSION 3.1.2)
project(gatherer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(
  include
)

add_executable(gatherer
  src/gatherer.cpp
)
