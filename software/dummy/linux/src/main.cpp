#define LLCP_LITTLE_ENDIAN

#include <llcp.h>
#include <llcp_minipix_messages.h>
#include <serial_port.h>

#include <thread>
#include <chrono>

#define SERIAL_BUFFER_SIZE 2048

int main() {

  // the hardware serial driver
  SerialPort serial_port;
  serial_port.connect("/tmp/ttyS0", 115200, true);

  // the bacaprotol receiver instance
  Receiver_t receiver;
  llcpInitialize(&receiver);

  printf("Dummy started\n");

  uint16_t boot_count_ = 0;

  while (true) {

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    uint8_t  read_buffer[LLCP_RX_TX_BUFFER_SIZE];
    uint16_t bytes_read;

    bytes_read = serial_port.readSerial(read_buffer, LLCP_RX_TX_BUFFER_SIZE);

    for (uint16_t i = 0; i < bytes_read; i++) {

      Message_t message;

      if (llcpProcessChar(read_buffer[i], &receiver, &message)) {

        switch ((MessageId_t)message.id) {

          case LLCP_MEASURE_FRAME_MSG_ID: {

            MeasureFrameMsg_t* request = (MeasureFrameMsg_t*)&message.payload;
            ntoh_MeasureFrameMsg_t(request);

            printf("starting acquisition (%d ms)\n", request->acquisition_time_ms);

            uint8_t n_pixels = 31;

            ImageDataMsg_t test_data;

            test_data.image_data.n_pixels = n_pixels;

            for (int i = 0; i < n_pixels; i++) {

              PixelData_t* pixel  = (PixelData_t*)&test_data.image_data.pixel_data[i];
              pixel->x_coordinate = i;
              pixel->y_coordinate = i;
              pixel->data[0]      = i;
              pixel->data[1]      = i;
              pixel->data[2]      = i;
              pixel->data[3]      = i;
              pixel->data[4]      = i;
              pixel->data[5]      = i;
            }

            uint8_t  tx_buffer[SERIAL_BUFFER_SIZE];
            uint16_t n_bytes = llcpPrepareMessage((uint8_t*)&test_data, sizeof(test_data), tx_buffer);

            if (!serial_port.sendCharArray(tx_buffer, n_bytes)) {
              printf("FAILED sending message with %d bytes\n", n_bytes);
            }

            break;
          };

          case LLCP_GET_STATUS_MSG_ID: {

            StatusMsg_t status;
            status.boot_count = boot_count_++;
            hton_StatusMsg_t(&status);

            uint8_t  tx_buffer[SERIAL_BUFFER_SIZE];
            uint16_t n_bytes = llcpPrepareMessage((uint8_t*)&status, sizeof(status), tx_buffer);

            if (!serial_port.sendCharArray(tx_buffer, n_bytes)) {
              printf("FAILED sending message with %d bytes\n", n_bytes);
            }

            break;
          };

          default: {

            printf("Received unsupported message with id = %d\n", message.id);
          }
        }
      }
    }
  }

  return 0;
}
