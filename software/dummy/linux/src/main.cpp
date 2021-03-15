#define LLCP_LITTLE_ENDIAN

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

        switch ((MessageId_t)message.id) {

          case LLCP_IMAGE_DATA_MSG_ID: {

            uint8_t n_pixels = ((ImageDataMsg_t*)&message.payload)->image_data.n_pixels;

            printf("received test message, n_pixels %d, last_pixel: %d\n", n_pixels,
                   ((ImageDataMsg_t*)&message.payload)->image_data.pixel_data[n_pixels - 1].x_coordinate);

            break;
          };

          case LLCP_GET_FRAME_MSG_ID: {

            GetFrameMsg_t request = *((GetFrameMsg_t*) &message.payload);
            ntoh(&request);

            printf("acquisition started with time = %d ms\n", request.acquisition_time_ms);

            break;
          };
        }
      }
    }
  }

  return 0;
}
