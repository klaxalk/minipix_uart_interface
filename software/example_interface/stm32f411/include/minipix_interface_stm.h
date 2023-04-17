#ifndef EXAMPLE_INTERFACE_STM_H
#define EXAMPLE_INTERFACE_STM_H

// include the "virtual" example interface
#include <main.h>
#include <gatherer_interface_stm.h>

#include <mui.h>

UART_HandleTypeDef *huart_minipix_ptr_;

Gatherer_Handler_t *gatherer_handler_ptr_;

uint8_t tx_buffer_lander[LLCP_RX_TX_BUFFER_SIZE];
/* LLCP_Receiver_t     llcp_receiver_lander; */

void mui_stm_setUart(UART_HandleTypeDef *huart_minipix);
void mui_stm_setGathererHandler(Gatherer_Handler_t *gatherer_handler);

void mui_stm_sleepHW(const uint16_t milliseconds);
void mui_stm_ledSetHW(bool state);
void mui_stm_sendChar(const uint8_t char_out);
void mui_stm_sendString(const uint8_t *str_out, const uint16_t len);
void mui_stm_processFrameData(const LLCP_FrameData_t *data);
void mui_stm_processFrameDataTerminator(const LLCP_FrameDataTerminator_t *data);
void mui_stm_processStatus(const LLCP_Status_t *data);
void mui_stm_processTemperature(const LLCP_Temperature_t *data);
void mui_stm_processChipVoltage(const LLCP_ChipVoltage_t *data);
void mui_stm_processAck(const LLCP_Ack_t *data);
void mui_stm_processMinipixError(const LLCP_MinipixError_t *data);
void mui_stm_processFrameMeasurementFinished();

#endif  // EXAMPLE_INTERFACE_STM_H
