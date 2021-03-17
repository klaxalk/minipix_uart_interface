#ifndef GATHERER_INTERFACE_H
#define GATHERER_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <mui.h>

#include <stdint.h>
#include <stdbool.h>

// hw support
typedef void (*gatherer_sendChar_t)(const uint8_t char_out);
typedef void (*gatherer_sendString_t)(const uint8_t *str_out, const uint16_t len);
// gatherer->minipix
typedef void (*gatherer_getStatus_t)(MUI_Handler_t *mui_handler);
typedef void (*gatherer_measureFrame_t)(MUI_Handler_t *mui_handler, const uint16_t acquisition_time);

typedef struct
{
  // hw support
  gatherer_sendChar_t   sendChar;
  gatherer_sendString_t sendString;
  // gatherer->minipix
  gatherer_getStatus_t    getStatus;
  gatherer_measureFrame_t measureFrame;
  //
} Gatherer_FcnPrototypes_t;


typedef struct
{
  MUI_Handler_t *          mui_handler_ptr_;
  LLCP_Receiver_t          llcp_receiver;
  uint8_t                  tx_buffer[LLCP_RX_TX_BUFFER_SIZE];
  Gatherer_FcnPrototypes_t fcns;
} Gatherer_Handler_t;

//}

void gatherer_initialize(Gatherer_Handler_t *gatherer_handler);

// | ------------- UART communication with MiniPIX ------------ |

void gatherer_receiveCharCallback(Gatherer_Handler_t *gatherer_handler, const uint8_t byte_in);

// | ----------------- callbacks called by MUI ---------------- |

void gatherer_processImagePacket(Gatherer_Handler_t *gatherer_handler, const ImageData_t *image_data);
void gatherer_processStatus(Gatherer_Handler_t *gatherer_handler, const Status_t *status);

#ifdef __cplusplus
}
#endif

#endif  // GATHERER_INTERFACE_H
