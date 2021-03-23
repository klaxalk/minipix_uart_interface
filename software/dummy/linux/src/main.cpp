#include <thread>
#include <chrono>

#include <minipix_dummy_linux.h>

std::string data_folder;
std::string serial_port_file;
int         baud_rate;
bool        serial_port_virtual;

SerialPort serial_port_;
void       serialThread(void);

int main(int argc, char *argv[]) {

  if (argc == 5) {
    serial_port_file    = argv[1];
    baud_rate           = atoi(argv[2]);
    serial_port_virtual = atoi(argv[3]);
    data_folder         = argv[4];

    printf("loaded params: %s, %d, %s, %s\n", serial_port_file.c_str(), baud_rate, serial_port_virtual ? "VIRTUAL" : "REAL", data_folder.c_str());
  } else {
    printf("params not supplied\n");
    return 0;
  }

  MinipixDummyLinux minipix_dummy;

  minipix_dummy.initializeSerialPort(serial_port_file, baud_rate, serial_port_virtual);
  minipix_dummy.setDataFolder(data_folder);

  printf("Starting while loop\n");

  while (true) {

    minipix_dummy.update_linux();
  }

  return 0;
}
