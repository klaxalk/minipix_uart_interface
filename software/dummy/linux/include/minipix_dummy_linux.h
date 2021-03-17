#ifndef MINIPIX_DUMMY_LINUX_H
#define MINIPIX_DUMMY_LINUX_H

#include <llcp_minipix_messages.h>

#include <minipix_dummy.h>

#include <thread>
#include <chrono>

#include <serial_port.h>
#define SERIAL_BUFFER_SIZE 2048

class MinipixDummyLinux : MinipixDummy {

public:
  MinipixDummyLinux();

  void initializeSerialPort(const std::string &file, const int &baud, const bool virtual_port);

  void update(void);

  void sendByte(const uint8_t &byte_out);
  void sendString(const uint8_t *bytes_out, const uint16_t &len);

  void sleep(const uint16_t &milliseconds);

private:
  SerialPort serial_port_;
  uint8_t    rx_buffer_[SERIAL_BUFFER_SIZE];
};

#endif  // MINIPIX_DUMMY_LINUX_H
