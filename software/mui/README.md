# MiniPIX UART Interface (MUI)

This is a C99 library that facilitates interraction with MiniPIX over UART.
This library is supposed to be incorporated into application code by an integrator of MiniPIX.

More general info and context in the general [README](../README.md).

## Pre-build requirements

The MUI should be compilable without any requirements and modifications from the integrator.
However, several compilation directives can be used to modify the behaviour of the MUI and the underlying LLCP.

### LLCP customization

LLCP can operate in either binary or hexadecimal transmission mode.
MiniPIX will most probably be set for the binary (default).
However, if you use LLCP for a custom project, or if you have MiniPIX set explicitly for HEX, change the behavior during compilation by the following define before including `mui.h`.
```C
#define LLCP_COMM_HEXADECIMAL 1
```

Debug print can be enabled in LLCP by defining:

```C
#define LLCP_DEBUG_PRINT 1
```

For more niche options, see ![llcp.h](../llcp/include/llcp.h).

### MUI customization

MUI allows the user/integrator to handle the communication _acking_ manually by defining:
```C
#define MUI_USER_HANDSHAKES 1
```
Otherwise, acks are sent automatically by the MUI after the user callback is executed.
Manual acking could be useful if the user/integrator requires more control of timings of the incomming data, particularly after calling `mui_getFrameData()`.
For details, please see the communication flow diagrams below under the label `#define MUI_USER_HANDSHAKES 1`.

## Run-time initialization

Adapting MUI requires to pass pointers to user implementated and hardware-specific functions.

## Operating MiniPIX with the MUI

### Masking dead or noisy pixels

  1. mui_updatePixelMask(...) #1
  2. ...
  3. mui_updatePixelMask(...) #n
  4. 

## Communication flow diagrams

### mui_pwr()

![](./fig/mui_pwr.png)

### mui_getStatus()

![](./fig/mui_get_status.png)

### mui_getTemperature()

![](./fig/mui_get_temperature.png)

### mui_measureFrame()

![](./fig/mui_measure_frame.png)

### mui_setThreshold()

![](./fig/mui_set_threshold.png)

### mui_setConfigurationPreset()

![](./fig/mui_set_config.png)

### mui_updatePixelMask()

![](./fig/mui_update_mask.png)
