#include <thread>
#include <chrono>

#include <minipix_dummy_linux.h>

std::string serial_port;
int         baud_rate;
bool        serial_port_virtual;

int main(int argc, char *argv[]) {

  if (argc == 4) {
    serial_port         = argv[1];
    baud_rate           = atoi(argv[2]);
    serial_port_virtual = atoi(argv[3]);

    printf("loaded params: %s, %d, %s\n", serial_port.c_str(), baud_rate, serial_port_virtual ? "VIRTUAL" : "REAL");
  } else {
    printf("params not supplied\n");
    return 0;
  }

  MinipixDummyLinux minipix_dummy;

  minipix_dummy.initializeSerialPort(serial_port, baud_rate, serial_port_virtual);

  printf("Starting while loop\n");

  while (true) {

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    minipix_dummy.update();
  }

  return 0;
}
