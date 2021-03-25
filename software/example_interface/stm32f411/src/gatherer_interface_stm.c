#include <gatherer_interface_stm.h>

void gatherer_stm_setUart(UART_HandleTypeDef *huart_gatherer) {

  huart_gatherer_ptr_ = huart_gatherer;
}

// | ----------------- STM-specific HW methods ---------------- |

void gatherer_stm_sendChar(const uint8_t char_out) {

  HAL_UART_Transmit(huart_gatherer_ptr_, (uint8_t *)&char_out, 1, USART_TIMEOUT);
}

void gatherer_stm_sendString(const uint8_t *str_out, const uint16_t len) {

  HAL_UART_Transmit(huart_gatherer_ptr_, (uint8_t *)str_out, len, USART_TIMEOUT);
}
