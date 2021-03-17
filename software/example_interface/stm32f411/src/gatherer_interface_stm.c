#include <gatherer_interface_stm.h>

#define USART_TIMEOUT 10  // # [ms]

/* setGathererUart() //{ */

void gatherer_setUart(UART_HandleTypeDef *huart_gatherer) {

  huart_gatherer_ptr_ = huart_gatherer;
}

//}

// | ----------------------- hw support ----------------------- |

/* gatherer_sendChar() //{ */

void gatherer_sendChar(const uint8_t char_out) {

  HAL_UART_Transmit(huart_gatherer_ptr_, (uint8_t *)&char_out, 1, USART_TIMEOUT);
}

//}

/* gatherer_sendString() //{ */

void gatherer_sendString(const uint8_t *str_out, const uint16_t len) {

  HAL_UART_Transmit(huart_gatherer_ptr_, (uint8_t *) str_out, len, USART_TIMEOUT);
}

//}
