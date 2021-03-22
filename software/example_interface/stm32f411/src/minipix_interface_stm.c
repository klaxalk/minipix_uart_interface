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

/* mui_stm_sleepHW() //{ */

void mui_stm_sleepHW(const uint16_t milliseconds) {

  HAL_Delay(milliseconds);
}

//}

/* mui_stm_sendChar() //{ */

void mui_stm_sendChar(const uint8_t char_out) {

  HAL_UART_Transmit(huart_minipix_ptr_, (uint8_t *)&char_out, 1, USART_TIMEOUT);
}

//}

/* mui_stm_sendString() //{ */

void mui_stm_sendString(const uint8_t *str_out, const uint16_t len) {

  HAL_UART_Transmit(huart_minipix_ptr_, (uint8_t *)str_out, len, USART_TIMEOUT);
}

//}

/* mui_stm_processFrameData() //{ */

void mui_stm_processFrameData(const LLCP_FrameData_t *data) {

  gatherer_processFrameData((Gatherer_Handler_t *)gatherer_handler_ptr_, data);
}

//}

/* mui_stm_processStreamData() //{ */

void mui_stm_processStreamData(const LLCP_StreamData_t *data) {

  gatherer_processStreamData((Gatherer_Handler_t *)gatherer_handler_ptr_, data);
}

//}

/* mui_stm_processStatus() //{ */

void mui_stm_processStatus(const LLCP_Status_t *status) {

  gatherer_processStatus((Gatherer_Handler_t *)gatherer_handler_ptr_, status);
}

//}

/* mui_stm_ledSetHW() //{ */

void mui_stm_ledSetHW(const bool new_state) {

  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, new_state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

//}
