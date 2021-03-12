#include <minipix_interface_stm.h>

#define USART_TIMEOUT 10  // # [ms]

/* constructor MinipixInterfaceSTM() //{ */

MinipixInterfaceSTM::MinipixInterfaceSTM() {
}

//}

/* setMinipixUart() //{ */

void MinipixInterfaceSTM::setMinipixUart(UART_HandleTypeDef &huart_minipix) {

  huart_minipix_ptr_ = &huart_minipix;
}

//}

/* ledSetHW() //{ */

void MinipixInterfaceSTM::ledSetHW(const bool &new_state) {

  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, new_state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

//}

/* sleepHW() //{ */

void MinipixInterfaceSTM::sleepHW(const int &milliseconds) {

  HAL_Delay(milliseconds);
}

//}

/* uart2SendChar() //{ */

void MinipixInterfaceSTM::minipixSendChar(const char &char_out) {

  HAL_UART_Transmit(huart_minipix_ptr_, (uint8_t *)&char_out, 1, USART_TIMEOUT);
}

//}

/* minipixSendString() //{ */

void MinipixInterfaceSTM::minipixSendString(const char *str_out, const char &len) {

  HAL_UART_Transmit(huart_minipix_ptr_, (uint8_t *)str_out, len, USART_TIMEOUT);
}

//}
