#include <llcp.h>
#include <serial_port.h>

#include <thread>
#include <chrono>

#define SERIAL_BUFFER_SIZE 2048

int main() {

  // the hardware serial driver
  SerialPort serial_port;
  serial_port.connect("/tmp/ttyS1", 115200);

  // the bacaprotol receiver instance
  Receiver_t receiver;

  while (true) {

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    Test_t test;
    test.test1 = 2;
    test.test2 = 4;
    test.test3 = 127;

    Message_t message;
    message.id = MSG_ID_TEST;
    llcpPrepareMessage((uint8_t*)&test, sizeof(test), &message);

    /* serial_port.sendCharArray(message.payload, message.payload_size); */
    serial_port.sendChar('b');
    printf("sending message with %d bytes\n", message.payload_size);
  }

  return 0;
}
