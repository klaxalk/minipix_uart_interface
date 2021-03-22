#include <gatherer_interface.h>

/* gatherer_initialize //{ */

void gatherer_initialize(Gatherer_Handler_t *gatherer_handler) {

  // initialize the inner state of the LLCP receiver
  llcp_initialize(&gatherer_handler->llcp_receiver);
}

//}

/* gatherer_receiveCharCallback() //{ */

void gatherer_receiveCharCallback(Gatherer_Handler_t *gatherer_handler, const uint8_t byte_in) {

  LLCP_Message_t message_in;

  if (llcp_processChar(byte_in, &(gatherer_handler->llcp_receiver), &message_in)) {

    switch (message_in.id) {

      case LLCP_GET_STATUS_REQ_MSG_ID: {

        mui_getStatus(gatherer_handler->mui_handler_ptr_);

        break;
      };

      case LLCP_PWR_REQ_MSG_ID: {

        LLCP_PwrReqMsg_t *msg = (LLCP_PwrReqMsg_t *)(&message_in.payload);
        ntoh_LLCP_PwrReqMsg_t(msg);

        LLCP_PwrReq_t *req = (LLCP_PwrReq_t *)(&msg->payload);

        mui_pwr(gatherer_handler->mui_handler_ptr_, req->state);

        break;
      };

      case LLCP_MEASURE_FRAME_REQ_MSG_ID: {

        LLCP_MeasureFrameReqMsg_t *msg = (LLCP_MeasureFrameReqMsg_t *)(&message_in.payload);
        ntoh_LLCP_MeasureFrameReqMsg_t(msg);

        LLCP_MeasureFrameReq_t *req = (LLCP_MeasureFrameReq_t *)(&msg->payload);

        mui_measureFrame(gatherer_handler->mui_handler_ptr_, req->acquisition_time_ms);

        break;
      };

      case LLCP_MEASURE_STREAM_REQ_MSG_ID: {

        LLCP_MeasureStreamReqMsg_t *msg = (LLCP_MeasureStreamReqMsg_t *)(&message_in.payload);
        ntoh_LLCP_MeasureStreamReqMsg_t(msg);

        LLCP_MeasureStreamReq_t *req = (LLCP_MeasureStreamReq_t *)(&msg->payload);

        mui_measureStream(gatherer_handler->mui_handler_ptr_, req->duty_cycle_ms);

        break;
      };

      case LLCP_UPDATE_PIXEL_MASK_REQ_MSG_ID: {

        LLCP_UpdatePixelMaskReqMsg_t *msg = (LLCP_UpdatePixelMaskReqMsg_t *)(&message_in.payload);
        ntoh_LLCP_UpdatePixelMaskReqMsg_t(msg);

        LLCP_UpdatePixelMaskReq_t *req = (LLCP_UpdatePixelMaskReq_t *)(&msg->payload);

        mui_updatePixelMask(gatherer_handler->mui_handler_ptr_, req);

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
  gatherer_handler->fcns.sendString(gatherer_handler->tx_buffer, n_bytes);
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
  gatherer_handler->fcns.sendString(gatherer_handler->tx_buffer, n_bytes);
}

//}

/* gatherer_processStreamData() //{ */

void gatherer_processStreamData(Gatherer_Handler_t *gatherer_handler, const LLCP_StreamData_t *data) {

  // create the message
  LLCP_StreamDataMsg_t msg;
  init_LLCP_StreamDataMsg_t(&msg);

  // fill in the payload
  msg.payload = *data;

  // convert it to the network endian
  hton_LLCP_StreamDataMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), gatherer_handler->tx_buffer);
  gatherer_handler->fcns.sendString(gatherer_handler->tx_buffer, n_bytes);
}

//}

/* gatherer_processStatus() //{ */

void gatherer_processStatus(Gatherer_Handler_t *gatherer_handler, const LLCP_Status_t *status) {

  // create the message
  LLCP_StatusMsg_t msg;
  init_LLCP_StatusMsg_t(&msg);

  // fill in the payload
  msg.payload = *status;

  // convert it to the network endian
  hton_LLCP_StatusMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), gatherer_handler->tx_buffer);
  gatherer_handler->fcns.sendString(gatherer_handler->tx_buffer, n_bytes);
}

//}
