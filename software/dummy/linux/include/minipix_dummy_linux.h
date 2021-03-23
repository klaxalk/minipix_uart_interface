#ifndef MINIPIX_DUMMY_LINUX_H
#define MINIPIX_DUMMY_LINUX_H

#include <llcp_minipix_messages.h>

#include <minipix_dummy.h>

#include <strings.h>
#include <vector>

#include <thread>
#include <chrono>

#include <atomic>

#include <serial_port.h>
#include <type_traits>
#define SERIAL_BUFFER_SIZE 2048

class MinipixDummyLinux : MinipixDummy {

public:
  MinipixDummyLinux();

  void initializeSerialPort(const std::string &file, const int &baud, const bool virtual_port);

  void sendByte(const uint8_t &byte_out);
  void sendString(const uint8_t *bytes_out, const uint16_t &len);

  void sleep(const uint16_t &milliseconds);

  void update_linux(void);

  void simulateImageAcquisition(const uint16_t &acquisition_time);

  std::string data_folder;
  void setDataFolder(const std::string &data_folder);

private:
  SerialPort        serial_port_;
  uint8_t           rx_buffer_[SERIAL_BUFFER_SIZE];
  std::atomic<bool> serial_port_initialized_ = false;

  std::thread thread_serial_port_;
  void        threadSerialPort(void);
};

#endif  // MINIPIX_DUMMY_LINUX_H
