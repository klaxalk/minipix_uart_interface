#include <gatherer_interface.h>

/* gatherer_initialize //{ */

void gatherer_initialize(Gatherer_Handler_t *gatherer_handler) {

  // initialize the inner state of the LLCP receiver
  llcp_initialize(&gatherer_handler->llcp_receiver);

  printf("[GathererInterface]: initialized\n");
}

//}

/* gatherer_receiveCharCallback() //{ */

void gatherer_receiveCharCallback(Gatherer_Handler_t *gatherer_handler, const uint8_t byte_in) {

  LLCP_Message_t message_in;

  if (llcp_processChar(byte_in, &(gatherer_handler->llcp_receiver), &message_in)) {

    switch (message_in.id) {

      case LLCP_GET_STATUS_MSG_ID: {

        mui_getStatus(gatherer_handler->mui_handler_ptr_);

        break;
      };

      case LLCP_MEASURE_FRAME_REQ_MSG_ID: {

        LLCP_MeasureFrameReqMsg_t *msg = (LLCP_MeasureFrameReqMsg_t *)(&message_in.payload);
        ntoh_LLCP_MeasureFrameReqMsg_t(msg);

        LLCP_MeasureFrameReq_t *req = (LLCP_MeasureFrameReq_t *)(&msg->payload);

        mui_measureFrame(gatherer_handler->mui_handler_ptr_, req->acquisition_time_ms);

        break;
      };

      default: {

        printf("Received unsupported message with id = %d\n", message_in.id);
      }
    }
  }
}

//}

// | -------------------- minipix->gatherer ------------------- |

/* gatherer_processImagePacket() //{ */

void gatherer_processImagePacket(Gatherer_Handler_t *gatherer_handler, const LLCP_FrameData_t *image_data) {

  LLCP_FrameDataMsg_t msg;
  init_LLCP_FrameDataMsg_t(&msg);

  msg.payload = *image_data;

  hton_LLCP_FrameDataMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), gatherer_handler->tx_buffer);
  gatherer_handler->fcns.sendString(gatherer_handler->tx_buffer, n_bytes);
}

//}

/* gatherer_processStatus() //{ */

void gatherer_processStatus(Gatherer_Handler_t *gatherer_handler, const LLCP_Status_t *status) {

  LLCP_StatusMsg_t msg;
  init_LLCP_StatusMsg_t(&msg);

  msg.payload = *status;

  hton_LLCP_StatusMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), gatherer_handler->tx_buffer);
  gatherer_handler->fcns.sendString(gatherer_handler->tx_buffer, n_bytes);
}

//}
