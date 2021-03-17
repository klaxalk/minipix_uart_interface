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

  if (llcp_processChar(byte_in, &gatherer_handler->llcp_receiver, &message_in)) {

    switch ((LLCP_MessageId_t)message_in.id) {

      case LLCP_GET_STATUS_MSG_ID: {

        gatherer_handler->fcns.getStatus(gatherer_handler->mui_handler_ptr_);

        break;
      };

      case LLCP_MEASURE_FRAME_MSG_ID: {

        LLCP_MeasureFrameReqMsg_t *msg = (LLCP_MeasureFrameReqMsg_t *)&message_in.payload;
        ntoh_LLCP_MeasureFrameReqMsg_t(msg);
        MeasureFrameReq_t *req = (MeasureFrameReq_t *)&msg->payload;

        gatherer_handler->fcns.measureFrame(gatherer_handler->mui_handler_ptr_, (uint16_t)req->acquisition_time_ms);

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

void gatherer_processImagePacket(Gatherer_Handler_t *gatherer_handler, const ImageData_t *image_data) {

  LLCP_ImageDataMsg_t msg;
  msg.message_id = LLCP_IMAGE_DATA_MSG_ID;
  msg.payload    = *image_data;
  hton_LLCP_ImageDataMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), gatherer_handler->tx_buffer);
  gatherer_handler->fcns.sendString(gatherer_handler->tx_buffer, n_bytes);
}

//}

/* gatherer_processStatus() //{ */

void gatherer_processStatus(Gatherer_Handler_t *gatherer_handler, const Status_t *status) {

  LLCP_StatusMsg_t msg;
  msg.message_id = LLCP_STATUS_MSG_ID;
  msg.payload    = *status;
  hton_LLCP_StatusMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), gatherer_handler->tx_buffer);
  gatherer_handler->fcns.sendString(gatherer_handler->tx_buffer, n_bytes);
}

//}
