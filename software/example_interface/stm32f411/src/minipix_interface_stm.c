#include <minipix_interface_stm.h>

#define USART_TIMEOUT 10  // # [ms]

/* setMinipixUart() //{ */

void setMinipixUart(UART_HandleTypeDef huart_minipix) {

  huart_minipix_ptr_ = &huart_minipix;
}

//}

/* ledSetHW() //{ */

void ledSetHW(const bool new_state) {

  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, new_state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

//}

/* sleepHW() //{ */

void sleepHW(const int milliseconds) {

  HAL_Delay(milliseconds);
}

//}

/* uart2SendChar() //{ */

void minipixSendChar(const char char_out) {

  HAL_UART_Transmit(huart_minipix_ptr_, (uint8_t *)&char_out, 1, USART_TIMEOUT);
}

//}

/* minipixSendString() //{ */

void minipixSendString(const char *str_out, const char len) {

  HAL_UART_Transmit(huart_minipix_ptr_, (uint8_t *)str_out, len, USART_TIMEOUT);
}

//}
