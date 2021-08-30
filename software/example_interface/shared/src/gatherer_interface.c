#include <gatherer_interface.h>

/* gatherer_initialize //{ */

void gatherer_initialize(Gatherer_Handler_t *gatherer_handler) {

  // initialize the inner state of the LLCP receiver
  llcp_initialize(&gatherer_handler->llcp_receiver);
}

//}

/* gatherer_receiveCharCallback() //{ */

void gatherer_receiveCharCallback(Gatherer_Handler_t *gatherer_handler, const uint8_t byte_in) {

  LLCP_Message_t *message_in;

  bool checksum_matched;

  if (llcp_processChar(byte_in, &(gatherer_handler->llcp_receiver), &message_in, &checksum_matched)) {

    switch (message_in->id) {

      case LLCP_GET_STATUS_REQ_MSG_ID: {

        mui_getStatus(gatherer_handler->mui_handler_ptr_);

        break;
      };

      case LLCP_GET_FRAME_DATA_REQ_MSG_ID: {

        mui_getFrameData(gatherer_handler->mui_handler_ptr_);

        break;
      };

      case LLCP_GET_TEMPERATURE_REQ_MSG_ID: {

        mui_getTemperature(gatherer_handler->mui_handler_ptr_);

        break;
      };

      case LLCP_PWR_REQ_MSG_ID: {

        LLCP_PwrReqMsg_t *msg = (LLCP_PwrReqMsg_t *)&message_in;
        ntoh_LLCP_PwrReqMsg_t(msg);

        LLCP_PwrReq_t *req = (LLCP_PwrReq_t *)(&msg->payload);

        mui_pwr(gatherer_handler->mui_handler_ptr_, req->state);

        break;
      };

      case LLCP_MEASURE_FRAME_REQ_MSG_ID: {

        LLCP_MeasureFrameReqMsg_t *msg = (LLCP_MeasureFrameReqMsg_t *)message_in;
        ntoh_LLCP_MeasureFrameReqMsg_t(msg);

        LLCP_MeasureFrameReq_t *req = (LLCP_MeasureFrameReq_t *)(&msg->payload);

        mui_measureFrame(gatherer_handler->mui_handler_ptr_, req->acquisition_time_ms, req->mode);

        break;
      };

      case LLCP_UPDATE_PIXEL_MASK_REQ_MSG_ID: {

        LLCP_UpdatePixelMaskReqMsg_t *msg = (LLCP_UpdatePixelMaskReqMsg_t *)message_in;
        ntoh_LLCP_UpdatePixelMaskReqMsg_t(msg);

        LLCP_UpdatePixelMaskReq_t *req = (LLCP_UpdatePixelMaskReq_t *)(&msg->payload);

        mui_updatePixelMask(gatherer_handler->mui_handler_ptr_, req);

        break;
      };

      case LLCP_SET_THRESHOLD_REQ_MSG_ID: {

        LLCP_SetThresholdReqMsg_t *msg = (LLCP_SetThresholdReqMsg_t *)message_in;
        ntoh_LLCP_SetThresholdReqMsg_t(msg);

        LLCP_SetThresholdReq_t *req = (LLCP_SetThresholdReq_t *)(&msg->payload);

        mui_setThreshold(gatherer_handler->mui_handler_ptr_, req->threshold_coarse, req->threshold_fine);

        break;
      };

      case LLCP_SET_CONFIGURATION_PRESET_REQ_MSG_ID: {

        LLCP_SetConfigurationPresetReqMsg_t *msg = (LLCP_SetConfigurationPresetReqMsg_t *)message_in;
        ntoh_LLCP_SetConfigurationPresetReqMsg_t(msg);

        LLCP_SetConfigurationPresetReq_t *req = (LLCP_SetConfigurationPresetReq_t *)(&msg->payload);

        mui_setConfigurationPreset(gatherer_handler->mui_handler_ptr_, req->preset);

        break;
      };

      case LLCP_ACK_MSG_ID: {

        // load up the message and convert it to our endian
        LLCP_AckMsg_t *msg = (LLCP_AckMsg_t *)message_in;
        ntoh_LLCP_AckMsg_t(msg);

        mui_sendAck(gatherer_handler->mui_handler_ptr_, msg);

        break;
      };

      default: {

        // received unsupported message
        break;
      }
    }
  }
}

//}

// | -------------------- minipix->gatherer ------------------- |

/* gatherer_processFrameData() //{ */

void gatherer_processFrameData(Gatherer_Handler_t *gatherer_handler, const LLCP_FrameData_t *data) {

  // create the message
  LLCP_FrameDataMsg_t msg;
  init_LLCP_FrameDataMsg_t(&msg);

  // fill in the payload
  msg.payload = *data;

  // convert it to the network endian
  hton_LLCP_FrameDataMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), gatherer_handler->tx_buffer);
  gatherer_sendMessage(gatherer_handler, gatherer_handler->tx_buffer, n_bytes);
}

//}

/* gatherer_processFrameTerminatorData() //{ */

void gatherer_processFrameDataTerminator(Gatherer_Handler_t *gatherer_handler, const LLCP_FrameDataTerminator_t *data) {

  // create the message
  LLCP_FrameDataTerminatorMsg_t msg;
  init_LLCP_FrameDataTerminatorMsg_t(&msg);

  // fill in the payload
  msg.payload = *data;

  // convert it to the network endian
  hton_LLCP_FrameDataTerminatorMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), gatherer_handler->tx_buffer);
  gatherer_sendMessage(gatherer_handler, gatherer_handler->tx_buffer, n_bytes);
}

//}

/* gatherer_processStatus() //{ */

void gatherer_processStatus(Gatherer_Handler_t *gatherer_handler, const LLCP_Status_t *data) {

  // create the message
  LLCP_StatusMsg_t msg;
  init_LLCP_StatusMsg_t(&msg);

  // fill in the payload
  msg.payload = *data;

  // convert it to the network endian
  hton_LLCP_StatusMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), gatherer_handler->tx_buffer);
  gatherer_sendMessage(gatherer_handler, gatherer_handler->tx_buffer, n_bytes);
}

//}

/* gatherer_processTemperature() //{ */

void gatherer_processTemperature(Gatherer_Handler_t *gatherer_handler, const LLCP_Temperature_t *data) {

  // create the message
  LLCP_TemperatureMsg_t msg;
  init_LLCP_TemperatureMsg_t(&msg);

  // fill in the payload
  msg.payload = *data;

  // convert it to the network endian
  hton_LLCP_TemperatureMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), gatherer_handler->tx_buffer);
  gatherer_sendMessage(gatherer_handler, gatherer_handler->tx_buffer, n_bytes);
}

//}

/* gatherer_processAck() //{ */

void gatherer_processAck(Gatherer_Handler_t *gatherer_handler, const LLCP_Ack_t *data) {

  // create the message
  LLCP_AckMsg_t msg;
  init_LLCP_AckMsg_t(&msg);

  // fill in the payload
  msg.payload = *data;

  // convert it to the network endian
  hton_LLCP_AckMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), gatherer_handler->tx_buffer);
  gatherer_sendMessage(gatherer_handler, gatherer_handler->tx_buffer, n_bytes);
}

//}

/* gatherer_processMinipixError() //{ */

void gatherer_processMinipixError(Gatherer_Handler_t *gatherer_handler, const LLCP_MinipixError_t *data) {

  // create the message
  LLCP_MinipixErrorMsg_t msg;
  init_LLCP_MinipixErrorMsg_t(&msg);

  // fill in the payload
  msg.payload = *data;

  // convert it to the network endian
  hton_LLCP_MinipixErrorMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), gatherer_handler->tx_buffer);
  gatherer_sendMessage(gatherer_handler, gatherer_handler->tx_buffer, n_bytes);
}

//}

/* gatherer_processFrameMeasurementFinished() //{ */

void gatherer_processFrameMeasurementFinished(Gatherer_Handler_t *gatherer_handler) {

  // create the message
  LLCP_FrameMeasurementFinishedMsg_t msg;
  init_LLCP_FrameMeasurementFinishedMsg_t(&msg);

  // convert it to the network endian
  hton_LLCP_FrameMeasurementFinishedMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), gatherer_handler->tx_buffer);
  gatherer_sendMessage(gatherer_handler, gatherer_handler->tx_buffer, n_bytes);
}

//}

// | ------------------------- private ------------------------ |

/* gatherer_sendMessage() //{ */

void gatherer_sendMessage(Gatherer_Handler_t *gatherer_handler, const uint8_t *str_out, const uint16_t len) {

#if GATHERER_SEND_STRING == 1

  gatherer_handler->fcns.sendString(str_out, len);

#elif GATHERER_SEND_CHAR == 1

  for (uint16_t i = 0; i < len; i++) {
    gatherer_handler->fcns.sendChar(str_out[i]);
  }

#else
#error "Gatherer interface will not work, neither GATHERER_SEND_STRING or GATHERER_SEND_CHAR needs to be 1"
  UNUSED(gatherer_handler);
  UNUSED(str_out);
  UNUSED(len);
#endif
}

//}
