#include <minipix_dummy_linux.h>

/* constructor MinipixDummyLinux() //{ */

MinipixDummyLinux::MinipixDummyLinux(void) {
}

//}

/* initializeSerialPort() //{ */

void MinipixDummyLinux::initializeSerialPort(const std::string& file, const int& baud, const bool virtual_port) {

  serial_port_.connect(file, baud, virtual_port);
}

//}

// | ------------ virtual functions --- HW-specific ----------- |

/* sendByte() //{ */

void MinipixDummyLinux::sendByte(const uint8_t& byte_out) {

  if (!serial_port_.sendCharArray((unsigned char*)&byte_out, 1)) {
    printf("FAILED sending message with %d bytes\n", 1);
  }
}

//}

/* sendByte() //{ */

void MinipixDummyLinux::sleep(const uint16_t& milliseconds) {

  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

//}

/* sendString() //{ */

void MinipixDummyLinux::sendString(const uint8_t* bytes_out, const uint16_t& len) {

  if (!serial_port_.sendCharArray((unsigned char*)bytes_out, len)) {
    printf("FAILED sending message with %d bytes\n", len);
  }
}

//}

/* update() //{ */

void MinipixDummyLinux::update(void) {

  // | --------- receive data from the minipix interface -------- |

  uint16_t bytes_read = serial_port_.readSerial(rx_buffer_, SERIAL_BUFFER_SIZE);

  if (bytes_read > 0) {
    // feed all the incoming bytes into the minipix interface
    serialDataCallback(rx_buffer_, bytes_read);
  } else {
    sleep(1);
  }
}

//}
