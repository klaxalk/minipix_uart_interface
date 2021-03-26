#ifndef MINIPIX_DUMMY_LINUX_H
#define MINIPIX_DUMMY_LINUX_H

#include <llcp_minipix_messages.h>

#include <minipix_dummy.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>

#include <thread>
#include <chrono>

#include <atomic>

#include <random>

#include <serial_port.h>
#include <type_traits>

#include <ctime>

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
  void        setDataFolder(const std::string &data_folder);

private:
  SerialPort serial_port_;
  uint8_t    rx_buffer_[SERIAL_BUFFER_SIZE];
  std::mutex mutex_serial_port_;

  std::thread thread_serial_port_;
  void        threadSerialPort(void);

  int randi(const int &from, const int &to);

  std::vector<std::vector<double>> loadImage(const std::string &file_path);
};

#endif  // MINIPIX_DUMMY_LINUX_H
