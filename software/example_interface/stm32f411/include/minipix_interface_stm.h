#ifndef EXAMPLE_INTERFACE_STM_H
#define EXAMPLE_INTERFACE_STM_H

// include the "virtual" example interface
#include <main.h>
#include <mui.h>

UART_HandleTypeDef *huart_minipix_ptr_;

UART_HandleTypeDef *huart_lander_ptr_;
uint8_t             tx_buffer_lander[LLCP_RX_TX_BUFFER_SIZE];
LLCP_Receiver_t     llcp_receiver_lander;

void mui_sleepHW(const uint16_t milliseconds);
void mui_ledSetHW(bool state);
void mui_sendChar(const uint8_t char_out);
void mui_sendString(const uint8_t *str_out, const uint16_t len);
void mui_processImagePacket(const ImageData_t *image_data);
void mui_processStatus(const Status_t *status);

#endif  // EXAMPLE_INTERFACE_STM_H
