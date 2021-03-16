#include <minipix_interface_linux.h>

SerialPort serial_port_minipix_;
SerialPort serial_port_lander_;

LLCP_Receiver_t llcp_receiver_lander;
uint8_t         tx_buffer_lander[SERIAL_BUFFER_SIZE];

/* mui_sleepHW() //{ */

void mui_sleepHW(const uint16_t milliseconds) {

  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

//}

/* mui_sendChar() //{ */

void mui_sendChar(const uint8_t char_out) {

  serial_port_minipix_.sendChar(char_out);
}

//}

/* mui_sendString() //{ */

void mui_sendString(const uint8_t *str_out, const uint16_t len) {

  if (!serial_port_minipix_.sendCharArray((unsigned char *)str_out, len)) {
    printf("FAILED sending message with %d bytes\n", len);
  }
}

//}

/* mui_processImagePacket() //{ */

void mui_processImagePacket(const ImageData_t *image_data) {

  LLCP_ImageDataMsg_t msg;
  msg.message_id = LLCP_IMAGE_DATA_MSG_ID;
  msg.payload    = *image_data;
  hton_LLCP_ImageDataMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), tx_buffer_lander);
  serial_port_lander_.sendCharArray(tx_buffer_lander, n_bytes);
}

//}

/* mui_processStatus() //{ */

void mui_processStatus(const Status_t *status) {

  LLCP_StatusMsg_t msg;
  msg.message_id = LLCP_STATUS_MSG_ID;
  msg.payload    = *status;
  hton_LLCP_StatusMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), tx_buffer_lander);
  serial_port_lander_.sendCharArray(tx_buffer_lander, n_bytes);
}

//}

/* mui_ledSetHW() //{ */

void mui_ledSetHW([[maybe_unused]] bool state) {
  // do nothing, we don't have LEDs on LINUX
}

//}
