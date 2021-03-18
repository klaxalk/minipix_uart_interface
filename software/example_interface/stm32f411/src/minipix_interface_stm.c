#include <minipix_interface_stm.h>

#define USART_TIMEOUT 10  // # [ms]

// | --------------------- initialization --------------------- |

/* mui_stm_setUart() //{ */

void mui_stm_setUart(UART_HandleTypeDef *huart_minipix) {

  huart_minipix_ptr_ = huart_minipix;
}

//}

/* mui_stm_setGathererHandler() //{ */

void mui_stm_setGathererHandler(Gatherer_Handler_t *gatherer_handler) {

  gatherer_handler_ptr_ = gatherer_handler;
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

void mui_processImagePacket(const LLCP_FrameData_t *image_data) {

  gatherer_processImagePacket((Gatherer_Handler_t*) gatherer_handler_ptr_, image_data);
}

//}

/* mui_processStatus() //{ */

void mui_processStatus(const LLCP_Status_t *status) {

  gatherer_processStatus((Gatherer_Handler_t*) gatherer_handler_ptr_, status);
}

//}
