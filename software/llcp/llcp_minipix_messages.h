#ifndef LLCP_MINIPIX_MESSAGES_H
#define LLCP_MINIPIX_MESSAGES_H

#include <stdint.h>
#include <assert.h>
#include <llcp/llcp_endian.h>

enum MessageId_t : uint8_t
{
  LLCP_IMAGE_DATA_MSG_ID,
  LLCP_GET_FRAME_MSG_ID,
};

/* ImageDataMsg_t //{ */

typedef struct __attribute__((packed))
{
  uint8_t x_coordinate;
  uint8_t y_coordinate;
  uint8_t data[6];
} PixelData_t;

typedef struct __attribute__((packed))
{
  uint16_t    frame_id;
  uint8_t     n_pixels;
  PixelData_t pixel_data[31];
} ImageData_t;

struct ImageDataMsg_t
{
  const MessageId_t message_id = LLCP_IMAGE_DATA_MSG_ID;
  ImageData_t       image_data;
} __attribute__((packed));

static_assert((sizeof(ImageDataMsg_t) > 255) == 0, "ImageDataMsg_t is too large");

//}

/* GetFrameMsg_t //{ */

struct GetFrameMsg_t
{
  const MessageId_t message_id = LLCP_GET_FRAME_MSG_ID;
  uint16_t          acquisition_time_ms;
} __attribute__((packed));

void hton(GetFrameMsg_t* data) {

  data->acquisition_time_ms = llcp_hton16(data->acquisition_time_ms);
}

void ntoh(GetFrameMsg_t* data) {

  data->acquisition_time_ms = llcp_ntoh16(data->acquisition_time_ms);
}

static_assert((sizeof(GetFrameMsg_t) > 255) == 0, "GetFrameMsg_t is too large");

//}

#endif  // LLCP_MINIPIX_MESSAGES_H
