#ifndef MINIPIX_INTERFACE_LINUX_H
#define MINIPIX_INTERFACE_LINUX_H

#include <chrono>
#include <thread>

#include <serial_port.h>

#include <mui.h>

#define SERIAL_BUFFER_SIZE 2048

extern SerialPort serial_port_minipix_;

extern SerialPort serial_port_lander_;
extern LLCP_Receiver_t llcp_receiver_lander;

extern uint8_t tx_buffer_lander[SERIAL_BUFFER_SIZE];

void mui_sleepHW(const uint16_t milliseconds);
void mui_ledSetHW(bool state);
void mui_sendChar(const uint8_t char_out);
void mui_sendString(const uint8_t *str_out, const uint16_t len);
void mui_processImagePacket(const ImageData_t *image_data);
void mui_processStatus(const Status_t *status);

#endif  // MINIPIX_INTERFACE_LINUX_H
