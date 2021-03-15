#include <llcp/llcp.h>
#include <llcp/llcp_minipix_messages.h>
#include <serial_port.h>

#include <thread>
#include <chrono>
#define SERIAL_BUFFER_SIZE 2048

int main() {

  // the hardware serial driver
  SerialPort serial_port;
  serial_port.connect("/tmp/ttyS1", 115200, true);

  printf("Sender started\n");

  while (true) {

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    Test_t test;
    test.test_1   = 27;
    test.test_2   = 39;
    test.test_254 = 13;

    uint8_t  tx_buffer[SERIAL_BUFFER_SIZE];
    uint16_t n_bytes = llcpPrepareMessage((uint8_t*)&test, sizeof(test), tx_buffer);

    if (serial_port.sendCharArray(tx_buffer, n_bytes)) {
      printf("message with %d bytes was sent\n", n_bytes);
    } else {
      printf("FAILED sending message with %d bytes\n", n_bytes);
    }
  }

  return 0;
}
