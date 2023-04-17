#include <minipix_interface_stm.h>

#define USART_TIMEOUT 100  // # [ms]

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

  mui_stm_ledSetHW(true);

  gatherer_processFrameData((Gatherer_Handler_t *)gatherer_handler_ptr_, data);
}

//}

/* mui_stm_processFrameDataTerminator() //{ */

void mui_stm_processFrameDataTerminator(const LLCP_FrameDataTerminator_t *data) {

  gatherer_processFrameDataTerminator((Gatherer_Handler_t *)gatherer_handler_ptr_, data);
}

//}

/* mui_stm_processStatus() //{ */

void mui_stm_processStatus(const LLCP_Status_t *data) {

  gatherer_processStatus((Gatherer_Handler_t *)gatherer_handler_ptr_, data);
}

//}

/* mui_stm_processTemperature() //{ */

void mui_stm_processTemperature(const LLCP_Temperature_t *data) {

  gatherer_processTemperature((Gatherer_Handler_t *)gatherer_handler_ptr_, data);
}

//}

/* mui_stm_processChipVoltage() //{ */

void mui_stm_processChipVoltage(const LLCP_ChipVoltage_t *data) {

  gatherer_processChipVoltage((Gatherer_Handler_t *)gatherer_handler_ptr_, data);
}

//}

/* mui_stm_processAck() //{ */

void mui_stm_processAck(const LLCP_Ack_t *data) {

  gatherer_processAck((Gatherer_Handler_t *)gatherer_handler_ptr_, data);
}

//}

/* mui_stm_processMinipixError() //{ */

void mui_stm_processMinipixError(const LLCP_MinipixError_t *data) {

  gatherer_processMinipixError((Gatherer_Handler_t *)gatherer_handler_ptr_, data);
}

//}

/* mui_stm_processFrameMeasurementFinished() //{ */

void mui_stm_processFrameMeasurementFinished() {

  gatherer_processFrameMeasurementFinished((Gatherer_Handler_t *)gatherer_handler_ptr_);
}

//}

/* mui_stm_ledSetHW() //{ */

void mui_stm_ledSetHW(const bool new_state) {

  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, new_state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

//}
