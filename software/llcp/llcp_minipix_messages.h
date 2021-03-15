#ifndef LLCP_MINIPIX_MESSAGES_H
#define LLCP_MINIPIX_MESSAGES_H

#include <stdint.h>
#include <assert.h>
#include <llcp_endian.h>

enum MessageId_t : uint8_t
{
  LLCP_IMAGE_DATA_MSG_ID = 0,
  LLCP_GET_FRAME_MSG_ID  = 1,
  LLCP_STATUS_MSG_ID     = 2,
  LLCP_GET_STATUS_MSG_ID = 3,
};

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
  const MessageId_t message_id = LLCP_IMAGE_DATA_MSG_ID;
  ImageData_t       image_data;
} __attribute__((packed));

void hton_ImageDataMsg_t(ImageDataMsg_t* data) {
  hton_ImageData_t(&data->image_data);
}

void ntoh_ImageDataMsg_t(ImageDataMsg_t* data) {
  ntoh_ImageData_t(&data->image_data);
}

static_assert((sizeof(ImageDataMsg_t) > 255) == 0, "ImageDataMsg_t is too large");

//}

/* GetFrameMsg_t //{ */

struct GetFrameMsg_t
{
  const MessageId_t message_id = LLCP_GET_FRAME_MSG_ID;
  uint16_t          acquisition_time_ms;
} __attribute__((packed));

void hton_GetFrameMsg_t(GetFrameMsg_t* data) {

  data->acquisition_time_ms = llcp_hton16(data->acquisition_time_ms);
}

void ntoh_GetFrameMsg_t(GetFrameMsg_t* data) {

  data->acquisition_time_ms = llcp_ntoh16(data->acquisition_time_ms);
}

static_assert((sizeof(GetFrameMsg_t) > 255) == 0, "GetFrameMsg_t is too large");

//}

/* StatusMsg_t //{ */

struct StatusMsg_t
{
  const MessageId_t message_id = LLCP_STATUS_MSG_ID;
  uint16_t          boot_count;
} __attribute__((packed));

void hton_StatusMsg_t(StatusMsg_t* data) {

  data->boot_count = llcp_hton16(data->boot_count);
}

void ntoh_StatusMsg_t(StatusMsg_t* data) {

  data->boot_count = llcp_ntoh16(data->boot_count);
}

static_assert((sizeof(StatusMsg_t) > 255) == 0, "StatusMsg_t is too large");

//}

/* GetStatusMsg_t //{ */

struct GetStatusMsg_t
{
  const MessageId_t message_id = LLCP_GET_STATUS_MSG_ID;
} __attribute__((packed));

void hton_GetStatusMsg_t([[maybe_unused]] GetStatusMsg_t* data) {
}

void ntoh_GetStatusMsg_t([[maybe_unused]] GetStatusMsg_t* data) {
}

static_assert((sizeof(GetStatusMsg_t) > 255) == 0, "GetStatusMsg_t is too large");

//}

#endif  // LLCP_MINIPIX_MESSAGES_H
