#include <gatherer_interface_linux.h>

SerialPort serial_port_gatherer_;

uint8_t tx_buffer_gatherer_[SERIAL_BUFFER_SIZE];

// | ----------------------- hw support ----------------------- |

/* gatherer_sendChar() //{ */

void gatherer_linux_sendChar(const uint8_t char_out) {

  serial_port_gatherer_.sendChar(char_out);
}

//}

/* gatherer_sendString() //{ */

void gatherer_linux_sendString(const uint8_t *str_out, const uint16_t len) {

  if (!serial_port_gatherer_.sendCharArray((unsigned char *)str_out, len)) {
    printf("FAILED sending message with %d bytes\n", len);
  }
}

//}
