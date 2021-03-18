#include <llcp_minipix_messages.h>

#define UNUSED(x) (void)(x)

/* LLCP_PixelData_t //{ */

void hton_LLCP_PixelData_t(LLCP_PixelData_t* data) {

  UNUSED(data);
}

void ntoh_LLCP_PixelData_t(LLCP_PixelData_t* data) {

  UNUSED(data);
}

//}

/* LLCP_FrameDataMsg_t //{ */

/* LLCP_FrameData_t //{ */

void hton_LLCP_FrameData_t(LLCP_FrameData_t* data) {

  data->frame_id = llcp_hton16(data->frame_id);

  for (uint8_t i = 0; i < data->n_pixels; i++) {
    hton_LLCP_PixelData_t(&(data->pixel_data[i]));
  }
}

void ntoh_LLCP_FrameData_t(LLCP_FrameData_t* data) {

  data->frame_id = llcp_ntoh16(data->frame_id);

  for (uint8_t i = 0; i < data->n_pixels; i++) {
    hton_LLCP_PixelData_t(&(data->pixel_data[i]));
  }
}

//}

void hton_LLCP_FrameDataMsg_t(LLCP_FrameDataMsg_t* msg) {

  hton_LLCP_FrameData_t(&msg->payload);
}

void ntoh_LLCP_FrameDataMsg_t(LLCP_FrameDataMsg_t* msg) {

  ntoh_LLCP_FrameData_t(&msg->payload);
}

//}

/* LLCP_MeasureFrameReqMsg_t //{ */

/* LLCP_MeasureFrameReq_t //{ */

void hton_MeasureFrameReq_t(MeasureFrameReq_t* data) {

  data->acquisition_time_ms = llcp_hton16((uint16_t)data->acquisition_time_ms);
}

void ntoh_MeasureFrameReq_t(MeasureFrameReq_t* data) {

  data->acquisition_time_ms = llcp_ntoh16((uint16_t)data->acquisition_time_ms);
}

//}

void hton_LLCP_MeasureFrameReqMsg_t(LLCP_MeasureFrameReqMsg_t* data) {

  hton_MeasureFrameReq_t(&data->payload);
}

void ntoh_LLCP_MeasureFrameReqMsg_t(LLCP_MeasureFrameReqMsg_t* data) {

  ntoh_MeasureFrameReq_t(&data->payload);
}

//}

/* LLCP_StreamDataMsg_t //{ */

/* LLCP_StreamData_t //{ */

void hton_LLCP_StreamData_t(LLCP_StreamData_t* data) {

  for (uint8_t i = 0; i < data->n_pixels; i++) {
    hton_LLCP_PixelData_t(&(data->pixel_data[i]));
  }
};

void ntoh_LLCP_StreamData_t(LLCP_StreamData_t* data) {

  for (uint8_t i = 0; i < data->n_pixels; i++) {
    ntoh_LLCP_PixelData_t(&(data->pixel_data[i]));
  }
};

//}

void hton_LLCP_StreamDataMsg_t(LLCP_StreamDataMsg_t* msg) {

  hton_LLCP_StreamData_t(&msg->payload);
}

void ntoh_LLCP_StreamDataMsg_t(LLCP_StreamDataMsg_t* msg) {

  ntoh_LLCP_StreamData_t(&msg->payload);
}

//}

/* LLCP_MeasureStreamReqMsg_t //{ */

void hton_LLCP_MeasureStreamReqMsg_t(LLCP_MeasureStreamReqMsg_t* data) {

  UNUSED(data);
}

void ntoh_LLCP_MeasureStreamReqMsg_t(LLCP_MeasureStreamReqMsg_t* data) {

  UNUSED(data);
}

//}

/* LLCP_StopStreamReqMsg_t //{ */

void hton_LLCP_StopStreamReqMsg_t(LLCP_StopStreamReqMsg_t* data) {

  UNUSED(data);
}

void ntoh_LLCP_StopStreamReqMsg_t(LLCP_StopStreamReqMsg_t* data) {

  UNUSED(data);
}

//}

/* LLCP_FlushBufferReqMsg_t //{ */

void hton_LLCP_FlushBufferReqMsg_t(LLCP_FlushBufferReqMsg_t* data) {

  UNUSED(data);
}

void ntoh_LLCP_FlushBufferReqMsg_t(LLCP_FlushBufferReqMsg_t* data) {

  UNUSED(data);
}

//}

/* LLCP_UpdatePixelMaskReqMsg_t //{ */

/* LLCP_UpdatePixelMaskReq_t //{ */

void hton_LLCP_UpdatePixelMaskReq_t(LLCP_UpdatePixelMaskReq_t* data) {

  UNUSED(data);
}

void ntoh_LLCP_UpdatePixelMaskReq_t(LLCP_UpdatePixelMaskReq_t* data) {

  UNUSED(data);
}

//}

void hton_UpdatePixelMaskReqMsg_t(LLCP_UpdatePixelMaskReqMsg_t* data) {

  hton_LLCP_UpdatePixelMaskReq_t(&data->payload);
}

void ntoh_UpdatePixelMaskReqMsg_t(LLCP_UpdatePixelMaskReqMsg_t* data) {

  ntoh_LLCP_UpdatePixelMaskReq_t(&data->payload);
}

//}

/* LLCP_AckMsg_t //{ */

/* LLCP_Ack_t //{ */

void hton_LLCP_Ack_t(LLCP_Ack_t* data) {

  UNUSED(data);
}

void ntoh_LLCP_Ack_t(LLCP_Ack_t* data) {

  UNUSED(data);
}

//}

void hton_LLCP_AckMsg_t(LLCP_AckMsg_t* data) {

  hton_LLCP_Ack_t(&data->payload);
}

void ntoh_LLCP_AckMsg_t(LLCP_AckMsg_t* data) {

  ntoh_LLCP_Ack_t(&data->payload);
}

//}

/* LLCP_StatusMsg_t //{ */

/* LLCP_Status_t //{ */

void hton_LLCP_Status_t(LLCP_Status_t* data) {

  data->boot_count = llcp_hton16(data->boot_count);
}

void ntoh_LLCP_Status_t(LLCP_Status_t* data) {

  data->boot_count = llcp_ntoh16(data->boot_count);
}

//}

void hton_LLCP_StatusMsg_t(LLCP_StatusMsg_t* data) {

  hton_LLCP_Status_t(&data->payload);
}

void ntoh_LLCP_StatusMsg_t(LLCP_StatusMsg_t* data) {

  ntoh_LLCP_Status_t(&data->payload);
}

//}

/* LLCP_GetStatusMsg_t //{ */

void hton_LLCP_GetStatusMsg_t(LLCP_GetStatusMsg_t* data) {

  UNUSED(data);
}

void ntoh_LLCP_GetStatusMsg_t(LLCP_GetStatusMsg_t* data) {

  UNUSED(data);
}

//}
