#ifndef LLCP_MINIPIX_MESSAGES_H
#define LLCP_MINIPIX_MESSAGES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>  // for assert checks of struct sizes
#include <string.h>  // for memset
#include <llcp_endian.h>

/* definition of message IDs //{ */

// I don't want to have 0 or 1, due to debugging
// also, 0 or 1 is too common to get by accident
//
// the ids are grouped by tens according to
// common usage

// status
#define LLCP_STATUS_MSG_ID 10
#define LLCP_GET_STATUS_REQ_MSG_ID 11
#define LLCP_TEMPERATURE_MSG_ID 12
#define LLCP_GET_TEMPERATURE_REQ_MSG_ID 13

// frame-based measurement
#define LLCP_FRAME_DATA_MSG_ID 20
#define LLCP_MEASURE_FRAME_REQ_MSG_ID 21
#define LLCP_FRAME_DATA_TERMINATOR_MSG_ID 22
#define LLCP_GET_FRAME_DATA_REQ_MSG_ID 23
#define LLCP_FRAME_MEASUREMENT_FINISHED_MSG_ID 24

// ack
#define LLCP_ACK_MSG_ID 40

// masking
#define LLCP_UPDATE_PIXEL_MASK_REQ_MSG_ID 50

// power
#define LLCP_PWR_REQ_MSG_ID 60

// threshold
#define LLCP_SET_THRESHOLD_REQ_MSG_ID 70

// configuration presets
#define LLCP_SET_CONFIGURATION_PRESET_REQ_MSG_ID 80

// error
#define LLCP_MINIPIX_ERROR_MSG_ID 90

//}

/* definition of error IDs //{ */

#define LLCP_MINIPIX_ERROR_COUNT 10
extern const char* LLCP_MinipixErrors[LLCP_MINIPIX_ERROR_COUNT];

#define LLCP_MINIPIX_ERROR_MEASUREMENT_FAILED 0
#define LLCP_MINIPIX_ERROR_POWERUP_FAILED 1
#define LLCP_MINIPIX_ERROR_POWERUP_TPX3_RESET_SYNC 2
#define LLCP_MINIPIX_ERROR_POWERUP_TPX3_RESET_RECVDATA 3
#define LLCP_MINIPIX_ERROR_POWERUP_TPX3_INIT_RESETS 4
#define LLCP_MINIPIX_ERROR_POWERUP_TPX3_INIT_CHIPID 5
#define LLCP_MINIPIX_ERROR_POWERUP_TPX3_INIT_DACS 6
#define LLCP_MINIPIX_ERROR_POWERUP_TPX3_INIT_PIXCFG 7
#define LLCP_MINIPIX_ERROR_POWERUP_TPX3_INIT_MATRIX 8
#define LLCP_MINIPIX_ERROR_INVALID_PRESET 9

//}

// | ----------------------- Pixel data ----------------------- |

/* LLCP_PixelData_t //{ */

typedef struct __attribute__((packed))
{
  uint8_t  value1 : 4;
  uint16_t value2 : 10;
  uint16_t value3 : 14;
  uint16_t address : 16;
  uint16_t header : 4;
} LLCP_PixelDataCommon_t;

/**
 * @brief Structure for interpreting the ToA and ToT mode
 * can be used only after derandomization of the data
 */
typedef struct __attribute__((packed))
{
  uint8_t  ftoa : 4;
  uint16_t tot : 10;
  uint16_t toa : 14;
  uint16_t address : 16;
  uint16_t header : 4;
} LLCP_PixelDataToAToT_t;

/**
 * @brief Structure for interpreting the ToA mode
 * can be used only after derandomization of the data
 */
typedef struct __attribute__((packed))
{
  uint8_t  ftoa : 4;
  uint16_t dummy : 10;
  uint16_t toa : 14;
  uint16_t address : 16;
  uint16_t header : 4;
} LLCP_PixelDataToA_t;

/**
 * @brief Structure for interpreting the Mpx and iToT mode
 * can be used only after derandomization of the data
 */
typedef struct __attribute__((packed))
{
  uint8_t  dummy : 4;
  uint16_t event_counter : 10;
  uint16_t itot : 14;
  uint16_t address : 16;
  uint16_t header : 4;
} LLCP_PixelDataMpxiToT_t;

/**
 * @brief Structure for holding pixel coordinates and measured pixel values.
 */
typedef struct __attribute__((packed))
{
  // * the data in this form are in a form that can not be easily interpreted
  // * the data needs to put through functions that decode the real values (not done by mui atm.)
  uint8_t data[6];
} LLCP_PixelData_t;

/**
 * @brief host-to-network conversion for LLCP_PixelData_t
 *
 * @param data
 */
void hton_LLCP_PixelData_t(LLCP_PixelData_t* data);

/**
 * @brief network-to-host conversion for LLCP_PixelData_t
 *
 * @param data
 */
void ntoh_LLCP_PixelData_t(LLCP_PixelData_t* data);

/**
 * @brief "constructor" for LLCP_PixelData_t
 *
 * @param data
 */
void init_LLCP_PixelData_t(LLCP_PixelData_t* data);

//}

// | ------------- Frame data (integrated images) ------------- |

/* LLCP_FrameDataMsg_t //{ */

/* struct LLCP_FrameData_t //{ */

#define LLCP_FRAME_DATA_N_PIXELS 41

#define LLCP_TPX3_PXL_MODE_UNSET 0
#define LLCP_TPX3_PXL_MODE_TOA_TOT 10
#define LLCP_TPX3_PXL_MODE_TOA 3       // TODO
#define LLCP_TPX3_PXL_MODE_MPX_ITOT 4  // TODO

/**
 * @brief Message data for LLCP_FrameDataMsg_t
 */
typedef struct __attribute__((packed))
{
  uint16_t         frame_id;          // a unique identifier of the frame, can be used to stitch the packets together
  uint16_t         packet_id;         // should be incremented for each packet of the frame
  uint8_t          mode;              // will determine the measurement mode
  uint8_t          n_pixels;          // how many pixels are filled in
  uint8_t          checksum_matched;  // 1 if the messages was received by MUI with correct checksum
  LLCP_PixelData_t pixel_data[LLCP_FRAME_DATA_N_PIXELS];
} LLCP_FrameData_t;

/**
 * @brief host-to-network conversion for LLCP_FrameData_t
 *
 * @param data
 */
void hton_LLCP_FrameData_t(LLCP_FrameData_t* data);

/**
 * @brief network-to-host conversion for LLCP_FrameData_t
 *
 * @param data
 */
void ntoh_LLCP_FrameData_t(LLCP_FrameData_t* data);

/**
 * @brief "constructor" for LLCP_FrameData_t
 *
 * @param data
 */
void init_LLCP_FrameData_t(LLCP_FrameData_t* data);

//}

/**
 * @brief LLCP Message for sending image data.
 */
typedef struct __attribute__((packed))
{
  uint8_t          message_id;
  LLCP_FrameData_t payload;
} LLCP_FrameDataMsg_t;

/**
 * @brief host-to-network conversion for LLCP_FrameDataMsg_t
 *
 * @param data
 */
void hton_LLCP_FrameDataMsg_t(LLCP_FrameDataMsg_t* msg);

/**
 * @brief network-to-host conversion for LLCP_FrameDataMsg_t
 *
 * @param data
 */
void ntoh_LLCP_FrameDataMsg_t(LLCP_FrameDataMsg_t* msg);

/**
 * @brief "constructor" for LLCP_FrameDataMsg_t
 *
 * @param data
 */
void init_LLCP_FrameDataMsg_t(LLCP_FrameDataMsg_t* msg);

static_assert((sizeof(LLCP_FrameDataMsg_t) > 255) == 0, "LLCP_FrameDataMsg_t is too large");

//}

/* LLCP_FrameDataTerminatorMsg_t //{ */

/* struct LLCP_FrameDataTerminator_t //{ */

/**
 * @brief Message data for LLCP_FrameDataTerminatorMsg_t
 */
typedef struct __attribute__((packed))
{
  uint16_t frame_id;   // a unique identifier of the frame, can be used to stitch the packets together
  uint16_t n_packets;  // how many packets were sent
} LLCP_FrameDataTerminator_t;

/**
 * @brief host-to-network conversion for LLCP_FrameDataTerminator_t
 *
 * @param data
 */
void hton_LLCP_FrameDataTerminator_t(LLCP_FrameDataTerminator_t* data);

/**
 * @brief network-to-host conversion for LLCP_FrameDataTerminator_t
 *
 * @param data
 */
void ntoh_LLCP_FrameDataTerminator_t(LLCP_FrameDataTerminator_t* data);

/**
 * @brief "constructor" for LLCP_FrameDataTerminator_t
 *
 * @param data
 */
void init_LLCP_FrameDataTerminator_t(LLCP_FrameDataTerminator_t* data);

//}

/**
 * @brief LLCP Message for sending image data.
 */
typedef struct __attribute__((packed))
{
  uint8_t                    message_id;
  LLCP_FrameDataTerminator_t payload;
} LLCP_FrameDataTerminatorMsg_t;

/**
 * @brief host-to-network conversion for LLCP_FrameDataTerminatorMsg_t
 *
 * @param data
 */
void hton_LLCP_FrameDataTerminatorMsg_t(LLCP_FrameDataTerminatorMsg_t* msg);

/**
 * @brief network-to-host conversion for LLCP_FrameDataTerminatorMsg_t
 *
 * @param data
 */
void ntoh_LLCP_FrameDataTerminatorMsg_t(LLCP_FrameDataTerminatorMsg_t* msg);

/**
 * @brief "constructor" for LLCP_FrameDataTerminatorMsg_t
 *
 * @param data
 */
void init_LLCP_FrameDataTerminatorMsg_t(LLCP_FrameDataTerminatorMsg_t* msg);

static_assert((sizeof(LLCP_FrameDataTerminatorMsg_t) > 255) == 0, "LLCP_FrameDataTerminatorMsg_t is too large");

//}

/* LLCP_MeasureFrameReqMsg_t //{ */

/* LLCP_MeasureFrameReq_t //{ */

/**
 * @brief Message data for LLCP_MeasureFrameReqMsg_t
 */
typedef struct __attribute__((packed))
{
  uint16_t acquisition_time_ms;
  uint8_t  mode;
} LLCP_MeasureFrameReq_t;

/**
 * @brief host-to-network conversion for LLCP_MeasureFrameReq_t
 *
 * @param data
 */
void hton_LLCP_MeasureFrameReq_t(LLCP_MeasureFrameReq_t* data);

/**
 * @brief network-to-host conversion for LLCP_MeasureFrameReq_t
 *
 * @param data
 */
void ntoh_LLCP_MeasureFrameReq_t(LLCP_MeasureFrameReq_t* data);

/**
 * @brief "constructor" for LLCP_MeasureFrameReq_t
 *
 * @param data
 */
void init_LLCP_MeasureFrameReq_t(LLCP_MeasureFrameReq_t* data);

//}

/**
 * @brief LLCP Message for requesting a frame measurement.
 */
typedef struct __attribute__((packed))
{
  uint8_t                message_id;
  LLCP_MeasureFrameReq_t payload;
} LLCP_MeasureFrameReqMsg_t;

/**
 * @brief host-to-network conversion for LLCP_MeasureFrameReqMsg_t
 *
 * @param msg
 */
void hton_LLCP_MeasureFrameReqMsg_t(LLCP_MeasureFrameReqMsg_t* msg);

/**
 * @brief network-to-host conversion for LLCP_MeasureFrameReqMsg_t
 *
 * @param msg
 */
void ntoh_LLCP_MeasureFrameReqMsg_t(LLCP_MeasureFrameReqMsg_t* msg);

/**
 * @brief "constructor" for LLCP_MeasureFrameReqMsg_t
 *
 * @param msg
 */
void init_LLCP_MeasureFrameReqMsg_t(LLCP_MeasureFrameReqMsg_t* msg);

static_assert((sizeof(LLCP_MeasureFrameReqMsg_t) > 255) == 0, "LLCP_MeasureFrameReqMsg_t is too large");

//}

/* LLCP_FrameMeasurementFinishedMsg_t //{ */

/**
 * @brief LLCP Message for getting noticed that MiniPIX measurement had just finished
 */
typedef struct __attribute__((packed))
{
  uint8_t message_id;
} LLCP_FrameMeasurementFinishedMsg_t;

/**
 * @brief host-to-network conversion for LLCP_FrameMeasurementFinishedMsg_t
 *
 * @param msg
 */
void hton_LLCP_FrameMeasurementFinishedMsg_t(LLCP_FrameMeasurementFinishedMsg_t* msg);

/**
 * @brief network-to-host conversion for LLCP_FrameMeasurementFinishedMsg_t
 *
 * @param msg
 */
void ntoh_LLCP_FrameMeasurementFinishedMsg_t(LLCP_FrameMeasurementFinishedMsg_t* msg);

/**
 * @brief "constructor" for LLCP_FrameMeasurementFinishedMsg_t
 *
 * @param msg
 */
void init_LLCP_FrameMeasurementFinishedMsg_t(LLCP_FrameMeasurementFinishedMsg_t* msg);

static_assert((sizeof(LLCP_FrameMeasurementFinishedMsg_t) > 255) == 0, "LLCP_FrameMeasurementFinishedMsg_t is too large");

//}

/* LLCP_GetFrameDataReqMsg_t //{ */

/**
 * @brief LLCP Message for requesting data from a previously-measured frame
 */
typedef struct __attribute__((packed))
{
  uint8_t message_id;
} LLCP_GetFrameDataReqMsg_t;

/**
 * @brief host-to-network conversion for LLCP_GetFrameDataReqMsg_t
 *
 * @param msg
 */
void hton_LLCP_GetFrameDataReqMsg_t(LLCP_GetFrameDataReqMsg_t* msg);

/**
 * @brief network-to-host conversion for LLCP_GetFrameDataReqMsg_t
 *
 * @param msg
 */
void ntoh_LLCP_GetFrameDataReqMsg_t(LLCP_GetFrameDataReqMsg_t* msg);

/**
 * @brief "constructor" for LLCP_GetFrameDataReqMsg_t
 *
 * @param msg
 */
void init_LLCP_GetFrameDataReqMsg_t(LLCP_GetFrameDataReqMsg_t* msg);

static_assert((sizeof(LLCP_GetFrameDataReqMsg_t) > 255) == 0, "LLCP_GetFrameDataReqMsg_t is too large");

//}

// | ---------------------- Pixel masking --------------------- |

/* LLCP_UpdatePixelMaskReqMsg_t //{ */

/* struct LLCP_UpdatePixelMaskReq_t //{ */

/**
 * @brief Message data for LLCP_UpdatePixelMaskReqMsg_t.
 */
typedef struct __attribute__((packed))
{
  uint8_t x_coordinate;
  uint8_t y_coordinate;
  uint8_t masked;
} LLCP_UpdatePixelMaskReq_t;

/**
 * @brief host-to-network conversion for LLCP_UpdatePixelMaskReq_t
 *
 * @param data
 */
void hton_LLCP_UpdatePixelMaskReq_t(LLCP_UpdatePixelMaskReq_t* data);

/**
 * @brief network-to-host conversion for LLCP_UpdatePixelMaskReq_t
 *
 * @param data
 */
void ntoh_LLCP_UpdatePixelMaskReq_t(LLCP_UpdatePixelMaskReq_t* data);

/**
 * @brief "constructor" for LLCP_UpdatePixelMaskReq_t
 *
 * @param data
 */
void init_LLCP_UpdatePixelMaskReq_t(LLCP_UpdatePixelMaskReq_t* data);

//}

/**
 * @brief LLCP Message for updating a pixel mask.
 */
typedef struct __attribute__((packed))
{
  uint8_t                   message_id;
  LLCP_UpdatePixelMaskReq_t payload;
} LLCP_UpdatePixelMaskReqMsg_t;

/**
 * @brief host-to-network conversion for LLCP_UpdatePixelMaskReqMsg_t
 *
 * @param msg
 */
void hton_LLCP_UpdatePixelMaskReqMsg_t(LLCP_UpdatePixelMaskReqMsg_t* msg);

/**
 * @brief network-to-host conversion for LLCP_UpdatePixelMaskReqMsg_t
 *
 * @param msg
 */
void ntoh_LLCP_UpdatePixelMaskReqMsg_t(LLCP_UpdatePixelMaskReqMsg_t* msg);

/**
 * @brief "constructor" for LLCP_UpdatePixelMaskReqMsg_t
 *
 * @param msg
 */
void init_LLCP_UpdatePixelMaskReqMsg_t(LLCP_UpdatePixelMaskReqMsg_t* msg);

static_assert((sizeof(LLCP_UpdatePixelMaskReqMsg_t) > 255) == 0, "LLCP_UpdatePixelMaskReqMsg_t is too large");

//}

// | ------------------------ Threshold ----------------------- |

/* LLCP_SetThresholdReqMsg_t //{ */

/* struct LLCP_SetThresholdReq_t //{ */

/**
 * @brief Message data for LLCP_SetThresholdReqMsg_t.
 */
typedef struct __attribute__((packed))
{
  uint16_t threshold_coarse;
  uint16_t threshold_fine;
} LLCP_SetThresholdReq_t;

/**
 * @brief host-to-network conversion for LLCP_SetThresholdReq_t
 *
 * @param data
 */
void hton_LLCP_SetThresholdReq_t(LLCP_SetThresholdReq_t* data);

/**
 * @brief network-to-host conversion for LLCP_SetThresholdReq_t
 *
 * @param data
 */
void ntoh_LLCP_SetThresholdReq_t(LLCP_SetThresholdReq_t* data);

/**
 * @brief "constructor" for LLCP_SetThresholdReq_t
 *
 * @param data
 */
void init_LLCP_SetThresholdReq_t(LLCP_SetThresholdReq_t* data);

//}

/**
 * @brief LLCP Message for updating a pixel mask.
 */
typedef struct __attribute__((packed))
{
  uint8_t                message_id;
  LLCP_SetThresholdReq_t payload;
} LLCP_SetThresholdReqMsg_t;

/**
 * @brief host-to-network conversion for LLCP_SetThresholdReqMsg_t
 *
 * @param msg
 */
void hton_LLCP_SetThresholdReqMsg_t(LLCP_SetThresholdReqMsg_t* msg);

/**
 * @brief network-to-host conversion for LLCP_SetThresholdReqMsg_t
 *
 * @param msg
 */
void ntoh_LLCP_SetThresholdReqMsg_t(LLCP_SetThresholdReqMsg_t* msg);

/**
 * @brief "constructor" for LLCP_SetThresholdReqMsg_t
 *
 * @param msg
 */
void init_LLCP_SetThresholdReqMsg_t(LLCP_SetThresholdReqMsg_t* msg);

static_assert((sizeof(LLCP_SetThresholdReqMsg_t) > 255) == 0, "LLCP_SetThresholdReqMsg_t is too large");

//}

// | ---------------------- Configuration --------------------- |

/* LLCP_SetConfigurationPresetReqMsg_t //{ */

/* struct LLCP_SetConfigurationPresetReq_t //{ */

/**
 * @brief Message data for LLCP_SetConfigurationPresetReqMsg_t.
 */
typedef struct __attribute__((packed))
{
  uint8_t preset;
} LLCP_SetConfigurationPresetReq_t;

/**
 * @brief host-to-network conversion for LLCP_SetConfigurationPresetReq_t
 *
 * @param data
 */
void hton_LLCP_SetConfigurationPresetReq_t(LLCP_SetConfigurationPresetReq_t* data);

/**
 * @brief network-to-host conversion for LLCP_SetConfigurationPresetReq_t
 *
 * @param data
 */
void ntoh_LLCP_SetConfigurationPresetReq_t(LLCP_SetConfigurationPresetReq_t* data);

/**
 * @brief "constructor" for LLCP_SetConfigurationPresetReq_t
 *
 * @param data
 */
void init_LLCP_SetConfigurationPresetReq_t(LLCP_SetConfigurationPresetReq_t* data);

//}

/**
 * @brief LLCP Message for updating a pixel mask.
 */
typedef struct __attribute__((packed))
{
  uint8_t                          message_id;
  LLCP_SetConfigurationPresetReq_t payload;
} LLCP_SetConfigurationPresetReqMsg_t;

/**
 * @brief host-to-network conversion for LLCP_SetConfigurationPresetReqMsg_t
 *
 * @param msg
 */
void hton_LLCP_SetConfigurationPresetReqMsg_t(LLCP_SetConfigurationPresetReqMsg_t* msg);

/**
 * @brief network-to-host conversion for LLCP_SetConfigurationPresetReqMsg_t
 *
 * @param msg
 */
void ntoh_LLCP_SetConfigurationPresetReqMsg_t(LLCP_SetConfigurationPresetReqMsg_t* msg);

/**
 * @brief "constructor" for LLCP_SetConfigurationPresetReqMsg_t
 *
 * @param msg
 */
void init_LLCP_SetConfigurationPresetReqMsg_t(LLCP_SetConfigurationPresetReqMsg_t* msg);

static_assert((sizeof(LLCP_SetConfigurationPresetReqMsg_t) > 255) == 0, "LLCP_SetConfigurationPresetReqMsg_t is too large");

//}

// | ----------------------- Acknowledge ---------------------- |

/* LLCP_AckMsg_t //{ */

/* LLCP_Ack_t //{ */

/**
 * @brief Message data for LLCP_AckMsg_t
 */
typedef struct __attribute__((packed))
{
  uint8_t success;
} LLCP_Ack_t;

/**
 * @brief host-to-network conversion for LLCP_Ack_t
 *
 * @param data
 */
void hton_Ack_t(LLCP_Ack_t* data);

/**
 * @brief network-to-host conversion for LLCP_Ack_t
 *
 * @param data
 */
void ntoh_Ack_t(LLCP_Ack_t* data);

/**
 * @brief "constructor" for LLCP_Ack_t (default = success = 1)
 *
 * @param data
 */
void init_Ack_t(LLCP_Ack_t* data);

//}

/**
 * @brief LLCP Message for sending "ack".
 */
typedef struct __attribute__((packed))
{
  uint8_t    message_id;
  LLCP_Ack_t payload;
} LLCP_AckMsg_t;

/**
 * @brief host-to-network conversion for LLCP_AckMsg_t
 *
 * @param msg
 */
void hton_LLCP_AckMsg_t(LLCP_AckMsg_t* msg);

/**
 * @brief network-to-host conversion for LLCP_AckMsg_t
 *
 * @param msg
 */
void ntoh_LLCP_AckMsg_t(LLCP_AckMsg_t* msg);

/**
 * @brief "constructor" for LLCP_AckMsg_t
 *
 * @param msg
 */
void init_LLCP_AckMsg_t(LLCP_AckMsg_t* msg);

static_assert((sizeof(LLCP_AckMsg_t) > 255) == 0, "LLCP_AckMsg_t is too large");

//}

// | -------------------------- Power ------------------------- |

/* LLCP_PwrReqMsg_t //{ */

/* LLCP_PwrReq_t //{ */

/**
 * @brief Message data for LLCP_PwrReqMsg_t
 */
typedef struct __attribute__((packed))
{
  uint8_t state;  // 1 = ON, 0 = OFF
} LLCP_PwrReq_t;

/**
 * @brief host-to-network conversion for LLCP_PwrReq_t
 *
 * @param data
 */
void hton_LLCP_PwrReq_t(LLCP_PwrReq_t* data);

/**
 * @brief network-to-host conversion for LLCP_PwrReq_t
 *
 * @param data
 */
void ntoh_LLCP_PwrReq_t(LLCP_PwrReq_t* data);

/**
 * @brief "constructor" for LLCP_PwrReq_t
 *
 * @param data
 */
void init_LLCP_PwrReq_t(LLCP_PwrReq_t* data);

//}

/**
 * @brief LLCP Message for requesting a frame measurement.
 */
typedef struct __attribute__((packed))
{
  uint8_t       message_id;
  LLCP_PwrReq_t payload;
} LLCP_PwrReqMsg_t;

/**
 * @brief host-to-network conversion for LLCP_PwrReqMsg_t
 *
 * @param msg
 */
void hton_LLCP_PwrReqMsg_t(LLCP_PwrReqMsg_t* msg);

/**
 * @brief network-to-host conversion for LLCP_PwrReqMsg_t
 *
 * @param msg
 */
void ntoh_LLCP_PwrReqMsg_t(LLCP_PwrReqMsg_t* msg);

/**
 * @brief "constructor" for LLCP_PwrReqMsg_t
 *
 * @param msg
 */
void init_LLCP_PwrReqMsg_t(LLCP_PwrReqMsg_t* msg);

static_assert((sizeof(LLCP_PwrReqMsg_t) > 255) == 0, "LLCP_PwrReqMsg_t is too large");

//}

// | ------------------------- Status ------------------------- |

/* LLCP_StatusMsg_t //{ */

/* LLCP_Status_t //{ */

#define LLCP_STATUS_STR_LEN 128

/**
 * @brief Message data for LLCP_StatusMsg_t
 */
typedef struct __attribute__((packed))
{
  uint16_t boot_count;
  uint8_t  status_str[LLCP_STATUS_STR_LEN];
} LLCP_Status_t;

/**
 * @brief host-to-network conversion for LLCP_Status_t
 *
 * @param data
 */
void hton_LLCP_Status_t(LLCP_Status_t* data);

/**
 * @brief network-to-host conversion for LLCP_Status_t
 *
 * @param data
 */
void ntoh_LLCP_Status_t(LLCP_Status_t* data);

/**
 * @brief "constructor" for LLCP_Status_t
 *
 * @param data
 */
void init_LLCP_Status_t(LLCP_Status_t* data);

//}

/**
 * @brief LLCP Message for MiniPIX status.
 */
typedef struct __attribute__((packed))
{
  uint8_t       message_id;
  LLCP_Status_t payload;
} LLCP_StatusMsg_t;

/**
 * @brief host-to-network conversion for LLCP_StatusMsg_t
 *
 * @param msg
 */
void hton_LLCP_StatusMsg_t(LLCP_StatusMsg_t* msg);

/**
 * @brief network-to-host conversion for LLCP_StatusMsg_t
 *
 * @param msg
 */
void ntoh_LLCP_StatusMsg_t(LLCP_StatusMsg_t* msg);

/**
 * @brief "constructor" for LLCP_StatusMsg_t
 *
 * @param msg
 */
void init_LLCP_StatusMsg_t(LLCP_StatusMsg_t* msg);

static_assert((sizeof(LLCP_StatusMsg_t) > 255) == 0, "LLCP_StatusMsg_t is too large");

//}

/* LLCP_GetStatusReqMsg_t //{ */

/**
 * @brief LLCP Message for requesting a MiniPIX status.
 */
typedef struct __attribute__((packed))
{
  uint8_t message_id;
} LLCP_GetStatusReqMsg_t;

/**
 * @brief host-to-network conversion for LLCP_GetStatusReqMsg_t
 *
 * @param msg
 */
void hton_LLCP_GetStatusReqMsg_t(LLCP_GetStatusReqMsg_t* msg);

/**
 * @brief network-to-host conversion for LLCP_GetStatusReqMsg_t
 *
 * @param msg
 */
void ntoh_LLCP_GetStatusReqMsg_t(LLCP_GetStatusReqMsg_t* msg);

/**
 * @brief "constructor" for LLCP_GetStatusReqMsg_t
 *
 * @param msg
 */
void init_LLCP_GetStatusReqMsg_t(LLCP_GetStatusReqMsg_t* msg);

static_assert((sizeof(LLCP_GetStatusReqMsg_t) > 255) == 0, "LLCP_GetStatusReqMsg_t is too large");

//}

/* LLCP_TemperatureMsg_t //{ */

/* LLCP_Temperature_t //{ */

/**
 * @brief Message data for LLCP_TemperatureMsg_t
 */
typedef struct __attribute__((packed))
{
  int16_t temperature;
} LLCP_Temperature_t;

/**
 * @brief host-to-network conversion for LLCP_Temperature_t
 *
 * @param data
 */
void hton_LLCP_Temperature_t(LLCP_Temperature_t* data);

/**
 * @brief network-to-host conversion for LLCP_Temperature_t
 *
 * @param data
 */
void ntoh_LLCP_Temperature_t(LLCP_Temperature_t* data);

/**
 * @brief "constructor" for LLCP_Temperature_t
 *
 * @param data
 */
void init_LLCP_Temperature_t(LLCP_Temperature_t* data);

//}

/**
 * @brief LLCP Message for MiniPIX temperature.
 */
typedef struct __attribute__((packed))
{
  uint8_t            message_id;
  LLCP_Temperature_t payload;
} LLCP_TemperatureMsg_t;

/**
 * @brief host-to-network conversion for LLCP_TemperatureMsg_t
 *
 * @param msg
 */
void hton_LLCP_TemperatureMsg_t(LLCP_TemperatureMsg_t* msg);

/**
 * @brief network-to-host conversion for LLCP_TemperatureMsg_t
 *
 * @param msg
 */
void ntoh_LLCP_TemperatureMsg_t(LLCP_TemperatureMsg_t* msg);

/**
 * @brief "constructor" for LLCP_TemperatureMsg_t
 *
 * @param msg
 */
void init_LLCP_TemperatureMsg_t(LLCP_TemperatureMsg_t* msg);

static_assert((sizeof(LLCP_TemperatureMsg_t) > 255) == 0, "LLCP_TemperatureMsg_t is too large");

//}

/* LLCP_GetTemperatureReqMsg_t //{ */

/**
 * @brief LLCP Message for requesting a MiniPIX temperature.
 */
typedef struct __attribute__((packed))
{
  uint8_t message_id;
} LLCP_GetTemperatureReqMsg_t;

/**
 * @brief host-to-network conversion for LLCP_GetTemperatureReqMsg_t
 *
 * @param msg
 */
void hton_LLCP_GetTemperatureReqMsg_t(LLCP_GetTemperatureReqMsg_t* msg);

/**
 * @brief network-to-host conversion for LLCP_GetTemperatureReqMsg_t
 *
 * @param msg
 */
void ntoh_LLCP_GetTemperatureReqMsg_t(LLCP_GetTemperatureReqMsg_t* msg);

/**
 * @brief "constructor" for LLCP_GetTemperatureReqMsg_t
 *
 * @param msg
 */
void init_LLCP_GetTemperatureReqMsg_t(LLCP_GetTemperatureReqMsg_t* msg);

static_assert((sizeof(LLCP_GetTemperatureReqMsg_t) > 255) == 0, "LLCP_GetTemperatureReqMsg_t is too large");

//}

// | ------------------------- Errors ------------------------- |

/* LLCP_MinipixErrorMsg_t //{ */

/* LLCP_MinipixError_t //{ */

/**
 * @brief Message data for LLCP_MinipixErrorMsg_t
 */
typedef struct __attribute__((packed))
{
  uint8_t error_id;
} LLCP_MinipixError_t;

/**
 * @brief host-to-network conversion for LLCP_MinipixError_t
 *
 * @param data
 */
void hton_LLCP_MinipixError_t(LLCP_MinipixError_t* data);

/**
 * @brief network-to-host conversion for LLCP_MinipixError_t
 *
 * @param data
 */
void ntoh_LLCP_MinipixError_t(LLCP_MinipixError_t* data);

/**
 * @brief "constructor" for LLCP_MinipixError_t
 *
 * @param data
 */
void init_LLCP_MinipixError_t(LLCP_MinipixError_t* data);

//}

/**
 * @brief LLCP Message for error from MiniPIX.
 */
typedef struct __attribute__((packed))
{
  uint8_t             message_id;
  LLCP_MinipixError_t payload;
} LLCP_MinipixErrorMsg_t;

/**
 * @brief host-to-network conversion for LLCP_MinipixErrorMsg_t
 *
 * @param msg
 */
void hton_LLCP_MinipixErrorMsg_t(LLCP_MinipixErrorMsg_t* msg);

/**
 * @brief network-to-host conversion for LLCP_MinipixErrorMsg_t
 *
 * @param msg
 */
void ntoh_LLCP_MinipixErrorMsg_t(LLCP_MinipixErrorMsg_t* msg);

/**
 * @brief "constructor" for LLCP_MinipixErrorMsg_t
 *
 * @param msg
 */
void init_LLCP_MinipixErrorMsg_t(LLCP_MinipixErrorMsg_t* msg);

static_assert((sizeof(LLCP_MinipixErrorMsg_t) > 255) == 0, "LLCP_MinipixErrorMsg_t is too large");

//}

#ifdef __cplusplus
}
#endif

#endif  // LLCP_MINIPIX_MESSAGES_H
