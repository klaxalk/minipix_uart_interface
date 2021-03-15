#include <minipix_dummy_linux.h>

/* constructor MinipixDummyLinux() //{ */

MinipixDummyLinux::MinipixDummyLinux(void) {
}

//}

/* initializeSerialPort() //{ */

void MinipixDummyLinux::initializeSerialPort(const std::string& file, const int& baud, const bool virtual_port) {

  serial_port_.connect(file, baud, virtual_port);
}

//}

/* update() //{ */

void MinipixDummyLinux::update(void) {

  uint16_t bytes_read = serial_port_.readSerial(rx_buffer_, SERIAL_BUFFER_SIZE);

  for (uint16_t i = 0; i < bytes_read; i++) {

    LLCPMessage_t message;

    if (llcpProcessChar(rx_buffer_[i], &llcp_receiver_, &message)) {

      switch ((LLCPMessageId_t)message.id) {

        case LLCP_MEASURE_FRAME_MSG_ID: {

          MeasureFrameReqMsg_t* msg = (MeasureFrameReqMsg_t*)(&message.payload);
          ntoh_MeasureFrameReqMsg_t(msg);

          MeasureFrameReq_t* req = (MeasureFrameReq_t*)(&msg->payload);

          printf("starting acquisition (%d ms)\n", req->acquisition_time_ms);

          uint8_t n_pixels = 31;

          ImageDataMsg_t image_data;
          image_data.message_id = LLCP_IMAGE_DATA_MSG_ID;

          image_data.payload.n_pixels = n_pixels;

          for (int i = 0; i < n_pixels; i++) {
            PixelData_t* pixel  = (PixelData_t*)&image_data.payload.pixel_data[i];
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
          uint16_t n_bytes = llcpPrepareMessage((uint8_t*)&image_data, sizeof(image_data), tx_buffer);

          if (!serial_port_.sendCharArray(tx_buffer, n_bytes)) {
            printf("FAILED sending message with %d bytes\n", n_bytes);
          }

          break;
        };

        case LLCP_GET_STATUS_MSG_ID: {

          StatusMsg_t status;
          status.message_id         = LLCP_STATUS_MSG_ID;
          status.payload.boot_count = boot_count_++;
          memset(&status.payload.status_str, 0, sizeof(status.payload.status_str));
          sprintf((char*)status.payload.status_str, "Timepix3 is OK, but it is cold out here...");
          hton_StatusMsg_t(&status);

          uint8_t  tx_buffer[SERIAL_BUFFER_SIZE];
          uint16_t n_bytes = llcpPrepareMessage((uint8_t*)&status, sizeof(status), tx_buffer);

          if (!serial_port_.sendCharArray(tx_buffer, n_bytes)) {
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

//}
