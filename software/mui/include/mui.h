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

// gives user more control over protocol handshakes, namely:
// * user needs to call mui_getFrameData() after he receives the mui_processFrameMeasurementFinished_t() callback. Otherwise, this callback is not called and
// the handshake happens automatically.
// * user needs to call mui_sendAck() after receiving FrameData, FrameDataTerminator, Temperature, ChipVoltage and Status callbacks.
#ifndef MUI_USER_HANDSHAKES
#define MUI_USER_HANDSHAKES 0
#endif

// user can pass the mui_sendString_t to facilitate sending of data
#ifndef MUI_SEND_STRING
#define MUI_SEND_STRING 0
#endif

// user can pass the mui_sendChar_t to facilitate sending of data
#ifndef MUI_SEND_CHAR
#define MUI_SEND_CHAR 0
#endif

/* automatically-defined configs //{ */

#if ((MUI_SEND_CHAR == 1) && (MUI_SEND_STRING == 1))
#warning "MUI is compiled with both MUI_SEND_CHAR = 1 and MUI_SEND_STRING = 1, will expect pointer to mui_sendString_t during runtime"
#elif ((MUI_SEND_CHAR == 0) && (MUI_SEND_STRING == 0))
#error "Cannot have both MUI_SEND_CHAR and MUI_SEND_STRING set to 0. At least one data handling function needs to be used! Please, read README at https://github.com/klaxalk/minipix_uart_interface/tree/master/software/mui for more details."
#elif ((MUI_SEND_CHAR == 1) && (MUI_SEND_STRING == 0))
#pragma message("MUI is being compiled with MUI_SEND_CHAR = 1, it will expect pointer to the mui_sendChar_t function during runtime")
#elif ((MUI_SEND_CHAR == 0) && (MUI_SEND_STRING == 1))
#pragma message("MUI is being compiled with MUI_SEND_STRING = 1, it will expect pointer to the mui_sendString_t function during runtime")
#endif

#if MUI_USER_HANDSHAKES == 1
#warning "MUI_USER_HANDSHAKES is 1, do not forget to call mui_sendAck() according to the README"
#endif

//}

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
 * message regarding that a measurement was finished.
 */
typedef void (*mui_processFrameMeasurementFinished_t)(void);

/**
 * @brief Function pointer to user implementation of callback to process
 * incoming Frame data terminator (the last packet).
 *
 * @param pointer to the structure with the data
 */
typedef void (*mui_processFrameDataTerminator_t)(const LLCP_FrameDataTerminator_t *terminator);

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
 * incoming ChipVoltage message.
 *
 * @param pointer to the structure with the data
 */
typedef void (*mui_processChipVoltage_t)(const LLCP_ChipVoltage_t *chip_voltage_data);

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
  mui_ledSetHW_t                        ledSetHW;
  mui_sendChar_t                        sendChar;
  mui_sendString_t                      sendString;
  mui_processFrameData_t                processFrameData;
  mui_processFrameDataTerminator_t      processFrameDataTerminator;
  mui_processStatus_t                   processStatus;
  mui_processTemperature_t              processTemperature;
  mui_processChipVoltage_t              processChipVoltage;
  mui_processAck_t                      processAck;
  mui_processMinipixError_t             processMinipixError;
  mui_processFrameMeasurementFinished_t processFrameMeasurementFinished;
  mui_sleepHW_t                         sleepHW;
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
 * @param acquisition_time Acquisition time in milliseconds
 * @param mode pixel measurement mode: LLCP_TPX3_PXL_MODE_{TOA_TOT,TOA,MPX_ITOT}
 */
void mui_measureFrame(MUI_Handler_t *mui_handler, const uint16_t acquisition_time, const uint8_t mode);

/**
 * @brief Command to request measured frame data.
 * As a result, the MiniPIX will start obtaning data and will call the
 *                        processFrameData()
 * function will be called.
 *
 * @param mui_handler
 */
void mui_getFrameData(MUI_Handler_t *mui_handler);

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

/**
 * @brief Command to get the chip voltage of the MinixPIX HW. As a result,
 * the method
 *                          processChipVoltage()
 * will get called.
 *
 * @param mui_handle
 */
void mui_getChipVoltage(MUI_Handler_t *mui_handle);

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
 * @brief Abstraction above sending a message. Will use either sendString or sendChar basend on configuration.
 *
 * @param mui_handler
 * @param message the string to send
 * @param len how many bytes to send
 */
void mui_sendMessage(MUI_Handler_t* mui_handler, const uint8_t* message, const uint16_t len);

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
