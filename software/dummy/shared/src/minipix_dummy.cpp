#include <minipix_dummy.h>

/* constructor MinipixDummy() //{ */

MinipixDummy::MinipixDummy() {

  llcp_initialize(&llcp_receiver_);
}

//}

/* serialDataCallback() //{ */

void MinipixDummy::serialDataCallback(const uint8_t *bytes_in, const uint16_t &len) {

  for (uint16_t i = 0; i < len; i++) {

    LLCP_Message_t message;

    if (llcp_processChar(bytes_in[i], &llcp_receiver_, &message)) {

      switch (message.id) {

        case LLCP_MEASURE_FRAME_MSG_ID: {

          LLCP_MeasureFrameReqMsg_t *msg = (LLCP_MeasureFrameReqMsg_t *)(&message.payload);
          ntoh_LLCP_MeasureFrameReqMsg_t(msg);

          MeasureFrameReq_t *req = (MeasureFrameReq_t *)(&msg->payload);

          printf("starting acquisition (%d ms)\n", req->acquisition_time_ms);

          uint8_t n_pixels = 31;

          LLCP_ImageDataMsg_t image_data;
          image_data.message_id = LLCP_IMAGE_DATA_MSG_ID;

          image_data.payload.n_pixels = n_pixels;

          for (int i = 0; i < n_pixels; i++) {
            PixelData_t *pixel  = (PixelData_t *)&image_data.payload.pixel_data[i];
            pixel->x_coordinate = i;
            pixel->y_coordinate = i;
            pixel->data[0]      = i;
            pixel->data[1]      = i;
            pixel->data[2]      = i;
            pixel->data[3]      = i;
            pixel->data[4]      = i;
            pixel->data[5]      = i;
          }

          uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&image_data, sizeof(image_data), tx_buffer_);
          sendString(tx_buffer_, n_bytes);

          break;
        };

        case LLCP_GET_STATUS_MSG_ID: {

          printf("received status request\n");

          LLCP_StatusMsg_t status_msg;
          status_msg.message_id         = LLCP_STATUS_MSG_ID;
          status_msg.payload.boot_count = boot_count_++;
          memset(&status_msg.payload.status_str, 0, sizeof(status_msg.payload.status_str));
          sprintf((char *)status_msg.payload.status_str, "Timepix3 is OK, but it is cold out here...");
          hton_LLCP_StatusMsg_t(&status_msg);

          uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&status_msg, sizeof(status_msg), tx_buffer_);
          sendString(tx_buffer_, n_bytes);

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
