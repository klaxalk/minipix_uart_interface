#ifndef MINIPIX_INTERFACE_H
#define MINIPIX_INTERFACE_H

// definition of packet structures of the measured data
extern "C" {
#include <llcp_minipix_messages.h>
#include <llcp.h>
}

#include <stdint.h>
#include <stdbool.h>

/* extern void ledSet(const bool new_state); */
bool led_state_;

LLCP_Receiver_t llcp_receiver_;

uint8_t tx_buffer_[LLCP_RX_TX_BUFFER_SIZE];

// get status, to be called by the integrator
void getStatus(void);

// | --------------------- LED signalling --------------------- |

extern void minipix_interface_ledSetHW(const bool new_state);

// | -------------------------- USART ------------------------- |

extern void minipix_interface_minipixSendChar(const uint8_t char_out);
extern void minipix_interface_minipixSendString(const uint8_t* str_out, const uint16_t len);

// | ----------- virtual callbacks of the interface ----------- |

// will be called when image packet is ready to be processed by the integrator
extern void minipix_interface_processImagePacket(const ImageData_t* image_data);

// will be called when status data is ready to be processed by the integrator
extern void minipix_interface_processStatus(const Status_t* status);

// | --------------------- other routines --------------------- |

extern void minipix_interface_sleepHW(const uint16_t milliseconds);

//}

/* minipix_interface_initialize() //{ */

void minipix_interface_initialize(void) {

  // initialize the inner state of the LLCP receiver
  llcpInitialize(&llcp_receiver_);

  printf("[MinipixInterface]: initialized\n");
}

//}

/* minipix_interface_measureFrame() //{ */

void minipix_interface_measureFrame(const uint16_t acquisition_time) {

  MeasureFrameReqMsg_t get_frame;
  get_frame.message_id = LLCP_MEASURE_FRAME_MSG_ID;
  get_frame.payload.acquisition_time_ms = acquisition_time;
  hton_MeasureFrameReqMsg_t(&get_frame);

  printf("asking for acquisition %d\n", acquisition_time);

  uint16_t n_bytes = llcpPrepareMessage((uint8_t*)&get_frame, sizeof(get_frame), tx_buffer_);

  minipix_interface_minipixSendString(tx_buffer_, n_bytes);
}

//}

/* minipix_interface_getStatus() //{ */

void minipix_interface_getStatus(void) {

  GetStatusMsg_t get_status;
  get_status.message_id = LLCP_GET_STATUS_MSG_ID;
  hton_GetStatusMsg_t(&get_status);

  uint16_t n_bytes = llcpPrepareMessage((uint8_t*)&get_status, sizeof(get_status), tx_buffer_);

  minipix_interface_minipixSendString(tx_buffer_, n_bytes);
}

//}

// | ------------- UART communication with MiniPIX ------------ |

/* minipix_interface_ReceiveCharCallback() //{ */

inline void minipix_interface_ReceiveCharCallback(const uint8_t byte_in) {

  LLCPMessage_t message_in;

  if (llcpProcessChar(byte_in, &llcp_receiver_, &message_in)) {

    switch ((LLCPMessageId_t)message_in.id) {

      case LLCP_IMAGE_DATA_MSG_ID: {

        ImageDataMsg_t* msg = (ImageDataMsg_t*)&message_in.payload;
        ntoh_ImageDataMsg_t(msg);

        minipix_interface_processImagePacket(&msg->payload);

        break;
      };

      case LLCP_STATUS_MSG_ID: {

        StatusMsg_t* msg = (StatusMsg_t*)&message_in.payload;
        ntoh_StatusMsg_t(msg);

        minipix_interface_processStatus(&msg->payload);

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

/* ledSet() //{ */

inline void ledSet(const bool new_state) {

  minipix_interface_ledSetHW(new_state);

  led_state_ = new_state;
}

//}

/* ledToggle() //{ */

inline void ledToggle(void) {

  ledSet(!led_state_);
}

//}

// | --------------- the main update() function --------------- |

/* update() //{ */

void inline update(void) {

  // TODO process chars from the buffer

  ledToggle();
  minipix_interface_sleepHW(100);
}

//}

#endif  // MINIPIX_INTERFACE_H
