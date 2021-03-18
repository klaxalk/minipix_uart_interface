#ifndef LLCP_MINIPIX_MESSAGES_H
#define LLCP_MINIPIX_MESSAGES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <assert.h>
#include <llcp_endian.h>

#define LLCP_STATUS_MSG_ID 0
#define LLCP_GET_STATUS_MSG_ID 1
#define LLCP_ACK_MSG_ID 2
#define LLCP_IMAGE_DATA_MSG_ID 3
#define LLCP_MEASURE_FRAME_MSG_ID 4
#define LLCP_UPDATE_PIXEL_MASK_REQ_MSG_ID 5

// | ------- Pixel data, common to frame and stream mode ------ |

/* LLCP_PixelData_t //{ */

/**
 * @brief Structure for holding pixel coordinates and measured pixel values.
 */
typedef struct __attribute__((packed))
{
  uint8_t x_coordinate;
  uint8_t y_coordinate;
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

//}

// | ------------- Frame data (integrated images) ------------- |

/* LLCP_FrameDataMsg_t //{ */

/* struct LLCP_FrameData_t //{ */

/**
 * @brief Message data for LLCP_FrameDataMsg_t
 */
typedef struct __attribute__((packed))
{
  uint16_t         frame_id;  // a unique identifier of the frame, can be used to stitch the packets together
  uint8_t          n_pixels;  // how many pixels are filled in
  LLCP_PixelData_t pixel_data[31];
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

static_assert((sizeof(LLCP_FrameDataMsg_t) > 255) == 0, "LLCP_FrameDataMsg_t is too large");

//}

/* LLCP_MeasureFrameReqMsg_t //{ */

/* MeasureFrameReq_t //{ */

/**
 * @brief Message data for LLCP_MeasureFrameReqMsg_t
 */
typedef struct __attribute__((packed))
{
  uint16_t acquisition_time_ms;
} MeasureFrameReq_t;

/**
 * @brief host-to-network conversion for MeasureFrameReq_t
 *
 * @param data
 */
void hton_LLCP_MeasureFrameReq_t(MeasureFrameReq_t* data);

/**
 * @brief network-to-host conversion for MeasureFrameReq_t
 *
 * @param data
 */
void ntoh_LLCP_MeasureFrameReq_t(MeasureFrameReq_t* data);

//}

/**
 * @brief LLCP Message for requesting a frame measurement.
 */
typedef struct __attribute__((packed))
{
  uint8_t           message_id;
  MeasureFrameReq_t payload;
} LLCP_MeasureFrameReqMsg_t;

/**
 * @brief host-to-network conversion for LLCP_MeasureFrameReqMsg_t
 *
 * @param data
 */
void hton_LLCP_MeasureFrameReqMsg_t(LLCP_MeasureFrameReqMsg_t* data);

/**
 * @brief network-to-host conversion for LLCP_MeasureFrameReqMsg_t
 *
 * @param data
 */
void ntoh_LLCP_MeasureFrameReqMsg_t(LLCP_MeasureFrameReqMsg_t* data);

static_assert((sizeof(LLCP_MeasureFrameReqMsg_t) > 255) == 0, "LLCP_MeasureFrameReqMsg_t is too large");

//}

// | ---------- Stream data (continuous event stream) --------- |

/* LLCP_StreamDataMsg_t //{ */

/* struct LLCP_StreamData_t //{ */

/**
 * @brief Message data for LLCP_StreamDataMsg_t
 */
typedef struct __attribute__((packed))
{
  uint8_t          full_buffer;  // 1 if MiniPIX has a full buffer
  uint8_t          n_pixels;     // how many pixels are filled in
  LLCP_PixelData_t pixel_data[31];
} LLCP_StreamData_t;

/**
 * @brief host-to-network conversion for LLCP_StreamData_t
 *
 * @param data
 */
void hton_LLCP_StreamData_t(LLCP_StreamData_t* data);

/**
 * @brief network-to-host conversion for LLCP_StreamData_t
 *
 * @param data
 */
void ntoh_LLCP_StreamData_t(LLCP_StreamData_t* data);

//}

/**
 * @brief LLCP Message for sending image stream.
 */
typedef struct __attribute__((packed))
{
  uint8_t           message_id;
  LLCP_StreamData_t payload;
} LLCP_StreamDataMsg_t;

/**
 * @brief host-to-network conversion for LLCP_StreamDataMsg_t
 *
 * @param data
 */
void hton_LLCP_StreamDataMsg_t(LLCP_StreamDataMsg_t* msg);

/**
 * @brief network-to-host conversion for LLCP_StreamDataMsg_t
 *
 * @param data
 */
void ntoh_LLCP_StreamDataMsg_t(LLCP_StreamDataMsg_t* msg);

static_assert((sizeof(LLCP_StreamDataMsg_t) > 255) == 0, "LLCP_StreamDataMsg_t is too large");

//}

/* LLCP_MeasureStreamReqMsg_t //{ */

/* LLCP_MeasureStreamReq_t //{ */

/**
 * @brief Message data for LLCP_MeasureStreamReq_t
 */
typedef struct __attribute__((packed))
{
  uint16_t duty_cycle_ms;  // duty cycle in ms per second, TOOD possible?
} LLCP_MeasureStreamReq_t;

/**
 * @brief host-to-network conversion for LLCP_MeasureStreamReq_t
 *
 * @param data
 */
void hton_LLCP_MeasureStreamReq_t(LLCP_MeasureStreamReq_t* data);

/**
 * @brief network-to-host conversion for LLCP_MeasureStreamReq_t
 *
 * @param data
 */
void ntoh_LLCP_MeasureStreamReq_t(LLCP_MeasureStreamReq_t* data);

//}

/**
 * @brief LLCP Message for requesting of measurement in the stream mode.
 */
typedef struct __attribute__((packed))
{
  uint8_t                 message_id;
  LLCP_MeasureStreamReq_t payload;
} LLCP_MeasureStreamReqMsg_t;

/**
 * @brief host-to-network conversion for LLCP_MeasureStreamReqMsg_t
 *
 * @param data
 */
void hton_LLCP_MeasureStreamReqMsg_t(LLCP_MeasureStreamReqMsg_t* data);

/**
 * @brief network-to-host conversion for LLCP_MeasureStreamReqMsg_t
 *
 * @param data
 */
void ntoh_LLCP_MeasureStreamReqMsg_t(LLCP_MeasureStreamReqMsg_t* data);

static_assert((sizeof(LLCP_MeasureStreamReqMsg_t) > 255) == 0, "LLCP_MeasureStreamReqMsg_t is too large");

//}

/* LLCP_StopStreamReqMsg_t //{ */

/**
 * @brief LLCP Message for requesting of stopping the stream measurement.
 */
typedef struct __attribute__((packed))
{
  uint8_t message_id;
} LLCP_StopStreamReqMsg_t;

/**
 * @brief host-to-network conversion for LLCP_StopStreamReqMsg_t
 *
 * @param data
 */
void hton_LLCP_StopStreamReqMsg_t(LLCP_StopStreamReqMsg_t* data);

/**
 * @brief network-to-host conversion for LLCP_StopStreamReqMsg_t
 *
 * @param data
 */
void ntoh_LLCP_StopStreamReqMsg_t(LLCP_StopStreamReqMsg_t* data);

static_assert((sizeof(LLCP_StopStreamReqMsg_t) > 255) == 0, "LLCP_StopStreamReqMsg_t is too large");

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
 * @param data
 */
void hton_UpdatePixelMaskReqMsg_t(LLCP_UpdatePixelMaskReqMsg_t* data);

/**
 * @brief network-to-host conversion for LLCP_UpdatePixelMaskReqMsg_t
 *
 * @param data
 */
void ntoh_UpdatePixelMaskReqMsg_t(LLCP_UpdatePixelMaskReqMsg_t* data);

static_assert((sizeof(LLCP_UpdatePixelMaskReqMsg_t) > 255) == 0, "LLCP_UpdatePixelMaskReqMsg_t is too large");

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

void hton_Ack_t(LLCP_Ack_t* data);

void ntoh_Ack_t(LLCP_Ack_t* data);

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
 * @param data
 */
void hton_LLCP_AckMsg_t(LLCP_AckMsg_t* data);

/**
 * @brief network-to-host conversion for LLCP_AckMsg_t
 *
 * @param data
 */
void ntoh_LLCP_AckMsg_t(LLCP_AckMsg_t* data);

static_assert((sizeof(LLCP_AckMsg_t) > 255) == 0, "LLCP_AckMsg_t is too large");

//}

// | ------------------------- Status ------------------------- |

/* LLCP_StatusMsg_t //{ */

/* LLCP_Status_t //{ */

/**
 * @brief Message data for LLCP_StatusMsg_t
 */
typedef struct __attribute__((packed))
{
  uint16_t boot_count;
  uint8_t  status_str[128];
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
 * @param data
 */
void hton_LLCP_StatusMsg_t(LLCP_StatusMsg_t* data);

/**
 * @brief network-to-host conversion for LLCP_StatusMsg_t
 *
 * @param data
 */
void ntoh_LLCP_StatusMsg_t(LLCP_StatusMsg_t* data);

static_assert((sizeof(LLCP_StatusMsg_t) > 255) == 0, "LLCP_StatusMsg_t is too large");

//}

/* LLCP_GetStatusMsg_t //{ */

/**
 * @brief LLCP Message for requesting a MiniPIX status.
 */
typedef struct __attribute__((packed))
{
  uint8_t message_id;
} LLCP_GetStatusMsg_t;

/**
 * @brief host-to-network conversion for LLCP_GetStatusMsg_t
 *
 * @param data
 */
void hton_LLCP_GetStatusMsg_t(LLCP_GetStatusMsg_t* data);

/**
 * @brief network-to-host conversion for LLCP_GetStatusMsg_t
 *
 * @param data
 */
void ntoh_LLCP_GetStatusMsg_t(LLCP_GetStatusMsg_t* data);

static_assert((sizeof(LLCP_GetStatusMsg_t) > 255) == 0, "LLCP_GetStatusMsg_t is too large");

//}

#ifdef __cplusplus
}
#endif

#endif  // LLCP_MINIPIX_MESSAGES_H
