# MiniPIX UART Interface (MUI)

MUI is a minimalistic C99 library for controlling [MiniPIX TPX3](https://advacam.com/camera/minipix-tpx3) devices over a serial line (UART).
The interface was developped specifically for constraint applications of Space radiation dosimetry.
Maximum efford was invested into making the interface as lightweight as possible (to support even 8bit microcontrollers) and to make the user integration as simple as possible, regardless of the underlying system.
This repository contains the software neccessary for interaction with MiniPIX, as well as example use cases (Linux, STM32F4), dummy detectors and decoder examples.

## [Software](https://github.com/klaxalk/tpx_lunar_lander/tree/master/software)

| Build status | [![Build](https://github.com/klaxalk/minipix_uart_interface/workflows/Build/badge.svg)](https://github.com/klaxalk/minipix_uart_interface/actions) | [![Docs](https://github.com/klaxalk/minipix_uart_interface/workflows/Docs/badge.svg)](https://github.com/klaxalk/minipix_uart_interface/actions) |
|--------------|----------------------------------------------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------|

Please follow to [software/README.md](./software/README.md) for more information.

### [Generated documentation](https://klaxalk.github.io/minipix_uart_interface/)

MiniPIX TPX3 UART Interface: [mui.h](https://klaxalk.github.io/minipix_uart_interface/mui_2include_2mui_8h.html)

Message definitions: [llcp_minipix_messages.h](https://klaxalk.github.io/minipix_uart_interface/mui_2include_2llcp_2llcp__minipix__messages_8h.html)

## Additional material

### System diagram of the provided software

A potential user/integrator should focus in directly using the **yellow**-labelled blocks, specifically, the [MUI](./software/mui/README.md).
The rest is provided for debugging and as examples, and is not supposed to serve _as is_.

[![](./fig/diagram.png)](./fig/diagram.pdf)

### HW diagram

HW diagram of the Linux Dummy + STM32F4 MUI example + Gatherer.

![](fig/hw_diagram_labels.png)

HW diagram of the MiniPIX TPX3 + Linux MUI example + Gatherer.

![](fig/uart_minipix_pinout.jpg)

### Supported operations

The MUI supports only a handful of functions.
The TPX3 measurement is done in a custom _frame mode_.

| "command"                      | description                                | parameters             | returns                    |
|--------------------------------|--------------------------------------------|------------------------|----------------------------|
| `mui_pwr()`                    | turns on/off the TPX3 detector             | 1/0                    | ack                        |
| `mui_getStatus()`              | gets housekeeping data from MiniPIX        | -                      | boot count, status message |
| `mui_getTemperature()`         | gets the MiniPIX's temperature             | -                      | int16_t (°C)               |
| `mui_measureFrame()`           | requests frame acquisition                 | acquisition time [ms]  | packetized frame           |
| `mui_updatePixelMask()`        | set pixel mask                             | pixel coordinates, 1/0 | ack                        |
| `mui_setThreshold()`           | set energy threshold, mostly for debugging | coarse thr, fine thr   | ack                        |
| `mui_setConfigurationPreset()` | switches between pre-loaded configurations | preset ID              | ack                        |
