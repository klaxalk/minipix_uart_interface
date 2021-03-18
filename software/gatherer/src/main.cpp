#include <llcp.h>
#include <llcp_minipix_messages.h>

#include <thread>
#include <chrono>

#include <serial_port.h>

#define SERIAL_BUFFER_SIZE 2048

std::string serial_port_file;
int         baud_rate;
bool        serial_port_virtual;

SerialPort serial_port;

uint8_t tx_buffer[SERIAL_BUFFER_SIZE];

/* getStatus() //{ */

void getStatus() {

  // create the message
  LLCP_GetStatusMsg_t msg;
  init_LLCP_GetStatusMsg_t(&msg);

  // convert to network endian
  hton_LLCP_GetStatusMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), tx_buffer);

  serial_port.sendCharArray(tx_buffer, n_bytes);
}

//}

/* measureFrame() //{ */

void measureFrame(const uint16_t& acquisition_time_ms) {

  // create the message
  LLCP_MeasureFrameReqMsg_t msg;
  init_LLCP_MeasureFrameReqMsg_t(&msg);

  // fill in the payload
  msg.payload.acquisition_time_ms = acquisition_time_ms;

  // convert to network endian
  hton_LLCP_MeasureFrameReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), tx_buffer);

  serial_port.sendCharArray(tx_buffer, n_bytes);
}

//}

/* startStream() //{ */

void startStream(const uint16_t& duty_cycle) {

  // create the message
  LLCP_MeasureStreamReqMsg_t msg;
  init_LLCP_MeasureStreamReqMsg_t(&msg);

  // fill in the payload
  msg.payload.duty_cycle_ms = duty_cycle;

  // convert to network endian
  hton_LLCP_MeasureStreamReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), tx_buffer);

  serial_port.sendCharArray(tx_buffer, n_bytes);
}

//}

/* maskPixel() //{ */

void maskPixel(const uint8_t& x, const uint8_t& y) {

  // create the message
  LLCP_UpdatePixelMaskReqMsg_t msg;
  init_LLCP_UpdatePixelMaskReqMsg_t(&msg);

  // fill in the payload
  msg.payload.masked       = 1;
  msg.payload.x_coordinate = x;
  msg.payload.y_coordinate = y;

  // convert to network endian
  hton_LLCP_UpdatePixelMaskReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), tx_buffer);

  serial_port.sendCharArray(tx_buffer, n_bytes);
}

//}

int main(int argc, char* argv[]) {

  if (argc == 4) {
    serial_port_file    = argv[1];
    baud_rate           = atoi(argv[2]);
    serial_port_virtual = atoi(argv[3]);

    printf("loaded params: %s, %d, %s\n", serial_port_file.c_str(), baud_rate, serial_port_virtual ? "VIRTUAL" : "REAL");
  } else {
    printf("params not supplied\n");
    return 0;
  }

  serial_port.connect(serial_port_file, baud_rate, serial_port_virtual);

  uint8_t  read_buffer[SERIAL_BUFFER_SIZE];
  uint16_t bytes_read;

  LLCP_Receiver_t llcp_receiver;
  llcp_initialize(&llcp_receiver);

  printf("Starting while loop\n");

  measureFrame(1200);
  /* maskPixel(10, 20); */
  /* startStream(200); */

  while (true) {

    // | --------- receive data from the minipix interface -------- |

    bytes_read = serial_port.readSerial(read_buffer, SERIAL_BUFFER_SIZE);

    if (bytes_read > 0) {

      // feed all the incoming bytes into the minipix interface
      for (uint16_t i = 0; i < bytes_read; i++) {

        LLCP_Message_t message_in;

        if (llcp_processChar(read_buffer[i], &llcp_receiver, &message_in)) {

          switch (message_in.id) {

            case LLCP_FRAME_DATA_MSG_ID: {

              LLCP_FrameDataMsg_t* msg = (LLCP_FrameDataMsg_t*)&message_in.payload;
              ntoh_LLCP_FrameDataMsg_t(msg);

              LLCP_FrameData_t* image = (LLCP_FrameData_t*)&msg->payload;

              uint8_t n_pixels = image->n_pixels;

              printf("received frame datam, id %d, n_pixels %d, last_pixel_x: %d\n", image->frame_id, n_pixels, image->pixel_data[n_pixels - 1].x_coordinate);

              break;
            };

            case LLCP_STREAM_DATA_MSG_ID: {

              LLCP_StreamDataMsg_t* msg = (LLCP_StreamDataMsg_t*)&message_in.payload;
              ntoh_LLCP_StreamDataMsg_t(msg);

              LLCP_StreamData_t* image = (LLCP_StreamData_t*)&msg->payload;

              uint8_t n_pixels = image->n_pixels;

              printf("received stream data, n_pixels %d, last_pixel_x: %d\n", n_pixels, image->pixel_data[n_pixels - 1].x_coordinate);

              break;
            };

            case LLCP_STATUS_MSG_ID: {

              LLCP_StatusMsg_t* msg = (LLCP_StatusMsg_t*)&message_in.payload;
              ntoh_LLCP_StatusMsg_t(msg);
              LLCP_Status_t* status = (LLCP_Status_t*)&msg->payload;

              printf("received status: boot count = %d, string: '%s'\n", status->boot_count, status->status_str);

              break;
            };

            default: {

              printf("Received unsupported message with id = %d\n", message_in.id);
            }
          }
        }
      }
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }
}
