#ifndef MINIPIX_INTERFACE_H
#define MINIPIX_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

// definition of packet structures of the measured data
#include <llcp_minipix_messages.h>
#include <llcp.h>

#include <stdint.h>
#include <stdbool.h>

typedef void (*mui_ledSetHW_t)(const bool);
typedef void (*mui_sendChar_t)(const uint8_t char_out);
typedef void (*mui_sendString_t)(const uint8_t *str_out, const uint16_t len);
typedef void (*mui_processImagePacket_t)(const ImageData_t *image_data);
typedef void (*mui_processStatus_t)(const Status_t *status_data);
typedef void (*mui_sleepHW_t)(const uint16_t duration);

typedef struct
{
  mui_ledSetHW_t           ledSetHW;
  mui_sendChar_t           sendChar;
  mui_sendString_t         sendString;
  mui_processImagePacket_t processImagePacket;
  mui_processStatus_t      processStatus;
  mui_sleepHW_t            sleepHW;
} MUI_FcnPrototypes_t;

typedef struct
{
  LLCP_Receiver_t     llcp_receiver;
  uint8_t             tx_buffer[LLCP_RX_TX_BUFFER_SIZE];
  bool                led_state;
  MUI_FcnPrototypes_t fcns;
} MUI_Handler_t;

// get status, to be called by the integrator
void getStatus(MUI_Handler_t *mui_handler);

//}

void mui_initialize(MUI_Handler_t *mui_handler);

void mui_measureFrame(MUI_Handler_t *mui_handler, const uint16_t acquisition_time);

void mui_getStatus(MUI_Handler_t *mui_handle);

// | ------------- UART communication with MiniPIX ------------ |

void mui_receiveCharCallback(MUI_Handler_t *mui_handler, const uint8_t byte_in);

// | -------------------- LED signalization ------------------- |

void mui_ledSet(MUI_Handler_t *mui_handler, const bool new_state);

void mui_ledToggle(MUI_Handler_t *mui_handler);

// | --------------- the main update() function --------------- |

void mui_update(MUI_Handler_t *mui_handler);

#ifdef __cplusplus
}
#endif

#endif  // MINIPIX_INTERFACE_H
