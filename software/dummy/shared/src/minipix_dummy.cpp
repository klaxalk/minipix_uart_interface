#include <minipix_dummy.h>

/* constructor MinipixDummy() //{ */

MinipixDummy::MinipixDummy() {

  llcp_initialize(&llcp_receiver_);
}

//}

/* waitForAck() //{ */

void MinipixDummy::clearToSend(void) {

  // TODO timeout
  while (!clear_to_send_) {
  }
}

//}

void MinipixDummy::sendMessage([[maybe_unused]] const uint8_t *bytes_out, [[maybe_unused]] const uint16_t &len) {

  sendString(bytes_out, len);

  clear_to_send_ = false;
}

/* integralFrameMeasurement() //{ */

void MinipixDummy::ingegralFrameMeasurement(const uint16_t &acquisition_time) {

  printf("starting acquisition (%d ms)\n", acquisition_time);

  sleep(acquisition_time);

  for (int j = 0; j < 100; j++) {

    uint8_t n_pixels = 31;

    // create the message
    LLCP_FrameDataMsg_t image_data;
    init_LLCP_FrameDataMsg_t(&image_data);

    // | ------------------- fill in the payload ------------------ |

    image_data.payload.n_pixels = n_pixels;

    for (int i = 0; i < n_pixels; i++) {
      LLCP_PixelData_t *pixel = (LLCP_PixelData_t *)&image_data.payload.pixel_data[i];
      pixel->x_coordinate     = j;
      pixel->y_coordinate     = j;
      pixel->data[0]          = j;
      pixel->data[1]          = j;
      pixel->data[2]          = j;
      pixel->data[3]          = j;
      pixel->data[4]          = j;
      pixel->data[5]          = j;
    }

    // convert to network endian
    hton_LLCP_FrameDataMsg_t(&image_data);

    uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&image_data, sizeof(image_data), tx_buffer_);
    sendMessage(tx_buffer_, n_bytes);

    clearToSend();
  }
}

//}

/* update() //{ */

void MinipixDummy::update(void) {

  std::scoped_lock lock(mutex_message_buffer_);

  if (!message_buffer_.empty()) {

    LLCP_Message_t message = message_buffer_.front();
    message_buffer_.pop_front();

    switch (message.id) {
      case LLCP_MEASURE_FRAME_REQ_MSG_ID: {

        LLCP_MeasureFrameReqMsg_t *msg = (LLCP_MeasureFrameReqMsg_t *)(&message.payload);
        ntoh_LLCP_MeasureFrameReqMsg_t(msg);

        LLCP_MeasureFrameReq_t *req = (LLCP_MeasureFrameReq_t *)(&msg->payload);

        ingegralFrameMeasurement(req->acquisition_time_ms);

        break;
      };

      default: {
        break;
      };
    }
  }
}

//}

/* serialDataCallback() //{ */

void MinipixDummy::serialDataCallback(const uint8_t *bytes_in, const uint16_t &len) {

  for (uint16_t i = 0; i < len; i++) {

    LLCP_Message_t message;

    if (llcp_processChar(bytes_in[i], &llcp_receiver_, &message)) {

      switch (message.id) {

        case LLCP_MEASURE_FRAME_REQ_MSG_ID: {

          std::scoped_lock lock(mutex_message_buffer_);

          message_buffer_.push_back(message);

          break;
        };

        case LLCP_GET_STATUS_MSG_ID: {

          printf("received status request\n");

          // create the message
          LLCP_StatusMsg_t status_msg;
          init_LLCP_StatusMsg_t(&status_msg);

          // fill in the payload
          status_msg.payload.boot_count = boot_count_++;
          memset(&status_msg.payload.status_str, 0, sizeof(status_msg.payload.status_str));
          sprintf((char *)status_msg.payload.status_str, "Timepix3 is OK, but it is cold out here...");

          // convert to network endian
          hton_LLCP_StatusMsg_t(&status_msg);

          uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&status_msg, sizeof(status_msg), tx_buffer_);
          sendMessage(tx_buffer_, n_bytes);

          break;
        };

        case LLCP_ACK_MSG_ID: {

          clear_to_send_ = true;

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
