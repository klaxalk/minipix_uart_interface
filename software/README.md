# MiniPIX TPX3 Lunar lander example

## LLCP - Low-level Communication Protocol

* C header library
* UART link layer between MiniPIX and the example interface
* to be used in the MiniPIX interface and in the MiniPIX itself

## MUI - MiniPIX UART Interface

* C++ header library
  * contains virtual classes to be implemented by the HW integrator
* uses LLCP
* interfaces with the MiniPIX over UART using

## Example interface

* Example implementatio of the MiniPIX interface on the STM32F411RE Nucleo board

## DPD - Data packet definitions

* C header library / python library
* Definition of the data packets provided by the MiniPIX interface

## Dummy - MiniPIX emulator

1. over USB<->FTDI<->UART
2. HW dummy

## Gatherer - PC data receiver from the Example interface over FTDI

Saves incoming packeted data into files.

## Decoder - PC data decoder

Stitches packeted data from files into viewable images.
