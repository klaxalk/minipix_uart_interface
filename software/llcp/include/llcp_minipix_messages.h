#ifndef LLCP_MINIPIX_MESSAGES_H
#define LLCP_MINIPIX_MESSAGES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <assert.h>
#include <llcp_endian.h>

typedef enum
{
  LLCP_IMAGE_DATA_MSG_ID     = 0,
  LLCP_MEASURE_FRAME_MSG_ID  = 1,
  LLCP_STATUS_MSG_ID         = 2,
  LLCP_GET_STATUS_MSG_ID     = 3,
  LLCP_FRAME_DATA_ACK_MSG_ID = 4,
} LLCPMessageId_t;

// | -------------------------- Frame ------------------------- |

/* ImageDataMsg_t //{ */

/* struct PixelData_t //{ */

typedef struct __attribute__((packed))
{
  uint8_t x_coordinate;
  uint8_t y_coordinate;
  uint8_t data[6];
} PixelData_t;

//}

/* struct ImageData_t //{ */

typedef struct __attribute__((packed))
{
  uint16_t    frame_id;
  uint8_t     n_pixels;
  PixelData_t pixel_data[31];
} ImageData_t;

void hton_ImageData_t(ImageData_t* data);

void ntoh_ImageData_t(ImageData_t* data);

//}

typedef struct __attribute__((packed))
{
  LLCPMessageId_t message_id;
  ImageData_t     payload;
} ImageDataMsg_t;

void hton_ImageDataMsg_t(ImageDataMsg_t* msg);

void ntoh_ImageDataMsg_t(ImageDataMsg_t* msg);

static_assert((sizeof(ImageDataMsg_t) > 255) == 0, "ImageDataMsg_t is too large");

//}

/* MeasureFrameReqMsg_t //{ */

/* MeasureFrameReq_t //{ */

typedef struct __attribute__((packed))
{
  uint16_t acquisition_time_ms;
} MeasureFrameReq_t;

void hton_MeasureFrameReq_t(MeasureFrameReq_t* data);

void ntoh_MeasureFrameReq_t(MeasureFrameReq_t* data);

//}

typedef struct __attribute__((packed))
{
  LLCPMessageId_t   message_id;
  MeasureFrameReq_t payload;
} MeasureFrameReqMsg_t;

void hton_MeasureFrameReqMsg_t(MeasureFrameReqMsg_t* data);

void ntoh_MeasureFrameReqMsg_t(MeasureFrameReqMsg_t* data);

static_assert((sizeof(MeasureFrameReqMsg_t) > 255) == 0, "MeasureFrameReqMsg_t is too large");

//}

/* FrameDataAckMsg_t //{ */

typedef struct __attribute__((packed))
{
  LLCPMessageId_t message_id;
} FrameDataAckMsg_t;

void hton_FrameDataAckMsg_t(FrameDataAckMsg_t* data);

void ntoh_FrameDataAckMsg_t(FrameDataAckMsg_t* data);

static_assert((sizeof(FrameDataAckMsg_t) > 255) == 0, "FrameDataAckMsg_t is too large");

//}

// | ------------------------- status ------------------------- |

/* StatusMsg_t //{ */

/* Status_t //{ */

typedef struct __attribute__((packed))
{
  uint16_t boot_count;
  uint8_t  status_str[128];
} Status_t;

void hton_Status_t(Status_t* data);

void ntoh_Status_t(Status_t* data);

//}

typedef struct __attribute__((packed))
{
  LLCPMessageId_t message_id;
  Status_t        payload;
} StatusMsg_t;

void hton_StatusMsg_t(StatusMsg_t* data);

void ntoh_StatusMsg_t(StatusMsg_t* data);

static_assert((sizeof(StatusMsg_t) > 255) == 0, "StatusMsg_t is too large");

//}

/* GetStatusMsg_t //{ */

typedef struct __attribute__((packed))
{
  LLCPMessageId_t message_id;
} GetStatusMsg_t;

void hton_GetStatusMsg_t(GetStatusMsg_t* data);

void ntoh_GetStatusMsg_t(GetStatusMsg_t* data);

static_assert((sizeof(GetStatusMsg_t) > 255) == 0, "GetStatusMsg_t is too large");

//}

#ifdef __cplusplus
}
#endif

#endif  // LLCP_MINIPIX_MESSAGES_H
