#include <minipix_interface_linux.h>
#include <gatherer_interface_linux.h>

SerialPort serial_port_minipix_;

Gatherer_Handler_t gatherer_handler_;

uint8_t tx_buffer_lander[SERIAL_BUFFER_SIZE];

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

  gatherer_processImagePacket(&gatherer_handler_, image_data);
}

//}

/* mui_processStatus() //{ */

void mui_processStatus(const Status_t *status) {

  gatherer_processStatus(&gatherer_handler_, status);
}

//}

/* mui_ledSetHW() //{ */

void mui_ledSetHW([[maybe_unused]] bool state) {
  // do nothing, we don't have LEDs on LINUX
}

//}
