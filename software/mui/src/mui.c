#include <mui.h>

/* mui_initialize() //{ */

void mui_initialize(MUI_Handler_t* mui_handler) {

  // initialize the inner state of the LLCP receiver
  llcp_initialize(&mui_handler->llcp_receiver);

  printf("[MinipixInterface]: initialized\n");
}

//}

/* mui_measureFrame() //{ */

void mui_measureFrame(MUI_Handler_t* mui_handler, const uint16_t acquisition_time) {

  LLCP_MeasureFrameReqMsg_t msg;
  msg.message_id                  = LLCP_MEASURE_FRAME_MSG_ID;
  msg.payload.acquisition_time_ms = acquisition_time;
  hton_LLCP_MeasureFrameReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), mui_handler->tx_buffer);

  mui_handler->fcns.sendString(mui_handler->tx_buffer, n_bytes);
}

//}

/* mui_getStatus() //{ */

void mui_getStatus(MUI_Handler_t* mui_handler) {

  LLCP_GetStatusMsg_t msg;
  msg.message_id = LLCP_GET_STATUS_MSG_ID;
  hton_LLCP_GetStatusMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), mui_handler->tx_buffer);

  mui_handler->fcns.sendString(mui_handler->tx_buffer, n_bytes);
}

//}

// | ------------- UART communication with MiniPIX ------------ |

/* mui_receiveCharCallback() //{ */

void mui_receiveCharCallback(MUI_Handler_t* mui_handler, const uint8_t byte_in) {

  LLCP_Message_t message_in;

  if (llcp_processChar(byte_in, &(mui_handler->llcp_receiver), &message_in)) {

    switch (message_in.id) {

      case LLCP_IMAGE_DATA_MSG_ID: {

        LLCP_ImageDataMsg_t* msg = (LLCP_ImageDataMsg_t*)&(message_in.payload);
        ntoh_LLCP_ImageDataMsg_t(msg);

        mui_handler->fcns.processImagePacket(&(msg->payload));

        break;
      };

      case LLCP_STATUS_MSG_ID: {

        LLCP_StatusMsg_t* msg = (LLCP_StatusMsg_t*)&(message_in.payload);
        ntoh_LLCP_StatusMsg_t(msg);

        mui_handler->fcns.processStatus(&(msg->payload));

        break;
      };

      default: {

        printf("Received unsupported message with id = %d\n", message_in.id);
      }
    }
  }
}

//}

// | -------------------- LED signalization ------------------- |

/* mui_ledSet() //{ */

void mui_ledSet(MUI_Handler_t* mui_handler, const bool new_state) {

  mui_handler->fcns.ledSetHW(new_state);

  mui_handler->led_state = new_state;
}

//}

/* mui_ledToggle() //{ */

void mui_ledToggle(MUI_Handler_t* mui_handler) {

  mui_ledSet(mui_handler, !mui_handler->led_state);
}

//}

// | --------------- the main update() function --------------- |

/* mui_update() //{ */

void mui_update(MUI_Handler_t* mui_handler) {

  // TODO process chars from the buffer

  mui_ledToggle(mui_handler);
  mui_handler->fcns.sleepHW((uint16_t)100);
}

//}
