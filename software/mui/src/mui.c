#include <mui.h>

// --------------------------------------------------------------
// |                           public                           |
// --------------------------------------------------------------

/* mui_initialize() //{ */

void mui_initialize(MUI_Handler_t* mui_handler) {

  // initialize the inner state of the LLCP receiver
  llcp_initialize(&mui_handler->llcp_receiver);
}

//}

// | -------------------------- power ------------------------- |

/* mui_pwr() //{ */

void mui_pwr(MUI_Handler_t* mui_handler, const bool state) {

  // create the message
  LLCP_PwrReqMsg_t msg;
  init_LLCP_PwrReqMsg_t(&msg);

  // fill in the payload
  msg.payload.state = state;

  // convert to network endian
  hton_LLCP_PwrReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), mui_handler->tx_buffer);

  mui_handler->fcns.sendString(mui_handler->tx_buffer, n_bytes);
}

//}

// | ----------------------- frame mode ----------------------- |

/* mui_measureFrame() //{ */

void mui_measureFrame(MUI_Handler_t* mui_handler, const uint16_t acquisition_time) {

  // create the message
  LLCP_MeasureFrameReqMsg_t msg;
  init_LLCP_MeasureFrameReqMsg_t(&msg);

  // fill in the payload
  msg.payload.acquisition_time_ms = acquisition_time;

  // convert to network endian
  hton_LLCP_MeasureFrameReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), mui_handler->tx_buffer);

  mui_handler->fcns.sendString(mui_handler->tx_buffer, n_bytes);
}

//}

// | ------------------------- masking ------------------------ |

/* mui_updatePixelMask //{ */

void mui_updatePixelMask(MUI_Handler_t* mui_handler, LLCP_UpdatePixelMaskReq_t* data) {

  // create the message
  LLCP_UpdatePixelMaskReqMsg_t msg;
  init_LLCP_UpdatePixelMaskReqMsg_t(&msg);

  msg.payload = *data;

  // convert to network endian
  hton_LLCP_UpdatePixelMaskReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), mui_handler->tx_buffer);

  mui_handler->fcns.sendString(mui_handler->tx_buffer, n_bytes);
}

//}

/* mui_setThreshold() //{ */

void mui_setThreshold(MUI_Handler_t* mui_handler, const uint16_t threshold_coarse, const uint16_t threshold_fine) {

  // create the message
  LLCP_SetThresholdReqMsg_t msg;
  init_LLCP_SetThresholdReqMsg_t(&msg);

  msg.payload.threshold_coarse = threshold_coarse;
  msg.payload.threshold_fine   = threshold_fine;

  // convert to network endian
  hton_LLCP_SetThresholdReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), mui_handler->tx_buffer);

  mui_handler->fcns.sendString(mui_handler->tx_buffer, n_bytes);
}

//}

/* mui_setConfigurationPreset() //{ */

void mui_setConfigurationPreset(MUI_Handler_t* mui_handler, const uint8_t preset) {

  // create the message
  LLCP_SetConfigurationPresetReqMsg_t msg;
  init_LLCP_SetConfigurationPresetReqMsg_t(&msg);

  msg.payload.preset = preset;

  // convert to network endian
  hton_LLCP_SetConfigurationPresetReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), mui_handler->tx_buffer);

  mui_handler->fcns.sendString(mui_handler->tx_buffer, n_bytes);
}

//}

// | ------------------------- status ------------------------- |

/* mui_getStatus() //{ */

void mui_getStatus(MUI_Handler_t* mui_handler) {

  // create the message
  LLCP_GetStatusReqMsg_t msg;
  init_LLCP_GetStatusReqMsg_t(&msg);

  // convert to network endian
  hton_LLCP_GetStatusReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), mui_handler->tx_buffer);

  mui_handler->fcns.sendString(mui_handler->tx_buffer, n_bytes);
}

//}

/* mui_getTemperature() //{ */

void mui_getTemperature(MUI_Handler_t* mui_handler) {

  // create the message
  LLCP_GetTemperatureReqMsg_t msg;
  init_LLCP_GetTemperatureReqMsg_t(&msg);

  // convert to network endian
  hton_LLCP_GetTemperatureReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), mui_handler->tx_buffer);

  mui_handler->fcns.sendString(mui_handler->tx_buffer, n_bytes);
}

//}

// | ------------- UART communication with MiniPIX ------------ |

/* mui_receiveCharCallback() //{ */

void mui_receiveCharCallback(MUI_Handler_t* mui_handler, const uint8_t byte_in) {

  LLCP_Message_t* message_in;

  bool checksum_matched;

  if (llcp_processChar(byte_in, &(mui_handler->llcp_receiver), &message_in, &checksum_matched)) {

    switch (message_in->id) {

      case LLCP_FRAME_DATA_MSG_ID: {

        // load up the message and convert it to our endian
        LLCP_FrameDataMsg_t* msg = (LLCP_FrameDataMsg_t*)message_in;
        ntoh_LLCP_FrameDataMsg_t(msg);

        msg->payload.checksum_matched = checksum_matched ? 1 : 0;

        // call the user's callback
        mui_handler->fcns.processFrameData(&(msg->payload));

#if MUI_USER_HANDSHAKES == 0
        mui_sendAck(mui_handler, true);
#endif

        break;
      };

      case LLCP_FRAME_MEASUREMENT_FINISHED_MSG_ID: {

#if MUI_USER_HANDSHAKES == 0
        mui_getFrameData(mui_handler);
#else
        // load up the message and convert it to our endian
        LLCP_FrameMeasurementFinishedMsg_t* msg = (LLCP_FrameMeasurementFinishedMsg_t*)message_in;
        ntoh_LLCP_FrameMeasurementFinishedMsg_t(msg);

        // call the user's callback
        mui_handler->fcns.processFrameMeasurementFinished();
#endif

        break;
      };

      case LLCP_FRAME_DATA_TERMINATOR_MSG_ID: {

        // load up the message and convert it to our endian
        LLCP_FrameDataTerminatorMsg_t* msg = (LLCP_FrameDataTerminatorMsg_t*)message_in;
        ntoh_LLCP_FrameDataTerminatorMsg_t(msg);

        // call the user's callback
        mui_handler->fcns.processFrameDataTerminator(&(msg->payload));

#if MUI_USER_HANDSHAKES == 0
        mui_sendAck(mui_handler, true);
#endif

        break;
      };

      case LLCP_STATUS_MSG_ID: {

        // load up the message and convert it to our endian
        LLCP_StatusMsg_t* msg = (LLCP_StatusMsg_t*)message_in;
        ntoh_LLCP_StatusMsg_t(msg);

        // call the user's callback
        mui_handler->fcns.processStatus(&(msg->payload));

#if MUI_USER_HANDSHAKES == 0
        mui_sendAck(mui_handler, true);
#endif

        break;
      };

      case LLCP_TEMPERATURE_MSG_ID: {

        // load up the message and convert it to our endian
        LLCP_TemperatureMsg_t* msg = (LLCP_TemperatureMsg_t*)message_in;
        ntoh_LLCP_TemperatureMsg_t(msg);

        // call the user's callback
        mui_handler->fcns.processTemperature(&(msg->payload));

#if MUI_USER_HANDSHAKES == 0
        mui_sendAck(mui_handler, true);
#endif

        break;
      };

      case LLCP_ACK_MSG_ID: {

        // load up the message and convert it to our endian
        LLCP_AckMsg_t* msg = (LLCP_AckMsg_t*)message_in;
        ntoh_LLCP_AckMsg_t(msg);

        // call the user's callback
        mui_handler->fcns.processAck(&(msg->payload));

        break;
      };

      case LLCP_MINIPIX_ERROR_MSG_ID: {

        // load up the message and convert it to our endian
        LLCP_MinipixErrorMsg_t* msg = (LLCP_MinipixErrorMsg_t*)message_in;
        ntoh_LLCP_MinipixErrorMsg_t(msg);

        // call the user's callbminipixerror
        mui_handler->fcns.processMinipixError(&(msg->payload));

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

// | --------------- the main update() function --------------- |

/* mui_update() //{ */

void mui_update(MUI_Handler_t* mui_handler) {

  mui_ledToggle(mui_handler);
  mui_handler->fcns.sleepHW((uint16_t)100);
}

//}

// | --------------------------- ack -------------------------- |

/* mui_sendAck() //{ */

void mui_sendAck(MUI_Handler_t* mui_handler, const bool success) {

  // create the message
  LLCP_AckMsg_t msg;
  init_LLCP_AckMsg_t(&msg);

  // fill in the payload
  msg.payload.success = success;

  // convert to network endian
  hton_LLCP_AckMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), mui_handler->tx_buffer);

  mui_handler->fcns.sendString(mui_handler->tx_buffer, n_bytes);

  /* mui_ledSet(mui_handler, false); */
}

//}

// --------------------------------------------------------------
// |                           private                          |
// --------------------------------------------------------------

/* mui_getFrameData() //{ */

void mui_getFrameData(MUI_Handler_t* mui_handler) {

  // create the message
  LLCP_GetFrameDataReqMsg_t msg;
  init_LLCP_GetFrameDataReqMsg_t(&msg);

  // convert to network endian
  hton_LLCP_GetFrameDataReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), mui_handler->tx_buffer);

  mui_handler->fcns.sendString(mui_handler->tx_buffer, n_bytes);
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
