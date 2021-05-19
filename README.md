## MiniPIX UART Interface (MUI)

## [Software](https://github.com/klaxalk/tpx_lunar_lander/tree/master/software)

| Build status | [![Build](https://github.com/klaxalk/minipix_uart_interface/workflows/Build/badge.svg)](https://github.com/klaxalk/minipix_uart_interface/actions) | [![Docs](https://github.com/klaxalk/minipix_uart_interface/workflows/Docs/badge.svg)](https://github.com/klaxalk/minipix_uart_interface/actions) |
|--------------|----------------------------------------------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------|

### [Documentation](https://klaxalk.github.io/minipix_uart_interface/)

MiniPIX UART Interface: [mui.h](https://klaxalk.github.io/minipix_uart_interface/mui_2include_2mui_8h.html)

Message definitions: [llcp_minipix_messages.h](https://klaxalk.github.io/minipix_uart_interface/mui_2include_2llcp_2llcp__minipix__messages_8h.html)

## Additional material

### System diagram

[![](./fig/diagram.png)](./fig/diagram.pdf)

### HW diagram

HW diagram of the Linux Dummy<->STM32F4 example<->Gatherer.

![](fig/hw_diagram_labels.png)

### Commands provided by the MiniPIX Interface

| "command"                      | description                                | parameters             | output form                                    |
|--------------------------------|--------------------------------------------|------------------------|------------------------------------------------|
| `mui_pwr()`                    | turns on/off the TPX3 detector             | 0/1                    | ack                                            |
| `mui_getStatus()`              | gets housekeeping data from MiniPIX        | -                      | a string of text, possibly with numeric values |
| `mui_getTemperature()`         | gets the MiniPIX's temperature             | -                      | int16t (interpretation TBD)                    |
| `mui_measureFrame()`           | requests frame acquisition                 | acquisition time       | packetized frame                               |
| `mui_updatePixelMask()`        | set pixel mask                             | pixel coordinates, 1/0 | ack                                            |
| `mui_setThreshold()`           | set energy threshold, mostly for debugging | coarse thre, fine thr  | ack                                            |
| `mui_setConfigurationPreset()` | switches between pre-loaded configurations | preset ID              | ack                                            |


For consideration

| "command"              | description                                                  | parameters       | output form                      |
|------------------------|--------------------------------------------------------------|------------------|----------------------------------|
| `mui_measureStream()`  | request stream acquisition (problematic to implement)        | duty cycle       | packets of measured event pixels |
| `mui_stopStream()`     | stop stream acquisition (might not be possible to implement) | -                | ack                              |
| `mui_flushBuffer()`    | request flusing of MiniPIX stream buffer                     | -                | ack                              |
| `getSinglePxlStream()` | requests image stream of single-pixel events (for VZLU)      | acquisition time | packetized stream                |
