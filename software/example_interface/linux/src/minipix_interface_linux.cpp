#include <minipix_interface_linux.h>
#include <gatherer_interface_linux.h>

SerialPort serial_port_minipix_;

Gatherer_Handler_t gatherer_handler_;

uint8_t tx_buffer_lander[SERIAL_BUFFER_SIZE];

// | --------- Linux-specific implementations for MUI --------- |

/* mui_linux_sleepHW() //{ */

void mui_linux_sleepHW(const uint16_t milliseconds) {

  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

//}

/* mui_linux_sendChar() //{ */

void mui_linux_sendChar(const uint8_t char_out) {

  serial_port_minipix_.sendChar(char_out);
}

//}

/* mui_linux_sendString() //{ */

void mui_linux_sendString(const uint8_t *str_out, const uint16_t len) {

  if (!serial_port_minipix_.sendCharArray((unsigned char *)str_out, len)) {
    printf("FAILED sending message with %d bytes\n", len);
  }
}

//}

/* mui_linux_processFrameData() //{ */

void mui_linux_processFrameData(const LLCP_FrameData_t *data) {

  gatherer_processFrameData(&gatherer_handler_, data);
}

//}

/* mui_linux_processFrameDataTerminator() //{ */

void mui_linux_processFrameDataTerminator(const LLCP_FrameDataTerminator_t *data) {

  gatherer_processFrameDataTerminator(&gatherer_handler_, data);
}

//}

/* mui_linux_processStatus() //{ */

void mui_linux_processStatus(const LLCP_Status_t *data) {

  gatherer_processStatus(&gatherer_handler_, data);
}

//}

/* mui_linux_processTemperature() //{ */

void mui_linux_processTemperature(const LLCP_Temperature_t *data) {

  gatherer_processTemperature(&gatherer_handler_, data);
}

//}

/* mui_linux_processAck() //{ */

void mui_linux_processAck(const LLCP_Ack_t *data) {

  gatherer_processAck(&gatherer_handler_, data);
}

//}

/* mui_linux_processMinipixError() //{ */

void mui_linux_processMinipixError(const LLCP_MinipixError_t *data) {

  gatherer_processMinipixError(&gatherer_handler_, data);
}

//}

/* mui_linux_ledSetHW() //{ */

void mui_linux_ledSetHW([[maybe_unused]] bool state) {
  // do nothing, we don't have LEDs on LINUX
}

//}
