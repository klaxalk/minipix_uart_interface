#include <thread>
#include <chrono>

#include <minipix_dummy_linux.h>

int main(int argc, char *argv[]) {

  std::string data_folder;
  std::string serial_port_file;
  int         baud_rate;
  bool        serial_port_virtual;

  if (argc == 5) {

    serial_port_file    = argv[1];
    baud_rate           = atoi(argv[2]);
    serial_port_virtual = atoi(argv[3]);
    data_folder         = argv[4];

    printf("loaded params: %s, %d, %s, %s\n", serial_port_file.c_str(), baud_rate, serial_port_virtual ? "VIRTUAL" : "REAL", data_folder.c_str());

  } else {

    printf("params not supplied\n");
    printf("required: ./dummy <serial port file> <baud rate> <serial port virtual ? true : false> <dummy data folder>\n");
    return 0;
  }

  MinipixDummyLinux minipix_dummy;

  minipix_dummy.initializeSerialPort(serial_port_file, baud_rate, serial_port_virtual);
  minipix_dummy.setDataFolder(data_folder);

  printf("Starting while loop\n");

  while (true) {

    // the dummy might not implement its own threads, so we need to update it
    // by calling this method
    minipix_dummy.update_linux();
  }

  return 0;
}
