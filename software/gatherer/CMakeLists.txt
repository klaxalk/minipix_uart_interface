cmake_minimum_required(VERSION 3.1.2)
project(gatherer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set this to 1 to compile with OpenCV
set(GUI 1)

if(${GUI})
  MESSAGE(WARNING "[Gatherer]: Compiling with GUI")
else()
  MESSAGE(WARNING "[Gatherer]: Compiling without GUI")
endif()

add_definitions(-DLLCP_LITTLE_ENDIAN)

add_definitions(-Wall)
add_definitions(-Wextra)
add_definitions(-Wpedantic)
# add_definitions(-O0)
# add_definitions(-g)

find_package(Threads REQUIRED) # only for linux

set(INCLUDE_DIRS
  include
  include/llcp
  )

set(LINK_LIBS
  Threads::Threads # only for linux
  )

## | ------------ add OpenCV GUI >>>>>>>>>>>>>>>>> ------------ |
if(${GUI})

find_package(OpenCV REQUIRED)

set(INCLUDE_DIRS
  ${INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
)

set(LINK_LIBS
  ${LINK_LIBS}
  ${OpenCV_LIBS}
)

endif()
## | ------------ add OpenCV GUI <<<<<<<<<<<<<<<<< ------------ |

add_executable(gatherer
  src/main.cpp
  src/gatherer.cpp
  src/pixel_decoder.c
  src/serial_port.cpp

  src/llcp/llcp.c
  src/llcp/llcp_endian.c
  src/llcp/llcp_minipix_messages.c
)

include_directories(
  ${INCLUDE_DIRS}
)

target_link_libraries(gatherer
  ${LINK_LIBS}
)

target_compile_definitions(gatherer PRIVATE GUI=${GUI})
