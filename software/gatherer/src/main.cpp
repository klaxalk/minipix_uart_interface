#include <llcp.h>
#include <llcp_minipix_messages.h>

#include <thread>
#include <chrono>

#include <serial_port.h>

#define SERIAL_BUFFER_SIZE 2048

std::string serial_port_file;
int         baud_rate;
bool        serial_port_virtual;

int main(int argc, char* argv[]) {

  if (argc == 4) {
    serial_port_file    = argv[1];
    baud_rate           = atoi(argv[2]);
    serial_port_virtual = argv[3];

    printf("loaded params: %s, %d, %s\n", serial_port_file.c_str(), baud_rate, serial_port_virtual ? "VIRTUAL" : "REAL");
  } else {
    printf("params not supplied\n");
    return 0;
  }

  SerialPort serial_port;

  serial_port.connect(serial_port_file, baud_rate, serial_port_virtual);

  uint8_t  read_buffer[SERIAL_BUFFER_SIZE];
  uint8_t  tx_buffer[SERIAL_BUFFER_SIZE];
  uint16_t bytes_read;

  LLCP_Receiver_t llcp_receiver;
  llcp_initialize(&llcp_receiver);

  printf("Starting while loop\n");

  while (true) {

    {
      LLCP_GetStatusMsg_t msg;
      msg.message_id = LLCP_GET_STATUS_MSG_ID;
      hton_LLCP_GetStatusMsg_t(&msg);

      uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), tx_buffer);

      serial_port.sendCharArray(tx_buffer, n_bytes);
    }

    {
      LLCP_MeasureFrameReqMsg_t msg;
      msg.message_id                  = LLCP_MEASURE_FRAME_MSG_ID;
      msg.payload.acquisition_time_ms = 333;
      hton_LLCP_MeasureFrameReqMsg_t(&msg);

      uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), tx_buffer);

      serial_port.sendCharArray(tx_buffer, n_bytes);
    }

    // | --------- receive data from the minipix interface -------- |

    bytes_read = serial_port.readSerial(read_buffer, SERIAL_BUFFER_SIZE);

    // feed all the incoming bytes into the minipix interface
    for (uint16_t i = 0; i < bytes_read; i++) {

      LLCP_Message_t message_in;

      if (llcp_processChar(read_buffer[i], &llcp_receiver, &message_in)) {

        switch ((LLCP_MessageId_t)message_in.id) {

          case LLCP_IMAGE_DATA_MSG_ID: {

            LLCP_ImageDataMsg_t* msg = (LLCP_ImageDataMsg_t*)&message_in.payload;
            ntoh_LLCP_ImageDataMsg_t(msg);

            ImageData_t* image = (ImageData_t*)&msg->payload;

            uint8_t n_pixels = image->n_pixels;

            printf("received image data, n_pixels %d, last_pixel_x: %d\n", n_pixels, image->pixel_data[n_pixels - 1].x_coordinate);

            break;
          };

          case LLCP_STATUS_MSG_ID: {

            LLCP_StatusMsg_t* msg = (LLCP_StatusMsg_t*)&message_in.payload;
            ntoh_LLCP_StatusMsg_t(msg);
            Status_t* status = (Status_t*)&msg->payload;

            printf("received status: boot count = %d, string: '%s'\n", status->boot_count, status->status_str);

            break;
          };

          default: {

            printf("Received unsupported message with id = %d\n", message_in.id);
          }
        }
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}
