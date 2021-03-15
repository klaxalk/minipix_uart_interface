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
  serial_port.connect("/tmp/ttyS1", 115200, true);

  // the bacaprotol receiver instance
  Receiver_t receiver;
  llcpInitialize(&receiver);

  printf("Sender started\n");

  while (true) {

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    {
      GetFrameMsg_t get_frame;
      get_frame.acquisition_time_ms = 1213;
      hton_GetFrameMsg_t(&get_frame);

      uint8_t  tx_buffer[SERIAL_BUFFER_SIZE];
      uint16_t n_bytes = llcpPrepareMessage((uint8_t*)&get_frame, sizeof(get_frame), tx_buffer);

      if (!serial_port.sendCharArray(tx_buffer, n_bytes)) {
        printf("FAILED sending message with %d bytes\n", n_bytes);
      }
    }

    {
      GetStatusMsg_t get_status;
      hton_GetStatusMsg_t(&get_status);

      uint8_t  tx_buffer[SERIAL_BUFFER_SIZE];
      uint16_t n_bytes = llcpPrepareMessage((uint8_t*)&get_status, sizeof(get_status), tx_buffer);

      if (!serial_port.sendCharArray(tx_buffer, n_bytes)) {
        printf("FAILED sending message with %d bytes\n", n_bytes);
      }
    }

    uint8_t  read_buffer[LLCP_RX_TX_BUFFER_SIZE];
    uint16_t bytes_read;

    bytes_read = serial_port.readSerial(read_buffer, LLCP_RX_TX_BUFFER_SIZE);

    for (uint16_t i = 0; i < bytes_read; i++) {

      Message_t message;

      if (llcpProcessChar(read_buffer[i], &receiver, &message)) {

        switch ((MessageId_t)message.id) {

          case LLCP_IMAGE_DATA_MSG_ID: {

            ImageDataMsg_t* msg = (ImageDataMsg_t*)&message.payload;
            ntoh_ImageDataMsg_t(msg);

            uint8_t n_pixels = msg->image_data.n_pixels;

            printf("received image data, n_pixels %d, last_pixel_x: %d\n", n_pixels, msg->image_data.pixel_data[n_pixels - 1].x_coordinate);

            break;
          };

          case LLCP_STATUS_MSG_ID: {

            StatusMsg_t* status = (StatusMsg_t*)&message.payload;
            ntoh_StatusMsg_t(status);

            printf("status, boot count = %d\n", status->boot_count);

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
