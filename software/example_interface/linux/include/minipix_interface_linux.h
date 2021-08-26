#ifndef MINIPIX_INTERFACE_LINUX_H
#define MINIPIX_INTERFACE_LINUX_H

#include <chrono>
#include <thread>

#include <serial_port.h>

#include <mui.h>

#define SERIAL_BUFFER_SIZE 2048

extern SerialPort serial_port_minipix_;

// | ------------ linux-specific functions for MUI ------------ |
void mui_linux_sleepHW(const uint16_t milliseconds);
void mui_linux_ledSetHW(bool state);
void mui_linux_sendChar(const uint8_t char_out);
void mui_linux_sendString(const uint8_t *str_out, const uint16_t len);
void mui_linux_processFrameData(const LLCP_FrameData_t *data);
void mui_linux_processFrameDataTerminator(const LLCP_FrameDataTerminator_t *data);
void mui_linux_processStatus(const LLCP_Status_t *data);
void mui_linux_processTemperature(const LLCP_Temperature_t *data);
void mui_linux_processAck(const LLCP_Ack_t *data);
void mui_linux_processMinipixError(const LLCP_MinipixError_t *data);
void mui_linux_processMeasurementFinished();

#endif  // MINIPIX_INTERFACE_LINUX_H
