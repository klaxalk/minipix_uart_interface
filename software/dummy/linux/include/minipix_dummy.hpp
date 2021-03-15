#ifndef MINIPIX_DUMMY_H
#define MINIPIX_DUMMY_H

#include <stdint.h>

extern "C" {
#include <llcp.h>
#include <llcp_minipix_messages.h>
}

class MinipixDummy {

public:
  MinipixDummy();

  virtual void sendByte([[maybe_unused]] const uint8_t &byte_out){};
  virtual void sendString([[maybe_unused]] const uint8_t *bytes_out, [[maybe_unused]] const uint16_t &len){};
  void         serialDataCallback(const uint8_t *bytes_in, const uint16_t &len);

  virtual void update(void){};

protected:
  LLCP_Receiver_t llcp_receiver_;

  uint16_t boot_count_ = 0;

private:
  uint8_t tx_buffer_[LLCP_RX_TX_BUFFER_SIZE];
};

// constructor
inline MinipixDummy::MinipixDummy() {

  llcpInitialize(&llcp_receiver_);
}

inline void MinipixDummy::serialDataCallback(const uint8_t *bytes_in, const uint16_t &len) {

  for (uint16_t i = 0; i < len; i++) {

    LLCPMessage_t message;

    if (llcpProcessChar(bytes_in[i], &llcp_receiver_, &message)) {

      switch ((LLCPMessageId_t)message.id) {

        case LLCP_MEASURE_FRAME_MSG_ID: {

          MeasureFrameReqMsg_t *msg = (MeasureFrameReqMsg_t *)(&message.payload);
          ntoh_MeasureFrameReqMsg_t(msg);

          MeasureFrameReq_t *req = (MeasureFrameReq_t *)(&msg->payload);

          printf("starting acquisition (%d ms)\n", req->acquisition_time_ms);

          uint8_t n_pixels = 31;

          ImageDataMsg_t image_data;
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

          uint16_t n_bytes = llcpPrepareMessage((uint8_t *)&image_data, sizeof(image_data), tx_buffer_);
          sendString(tx_buffer_, n_bytes);

          break;
        };

        case LLCP_GET_STATUS_MSG_ID: {

          StatusMsg_t status_msg;
          status_msg.message_id         = LLCP_STATUS_MSG_ID;
          status_msg.payload.boot_count = boot_count_++;
          memset(&status_msg.payload.status_str, 0, sizeof(status_msg.payload.status_str));
          sprintf((char *)status_msg.payload.status_str, "Timepix3 is OK, but it is cold out here...");
          hton_StatusMsg_t(&status_msg);

          uint16_t n_bytes = llcpPrepareMessage((uint8_t *)&status_msg, sizeof(status_msg), tx_buffer_);
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

#endif  // MINIPIX_DUMMY_H
