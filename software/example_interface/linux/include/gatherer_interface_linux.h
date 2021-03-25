#ifndef GATHERER_INTERFACE_LINUX_H
#define GATHERER_INTERFACE_LINUX_H

#include <gatherer_interface.h>

// need to be able to call the interfaces routines to control the MiniPIX
#include <minipix_interface_linux.h>

#include <serial_port.h>
#define SERIAL_BUFFER_SIZE 2048
extern SerialPort serial_port_gatherer_;

extern Gatherer_Handler_t gatherer_handler_;

// Linux-specific functions
void gatherer_linux_sendChar(const uint8_t char_out);
void gatherer_linux_sendString(const uint8_t *str_out, const uint16_t len);

#endif  // GATHERER_INTERFACE_LINUX_H
