# MiniPIX UART Interface (MUI) - Software

## LLCP - Low-level Communication Protocol

* C library
* UART link layer between MiniPIX and MUI
* Capable of sending messages with up to 255 bytes of payload
* Can send data in binary or hexadecimal (for easier debugging)
* RAM efficient: uses a single buffer to store received data and to present them to the user when decoded

## MUI - MiniPIX UART Interface

* C library
  * needs "virtual" methods to be implemented by the HW integrator
* uses LLCP to communicate with MiniPIX
* provides routines for controling MiniPIX
* calls user callbacks when data arrive

## Example interface

* Example implementation of MUI on
  * Linux
  * STM32F411RE Nucleo

## Dummy - MiniPIX emulator

* Responds identically as the MiniPIX over the UART
* Linux Implementation:
  * Loads real Timepix [data from a database](https://github.com/vzlusat/vzlusat1-timepix-data)
* HW implementation (TODO)
  * Possibly STM32F411RE Nucleo or Blue Pill
  * Could store a few real images
  * Will probably randomly generate images using store track data

## Gatherer - PC data receiver from the Example interface over FTDI

Saves incoming packeted data into files.

### OpenCV Visualization

```bash
sudo apt install libopencv-dev
```

## Decoder - PC data decoder

Stitches packeted data from files into viewable images.

# Running in Tmux sessions

`./tmux` contains tmuxinator sessions ready for testing.

```bash
sudo apt-get -y install tmux tmuxinator
```
