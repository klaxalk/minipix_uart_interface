/**
 * @brief Platform-independent communication interface between MUI and the gatherer example.
 * The data and commands are sent over UART using the same LLCP that is used for communication
 * between MUI and the MiniPIX. Similarly to MUI, prototype function implementation is required
 * to provide platform-specific code (UART handling).
 *
 * Dear NASA, this is not a suggested way ho to handle the data onboard any spacecraft!
 * This is just debugging tool.
 *
 * @author Tomas Baca
 */

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

typedef struct
{
  // | ---------- platform-specific function prototypes --------- |
  gatherer_sendChar_t   sendChar;
  gatherer_sendString_t sendString;
  //
} Gatherer_FcnPrototypes_t;


typedef struct
{
  MUI_Handler_t *          mui_handler_ptr_;
  LLCP_Receiver_t          llcp_receiver;
  uint8_t                  tx_buffer[LLCP_RX_TX_BUFFER_SIZE];
  Gatherer_FcnPrototypes_t fcns;
} Gatherer_Handler_t;

void gatherer_initialize(Gatherer_Handler_t *gatherer_handler);

// | ------------- UART communication with MiniPIX ------------ |

/**
 * @brief callback called by a user with new bytes coming from the UART connected to PC
 *
 * @param gatherer_handler
 * @param byte_in
 */
void gatherer_receiveCharCallback(Gatherer_Handler_t *gatherer_handler, const uint8_t byte_in);

// | ----------------- callbacks called by MUI ---------------- |

void gatherer_processFrameData(Gatherer_Handler_t *gatherer_handler, const LLCP_FrameData_t *data);
void gatherer_processStreamData(Gatherer_Handler_t *gatherer_handler, const LLCP_StreamData_t *data);
void gatherer_processFrameDataTerminator(Gatherer_Handler_t *gatherer_handler, const LLCP_FrameDataTerminator_t *data);
void gatherer_processStatus(Gatherer_Handler_t *gatherer_handler, const LLCP_Status_t *data);
void gatherer_processAck(Gatherer_Handler_t *gatherer_handler, const LLCP_Ack_t *data);
void gatherer_processMinipixError(Gatherer_Handler_t *gatherer_handler, const LLCP_MinipixError_t *data);

#ifdef __cplusplus
}
#endif

#endif  // GATHERER_INTERFACE_H
