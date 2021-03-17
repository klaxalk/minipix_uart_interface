#include <minipix_dummy_linux.h>

/* constructor MinipixDummyLinux() //{ */

MinipixDummyLinux::MinipixDummyLinux(void) {

  thread_serial_port_ = std::thread(&MinipixDummyLinux::threadSerialPort, this);
  /* thread_serial_port_.join(); */
}

//}

/* initializeSerialPort() //{ */

void MinipixDummyLinux::initializeSerialPort(const std::string& file, const int& baud, const bool virtual_port) {

  serial_port_.connect(file, baud, virtual_port);
  serial_port_initialized_ = true;
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

/* threadSerialPort() //{ */

void MinipixDummyLinux::threadSerialPort(void) {

  printf("waiting for serial\n");

  while (!serial_port_initialized_) {
    sleep(100);
  }

  printf("starting serial thread\n");

  while (true) {

    // | --------- receive data from the minipix interface -------- |

    uint16_t bytes_read = serial_port_.readSerial(rx_buffer_, SERIAL_BUFFER_SIZE);

    if (bytes_read > 0) {
      // feed all the incoming bytes into the minipix interface
      serialDataCallback(rx_buffer_, bytes_read);
    } else {
      sleep(1);
    }
  }
}

//}

/* update() //{ */

void MinipixDummyLinux::update_linux(void) {
  this->update();
}

//}
