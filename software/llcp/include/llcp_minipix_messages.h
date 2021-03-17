#ifndef LLCP_MINIPIX_MESSAGES_H
#define LLCP_MINIPIX_MESSAGES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <assert.h>
#include <llcp_endian.h>

#define LLCP_IMAGE_DATA_MSG_ID 0
#define LLCP_MEASURE_FRAME_MSG_ID 1
#define LLCP_STATUS_MSG_ID 2
#define LLCP_GET_STATUS_MSG_ID 3
#define LLCP_ACK_MSG_ID 4

// | ------------- Frame data (integrated images) ------------- |

/* LLCP_ImageDataMsg_t //{ */

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
  uint8_t     message_id;
  ImageData_t payload;
} LLCP_ImageDataMsg_t;

void hton_LLCP_ImageDataMsg_t(LLCP_ImageDataMsg_t* msg);

void ntoh_LLCP_ImageDataMsg_t(LLCP_ImageDataMsg_t* msg);

static_assert((sizeof(LLCP_ImageDataMsg_t) > 255) == 0, "LLCP_ImageDataMsg_t is too large");

//}

/* LLCP_MeasureFrameReqMsg_t //{ */

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
  uint8_t           message_id;
  MeasureFrameReq_t payload;
} LLCP_MeasureFrameReqMsg_t;

void hton_LLCP_MeasureFrameReqMsg_t(LLCP_MeasureFrameReqMsg_t* data);

void ntoh_LLCP_MeasureFrameReqMsg_t(LLCP_MeasureFrameReqMsg_t* data);

static_assert((sizeof(LLCP_MeasureFrameReqMsg_t) > 255) == 0, "LLCP_MeasureFrameReqMsg_t is too large");

//}

// | ------------------- Ackwnoledge message ------------------ |

/* LLCP_AckMsg_t //{ */

/* Ack_t //{ */

typedef struct __attribute__((packed))
{
  uint8_t success;
} Ack_t;

void hton_Ack_t(Ack_t* data);

void ntoh_Ack_t(Ack_t* data);

//}

typedef struct __attribute__((packed))
{
  uint8_t message_id;
  Ack_t   payload;
} LLCP_AckMsg_t;

void hton_LLCP_AckMsg_t(LLCP_AckMsg_t* data);

void ntoh_LLCP_AckMsg_t(LLCP_AckMsg_t* data);

static_assert((sizeof(LLCP_AckMsg_t) > 255) == 0, "LLCP_AckMsg_t is too large");

//}

// | ------------------------- status ------------------------- |

/* LLCP_StatusMsg_t //{ */

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
  uint8_t  message_id;
  Status_t payload;
} LLCP_StatusMsg_t;

void hton_LLCP_StatusMsg_t(LLCP_StatusMsg_t* data);

void ntoh_LLCP_StatusMsg_t(LLCP_StatusMsg_t* data);

static_assert((sizeof(LLCP_StatusMsg_t) > 255) == 0, "LLCP_StatusMsg_t is too large");

//}

/* LLCP_GetStatusMsg_t //{ */

typedef struct __attribute__((packed))
{
  uint8_t message_id;
} LLCP_GetStatusMsg_t;

void hton_LLCP_GetStatusMsg_t(LLCP_GetStatusMsg_t* data);

void ntoh_LLCP_GetStatusMsg_t(LLCP_GetStatusMsg_t* data);

static_assert((sizeof(LLCP_GetStatusMsg_t) > 255) == 0, "LLCP_GetStatusMsg_t is too large");

//}

#ifdef __cplusplus
}
#endif

#endif  // LLCP_MINIPIX_MESSAGES_H
