#include <minipix_interface_stm.h>

#define USART_TIMEOUT 10  // # [ms]

/* setMinipixUart() //{ */

void setMinipixUart(UART_HandleTypeDef huart_minipix) {

  huart_minipix_ptr_ = &huart_minipix;
}

//}

// | ------------------ the prototype methods ----------------- |

/* mui_ledSetHW() //{ */

void mui_ledSetHW(const bool new_state) {

  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, new_state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

//}

/* mui_sleepHW() //{ */

void mui_sleepHW(const uint16_t milliseconds) {

  HAL_Delay(milliseconds);
}

//}

/* mui_sendChar() //{ */

void mui_sendChar(const uint8_t char_out) {

  HAL_UART_Transmit(huart_minipix_ptr_, (uint8_t *)&char_out, 1, USART_TIMEOUT);
}

//}

/* mui_sendString() //{ */

void mui_sendString(const uint8_t *str_out, const uint16_t len) {

  HAL_UART_Transmit(huart_minipix_ptr_, (uint8_t *)str_out, len, USART_TIMEOUT);
}

//}

/* mui_processImagePacket() //{ */

void mui_processImagePacket(const ImageData_t *image_data) {
}

//}

/* mui_processStatus() //{ */

void mui_processStatus(const Status_t *status) {
}

//}
