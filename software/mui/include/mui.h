#ifndef MUI_H
#define MUI_H

#ifdef __cplusplus
extern "C" {
#endif

// definition of packet structures of the measured data
#include <llcp_minipix_messages.h>
#include <llcp.h>

#include <stdint.h>
#include <stdbool.h>

// | ---------- function pointers to user's callbacks --------- |

/**
 * @brief Function pointer for implementation of seting an LED state.
 *
 * @param true = light is on, false = light is of
 */
typedef void (*mui_ledSetHW_t)(const bool);

/**
 * @brief Function pointer for implementaion of sending a char (byte) over
 * a serial line to MiniPIX.
 *
 * @param byte to send
 */
typedef void (*mui_sendChar_t)(const uint8_t char_out);

/**
 * @brief Function pointer for implementation of sending a string (array of
 * bytes) over a serial line to MiniPIX.
 *
 * @param str_out poiter to the first element of the byte array
 * @param len the number of bytes to send
 */
typedef void (*mui_sendString_t)(const uint8_t *str_out, const uint16_t len);

/**
 * @brief Function pointer to user implementation of callback to process
 * incoming Frame data.
 *
 * @param pointer to the structure with the data
 */
typedef void (*mui_processFrameData_t)(const LLCP_FrameData_t *image_data);

/**
 * @brief Function pointer to user implementation of callback to process
 * incoming Frame data terminator (the last packet).
 *
 * @param pointer to the structure with the data
 */
typedef void (*mui_processFrameDataTerminator_t)(const LLCP_FrameDataTerminator_t *terminator);

/**
 * @brief Function pointer to user implementation of callback to process
 * incoming Stream data.
 *
 * @param pointer to the structure with the data
 */
typedef void (*mui_processStreamData_t)(const LLCP_StreamData_t *image_data);

/**
 * @brief Function pointer to user implementation of callback to process
 * incoming Status message.
 *
 * @param pointer to the structure with the data
 */
typedef void (*mui_processStatus_t)(const LLCP_Status_t *status_data);

/**
 * @brief Function pointer to user implementation of callback to process
 * incoming Temperature message.
 *
 * @param pointer to the structure with the data
 */
typedef void (*mui_processTemperature_t)(const LLCP_Temperature_t *temperature_data);

/**
 * @brief Function pointer to user implementation of callback to process
 * incoming Ack message.
 *
 * @param pointer to the structure with the data
 */
typedef void (*mui_processAck_t)(const LLCP_Ack_t *ack_data);

/**
 * @brief Function pointer to user implementation of callback to process
 * incoming MiniPIX error message.
 *
 * @param pointer to the structure with the data
 */
typedef void (*mui_processMinipixError_t)(const LLCP_MinipixError_t *error_data);

/**
 * @brief Function pointer for implementation of a sleep function.
 *
 * @param duration time to sleep in milliseconds
 */
typedef void (*mui_sleepHW_t)(const uint16_t duration);

/**
 * @brief Structure containing pointers to user callbacks.
 * User shouls provide implementations to all the listed
 * functions.
 */
typedef struct
{
  mui_ledSetHW_t                   ledSetHW;
  mui_sendChar_t                   sendChar;
  mui_sendString_t                 sendString;
  mui_processFrameData_t           processFrameData;
  mui_processFrameDataTerminator_t processFrameDataTerminator;
  mui_processStreamData_t          processStreamData;
  mui_processStatus_t              processStatus;
  mui_processTemperature_t         processTemperature;
  mui_processAck_t                 processAck;
  mui_processMinipixError_t        processMinipixError;
  mui_sleepHW_t                    sleepHW;
} MUI_FcnPrototypes_t;

typedef struct
{
  LLCP_Receiver_t     llcp_receiver;
  uint8_t             tx_buffer[LLCP_RX_TX_BUFFER_SIZE];
  bool                led_state;
  MUI_FcnPrototypes_t fcns;
} MUI_Handler_t;

// --------------------------------------------------------------
// |                           public                           |
// --------------------------------------------------------------

// | ---------------------- "constructor" --------------------- |

/**
 * @brief The "constructor" of the MUI handler. Should be called
 * by the user just after allocating it.
 *
 * @param mui_handler
 */
void mui_initialize(MUI_Handler_t *mui_handler);

// | -------------- interraction with the MiniPIX ------------- |

/**
 * @brief Command to power on/off the Timepix3 chip.
 *
 * @param mui_handler
 * @param state true/false, power on/off
 */
void mui_pwr(MUI_Handler_t *mui_handler, const bool state);

/**
 * @brief Command to acquire a frame over a period of the acquisition_time.
 * As a result, the MiniPIX will start obtaning data and will call the
 *                        processFrameData()
 * function will be called.
 *
 * @param mui_handler
 * @param acquisition_time
 */
void mui_measureFrame(MUI_Handler_t *mui_handler, const uint16_t acquisition_time);

/**
 * @brief Command to start measurement in the form of the event stream.
 * As a result, packets with event data will be comming until stopped
 * with the mui_stopStream() function. The incoming data will cause the
 *                        processStreamData()
 * to be called.
 *
 * @param mui_handler
 * @param duty_cycle how many milliseconds in a second should the stream be outputed.
 */
void mui_measureStream(MUI_Handler_t *mui_handler, const uint16_t duty_cycle);

/**
 * @brief Command to set energy threshold
 *
 * @param mui_handler
 * @param threshold
 */
void mui_setThreshold(MUI_Handler_t *mui_handler, const uint16_t threshold_coarse, const uint16_t threshold_fine);

/**
 * @brief Command to set configuration preset
 *
 * @param mui_handler
 * @param preset
 */
void mui_setConfigurationPreset(MUI_Handler_t *mui_handler, const uint8_t preset);

/**
 * @brief Command to flush the stream buffer in the MiniPIX.
 *
 * @param mui_handler
 */
void mui_flushBuffer(MUI_Handler_t *mui_handler);

/**
 * @brief Command to update a mask of a particular pixel.
 *
 * @param mui_handler
 * @param LLCP_UpdatePixelMaskReq_t
 */
void mui_updatePixelMask(MUI_Handler_t *mui_handler, LLCP_UpdatePixelMaskReq_t *data);

/**
 * @brief Command to get the status of the MinixPIX HW. As a result,
 * the method
 *                          processStatus()
 * will get called.
 *
 * @param mui_handle
 */
void mui_getStatus(MUI_Handler_t *mui_handle);

/**
 * @brief Command to get the temperature of the MinixPIX HW. As a result,
 * the method
 *                          processTemperature()
 * will get called.
 *
 * @param mui_handle
 */
void mui_getTemperature(MUI_Handler_t *mui_handle);

// | ------------- UART communication with MiniPIX ------------ |

/**
 * @brief Called by the user with every incoming byte on the UART to the MiniPIX.
 *
 * @param mui_handler
 * @param byte_in
 */
void mui_receiveCharCallback(MUI_Handler_t *mui_handler, const uint8_t byte_in);

// | --------------- the main update() function --------------- |

/**
 * @brief Called by the user to led MUI do stuff in the background.
 * Should be called regularly, e.g., in the main() loop or in a
 * timer's callback.
 *
 * @param mui_handler
 */
void mui_update(MUI_Handler_t *mui_handler);

/**
 * @brief Sends ack to the MiniPIX. Not to be called by the user.
 *
 * @param mui_handler
 * @param success
 */
void mui_sendAck(MUI_Handler_t *mui_handler, const bool success);

// --------------------------------------------------------------
// |                           private                          |
// --------------------------------------------------------------

/**
 * @brief Command to request measured frame data.
 * As a result, the MiniPIX will start obtaning data and will call the
 *                        processFrameData()
 * function will be called.
 *
 * @param mui_handler
 */
void mui_getFrameData(MUI_Handler_t *mui_handler);

// | -------------------- LED signalization ------------------- |

/**
 * @brief set LED state
 *
 * @param mui_handler
 * @param new_state true = light is on, false = light is off
 */
void mui_ledSet(MUI_Handler_t *mui_handler, const bool new_state);

/**
 * @brief toggle the state of the LED
 *
 * @param mui_handler
 */
void mui_ledToggle(MUI_Handler_t *mui_handler);

#ifdef __cplusplus
}
#endif

#endif  // MUI_H
