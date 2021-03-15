#ifndef LLCP_MINIPIX_MESSAGES_H
#define LLCP_MINIPIX_MESSAGES_H

#include <stdint.h>
#include <assert.h>
#include <llcp_endian.h>

enum LLCPMessageId_t : uint8_t
{
  LLCP_IMAGE_DATA_MSG_ID     = 0,
  LLCP_MEASURE_FRAME_MSG_ID  = 1,
  LLCP_STATUS_MSG_ID         = 2,
  LLCP_GET_STATUS_MSG_ID     = 3,
  LLCP_FRAME_DATA_ACK_MSG_ID = 4,
};

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

void hton_ImageData_t(ImageData_t* data) {
  data->frame_id = llcp_hton16(data->frame_id);
}

void ntoh_ImageData_t(ImageData_t* data) {
  data->frame_id = llcp_ntoh16(data->frame_id);
}

//}

struct ImageDataMsg_t
{
  const LLCPMessageId_t message_id = LLCP_IMAGE_DATA_MSG_ID;
  ImageData_t         payload;
} __attribute__((packed));

void hton_ImageDataMsg_t(ImageDataMsg_t* msg) {
  hton_ImageData_t(&msg->payload);
}

void ntoh_ImageDataMsg_t(ImageDataMsg_t* msg) {
  ntoh_ImageData_t(&msg->payload);
}

static_assert((sizeof(ImageDataMsg_t) > 255) == 0, "ImageDataMsg_t is too large");

//}

/* MeasureFrameReqMsg_t //{ */

/* MeasureFrameReq_t //{ */

typedef struct __attribute__((packed))
{
  uint16_t acquisition_time_ms;
} MeasureFrameReq_t;

void hton_MeasureFrameReq_t(MeasureFrameReq_t* data) {
  data->acquisition_time_ms = llcp_hton16(data->acquisition_time_ms);
}

void ntoh_MeasureFrameReq_t(MeasureFrameReq_t* data) {
  data->acquisition_time_ms = llcp_ntoh16(data->acquisition_time_ms);
}

//}

struct MeasureFrameReqMsg_t
{
  const LLCPMessageId_t message_id = LLCP_MEASURE_FRAME_MSG_ID;
  MeasureFrameReq_t   payload;
} __attribute__((packed));

void hton_MeasureFrameReqMsg_t(MeasureFrameReqMsg_t* data) {

  hton_MeasureFrameReq_t(&data->payload);
}

void ntoh_MeasureFrameReqMsg_t(MeasureFrameReqMsg_t* data) {

  ntoh_MeasureFrameReq_t(&data->payload);
}

static_assert((sizeof(MeasureFrameReqMsg_t) > 255) == 0, "MeasureFrameReqMsg_t is too large");

//}

/* FrameDataAckMsg_t //{ */

struct FrameDataAckMsg_t
{
  const LLCPMessageId_t message_id = LLCP_FRAME_DATA_ACK_MSG_ID;
} __attribute__((packed));

void hton_FrameDataAckMsg_t([[maybe_unused]] FrameDataAckMsg_t* data) {
}

void ntoh_FrameDataAckMsg_t([[maybe_unused]] FrameDataAckMsg_t* data) {
}

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

void hton_Status_t(Status_t* data) {
  data->boot_count = llcp_hton16(data->boot_count);
}

void ntoh_Status_t(Status_t* data) {
  data->boot_count = llcp_ntoh16(data->boot_count);
}

//}

struct StatusMsg_t
{
  const LLCPMessageId_t message_id = LLCP_STATUS_MSG_ID;
  Status_t            payload;
} __attribute__((packed));

void hton_StatusMsg_t(StatusMsg_t* data) {

  hton_Status_t(&data->payload);
}

void ntoh_StatusMsg_t(StatusMsg_t* data) {

  ntoh_Status_t(&data->payload);
}

static_assert((sizeof(StatusMsg_t) > 255) == 0, "StatusMsg_t is too large");

//}

/* GetStatusMsg_t //{ */

struct GetStatusMsg_t
{
  const LLCPMessageId_t message_id = LLCP_GET_STATUS_MSG_ID;
} __attribute__((packed));

void hton_GetStatusMsg_t([[maybe_unused]] GetStatusMsg_t* data) {
}

void ntoh_GetStatusMsg_t([[maybe_unused]] GetStatusMsg_t* data) {
}

static_assert((sizeof(GetStatusMsg_t) > 255) == 0, "GetStatusMsg_t is too large");

//}

#endif  // LLCP_MINIPIX_MESSAGES_H
