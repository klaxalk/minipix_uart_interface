#include <llcp/llcp.h>
#include <llcp/llcp_minipix_messages.h>
#include <serial_port.h>

#include <thread>
#include <chrono>

int main() {

  // the hardware serial driver
  SerialPort serial_port;
  serial_port.connect("/tmp/ttyS0", 115200, true);

  // the bacaprotol receiver instance
  Receiver_t receiver;
  llcpInitialize(&receiver);

  printf("Dummy started\n");

  while (true) {

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    uint8_t  read_buffer[LLCP_RX_TX_BUFFER_SIZE];
    uint16_t bytes_read;

    bytes_read = serial_port.readSerial(read_buffer, LLCP_RX_TX_BUFFER_SIZE);

    for (uint16_t i = 0; i < bytes_read; i++) {

      Message_t message;

      if (llcpProcessChar(read_buffer[i], &receiver, &message)) {

        switch ((Message_id_t)message.id) {

          case LLCP_MSG_ID_TEST: {

            printf("received test message, %d, %d, %d\n", ((Test_t*)&message.payload)->test_1, ((Test_t*)&message.payload)->test_2,
                   ((Test_t*)&message.payload)->test_254);

            break;
          };
        }
      }
    }
  }

  return 0;
}
