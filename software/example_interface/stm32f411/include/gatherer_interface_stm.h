#ifndef GATHERER_INTERFACE_STM_H
#define GATHERER_INTERFACE_STM_H

#include <gatherer_interface.h>

// need to be able to call the interfaces routines to control the minipix
#include <minipix_interface_stm.h>

extern Gatherer_Handler_t gatherer_handler_;

UART_HandleTypeDef *huart_gatherer_ptr_;

void gatherer_stm_setUart(UART_HandleTypeDef *huart_gatherer);

// hw support
void gatherer_stm_sendChar(const uint8_t char_out);
void gatherer_stm_sendString(const uint8_t *str_out, const uint16_t len);

#endif  // GATHERER_INTERFACE_STM_H
