#include <thread>
#include <chrono>

#include <minipix_dummy_linux.h>

int main() {

  MinipixDummyLinux minipix_dummy;

  minipix_dummy.initializeSerialPort("/tmp/ttyS0", 115200, true);

  printf("Starting while loop\n");

  while (true) {

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    minipix_dummy.update();
  }

  return 0;
}
