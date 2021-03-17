#include <gatherer_interface_linux.h>

SerialPort serial_port_gatherer_;

uint8_t tx_buffer_gatherer_[SERIAL_BUFFER_SIZE];

// | ----------------------- hw support ----------------------- |

/* gatherer_sendChar() //{ */

void gatherer_sendChar(const uint8_t char_out) {

  serial_port_gatherer_.sendChar(char_out);
}

//}

/* gatherer_sendString() //{ */

void gatherer_sendString(const uint8_t *str_out, const uint16_t len) {

  if (!serial_port_gatherer_.sendCharArray((unsigned char *)str_out, len)) {
    printf("FAILED sending message with %d bytes\n", len);
  }
}

//}

void gatherer_getStatus(MUI_Handler_t *mui_handler) {

  printf("received request for status\n");

  mui_getStatus(mui_handler);
}

void gatherer_measureFrame(MUI_Handler_t *mui_handler, const uint16_t acquisition_time) {

  printf("received request for a frame\n");

  mui_measureFrame(mui_handler, acquisition_time);
}
