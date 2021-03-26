#include <minipix_dummy.h>

/* constructor MinipixDummy() //{ */

MinipixDummy::MinipixDummy() {

  llcp_initialize(&llcp_receiver_);
}

//}

/* sendMessage() //{ */

void MinipixDummy::sendMessage([[maybe_unused]] const uint8_t *bytes_out, [[maybe_unused]] const uint16_t &len) {

  // TODO use condition_varialbe to pause the thread
  while (!clear_to_send_) {
    sleep(1);
  }

  sendString(bytes_out, len);

  clear_to_send_ = false;
}

//}

/* sendMessageNoAck() //{ */

void MinipixDummy::sendMessageNoAck([[maybe_unused]] const uint8_t *bytes_out, [[maybe_unused]] const uint16_t &len) {

  sendString(bytes_out, len);
}

//}

/* sendAck() //{ */

void MinipixDummy::sendAck(void) {

  LLCP_AckMsg_t ack_msg;
  init_LLCP_AckMsg_t(&ack_msg);

  hton_LLCP_AckMsg_t(&ack_msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&ack_msg, sizeof(ack_msg), tx_buffer_);
  sendMessageNoAck(tx_buffer_, n_bytes);
}

//}

/* testStripe() //{ */

void MinipixDummy::testStripe() {

  const int n_packets = 100;

  uint16_t packet_id = 0;

  for (int j = 0; j < n_packets; j++) {

    uint8_t n_pixels = LLCP_FRAME_DATA_N_PIXELS;

    // create the message
    LLCP_FrameDataMsg_t image_data;
    init_LLCP_FrameDataMsg_t(&image_data);

    // | ------------------- fill in the payload ------------------ |

    image_data.payload.frame_id  = frame_id_;
    image_data.payload.mode      = LLCP_TPX3_PXL_MODE_TOA_TOT;
    image_data.payload.n_pixels  = n_pixels;
    image_data.payload.packet_id = packet_id++;

    for (int i = 0; i < n_pixels; i++) {
      LLCP_PixelDataToAToT_t *pixel = (LLCP_PixelDataToAToT_t *)&image_data.payload.pixel_data[i];
      pixel->address                = (j + i) + 256 * j;
      pixel->tot                    = i;
      pixel->toa                    = j * 41 + i;
      pixel->ftoa                   = 0;
    }

    // convert to network endian
    hton_LLCP_FrameDataMsg_t(&image_data);

    uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&image_data, sizeof(image_data), tx_buffer_);

    sendMessage(tx_buffer_, n_bytes);
  }

  // | ---------------- send FrameDataTerminator ---------------- |

  // create the message
  LLCP_FrameDataTerminatorMsg_t terminator;
  init_LLCP_FrameDataTerminatorMsg_t(&terminator);

  terminator.payload.frame_id  = frame_id_;
  terminator.payload.n_packets = n_packets;

  hton_LLCP_FrameDataTerminatorMsg_t(&terminator);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&terminator, sizeof(terminator), tx_buffer_);

  sendMessage(tx_buffer_, n_bytes);
}

//}

/* continuousStreamMeasurement() //{ */

void MinipixDummy::continuousStreamMeasurement() {

  uint16_t duty_cycle = stream_measurement_duty_cycle;  // TODO mutex

  printf("starting stream acquisition (duty %d/1000)\n", duty_cycle);

  uint16_t sleep_time;
  if (duty_cycle < 1000) {
    sleep_time = 1000 - duty_cycle;
  } else {
    sleep_time = 0;
  }

  sleep(sleep_time);

  for (int j = 0; j < duty_cycle; j++) {

    uint8_t n_pixels = 31;

    // create the message
    LLCP_StreamDataMsg_t image_data;
    init_LLCP_StreamDataMsg_t(&image_data);

    // | ------------------- fill in the payload ------------------ |

    image_data.payload.n_pixels = n_pixels;

    for (int i = 0; i < n_pixels; i++) {
      LLCP_PixelDataToAToT_t *pixel = (LLCP_PixelDataToAToT_t *)&image_data.payload.pixel_data[i];
      pixel->address                = j + j * 256;
      pixel->tot                    = j;
      pixel->toa                    = j;
      pixel->ftoa                   = j;
    }

    // convert to network endian
    hton_LLCP_StreamDataMsg_t(&image_data);

    uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&image_data, sizeof(image_data), tx_buffer_);
    sendMessage(tx_buffer_, n_bytes);

    sleep(1);
  }
}

//}

/* update() //{ */

void MinipixDummy::update(void) {

  {
    std::scoped_lock lock(mutex_message_buffer_);

    if (!message_buffer_.empty()) {

      LLCP_Message_t message = message_buffer_.front();
      message_buffer_.pop_front();

      // I know the less pointers the beeter, but I want the following switch
      // to look the same as the rest and in ANSI C, to minimize copy-paster errors.
      LLCP_Message_t *message_in = &message;

      switch (message_in->id) {

        case LLCP_MEASURE_FRAME_REQ_MSG_ID: {

          printf("processing frame measurement request from the queue\n");

          LLCP_MeasureFrameReqMsg_t *msg = (LLCP_MeasureFrameReqMsg_t *)message_in;
          ntoh_LLCP_MeasureFrameReqMsg_t(msg);

          LLCP_MeasureFrameReq_t *req = (LLCP_MeasureFrameReq_t *)(&msg->payload);

          if (powered_) {
            simulateImageAcquisition(req->acquisition_time_ms);
          } else {
            printf("cannot do frame measurement, not powered!\n");
          }

          break;
        };

        case LLCP_MEASURE_STREAM_REQ_MSG_ID: {

          printf("processing stream measurement request from the queue\n");

          LLCP_MeasureStreamReqMsg_t *msg = (LLCP_MeasureStreamReqMsg_t *)&message_in;
          ntoh_LLCP_MeasureStreamReqMsg_t(msg);

          LLCP_MeasureStreamReq_t *req = (LLCP_MeasureStreamReq_t *)(&msg->payload);

          if (powered_) {
            stream_measurement_duty_cycle = req->duty_cycle_ms;
            stream_measurement_on_        = true;
          } else {
            printf("cannot do stream measurement, not powered!\n");
          }

          break;
        };

        default: {
          break;
        };
      }
    }
  }

  if (stream_measurement_on_) {
    continuousStreamMeasurement();
  } else {
    sleep(5);
  }
}

//}

/* serialDataCallback() //{ */

void MinipixDummy::serialDataCallback(const uint8_t *bytes_in, const uint16_t &len) {

  for (uint16_t i = 0; i < len; i++) {

    LLCP_Message_t *message_in;

    bool checksum_matched;

    if (llcp_processChar(bytes_in[i], &llcp_receiver_, &message_in, &checksum_matched)) {

      switch (message_in->id) {

        case LLCP_MEASURE_FRAME_REQ_MSG_ID: {

          printf("received frame measurement request\n");

          std::scoped_lock lock(mutex_message_buffer_);

          message_buffer_.push_back(*message_in);

          break;
        };

        case LLCP_MEASURE_STREAM_REQ_MSG_ID: {

          printf("received stream measurement request\n");

          std::scoped_lock lock(mutex_message_buffer_);

          message_buffer_.push_back(*message_in);

          break;
        };

        case LLCP_PWR_REQ_MSG_ID: {

          LLCP_PwrReqMsg_t *msg = (LLCP_PwrReqMsg_t *)message_in;
          ntoh_LLCP_PwrReqMsg_t(msg);

          LLCP_PwrReq_t *req = (LLCP_PwrReq_t *)(&msg->payload);

          printf("detector power %s\n", req->state == 1 ? "ON" : "OFF");

          powered_ = req->state == 1 ? true : false;

          break;
        };

        case LLCP_UPDATE_PIXEL_MASK_REQ_MSG_ID: {

          LLCP_UpdatePixelMaskReqMsg_t *msg = (LLCP_UpdatePixelMaskReqMsg_t *)message_in;
          ntoh_LLCP_UpdatePixelMaskReqMsg_t(msg);

          LLCP_UpdatePixelMaskReq_t *req = (LLCP_UpdatePixelMaskReq_t *)(&msg->payload);

          printf("received pixel mask update, x = %d, y = %d, masked = %s\n", req->x_coordinate, req->y_coordinate, req->masked ? "MASK" : "UNMASK");

          sendAck();

          break;
        };

        case LLCP_SET_THRESHOLD_REQ_MSG_ID: {

          LLCP_SetThresholdReqMsg_t *msg = (LLCP_SetThresholdReqMsg_t *)message_in;
          ntoh_LLCP_SetThresholdReqMsg_t(msg);

          LLCP_SetThresholdReq_t *req = (LLCP_SetThresholdReq_t *)(&msg->payload);

          printf("setting threshold to %d\n", req->threshold);

          sendAck();

          break;
        };

        case LLCP_SET_CONFIGURATION_PRESET_REQ_MSG_ID: {

          LLCP_SetConfigurationPresetReqMsg_t *msg = (LLCP_SetConfigurationPresetReqMsg_t *)message_in;
          ntoh_LLCP_SetConfigurationPresetReqMsg_t(msg);

          LLCP_SetConfigurationPresetReq_t *req = (LLCP_SetConfigurationPresetReq_t *)(&msg->payload);

          printf("setting configuration preset to %d\n", req->preset);

          sendAck();

          break;
        };

        case LLCP_GET_STATUS_REQ_MSG_ID: {

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

          LLCP_AckMsg_t *msg = (LLCP_AckMsg_t *)message_in;
          ntoh_LLCP_AckMsg_t(msg);
          LLCP_Ack_t *ack = (LLCP_Ack_t *)&msg->payload;

          printf("received ack: %s\n", ack->success ? "true" : "false");

          // we received acks from MUI, we are clear to send
          clear_to_send_ = true;

          break;
        };

        default: {

          printf("Received unsupported message with id = %d\n", message_in->id);
        }
      }
    }
  }
}

//}
