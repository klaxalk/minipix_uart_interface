#include <llcp.h>
#include <serial_port.h>

#include <thread>
#include <chrono>

#define SERIAL_BUFFER_SIZE 2048

int main() {

  // the hardware serial driver
  SerialPort serial_port;
  serial_port.connect("/tmp/ttyS0", 115200);

  // the bacaprotol receiver instance
  Receiver_t receiver;
  llcpInitialize(&receiver);

  while (true) {

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    uint8_t read_buffer[SERIAL_BUFFER_SIZE];
    int     bytes_read;

    bytes_read = serial_port.readSerial(read_buffer, SERIAL_BUFFER_SIZE);

    for (int i = 0; i < bytes_read; i++) {

      printf("received %d chars\n", bytes_read);

      Message_t message;

      if (llcpProcessChar(read_buffer[i], &receiver, &message)) {

        printf("received messge\n");

        switch (message.id) {

          printf("id fits\n");

          case MSG_ID_TEST: {

            printf("received test message, %d, %d, %d\n", ((Test_t*)&message.payload)->test1, ((Test_t*)&message.payload)->test2, ((Test_t*)&message.payload)->test3);

            break;
          }
        }
      }
    }
  }

  return 0;
}
