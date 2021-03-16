#include <llcp_minipix_messages.h>

/* ImageData_t //{ */

void hton_ImageData_t(ImageData_t* data) {
  data->frame_id = llcp_hton16(data->frame_id);
}

void ntoh_ImageData_t(ImageData_t* data) {
  data->frame_id = llcp_ntoh16(data->frame_id);
}

//}

/* LLCP_ImageDataMsg_t //{ */

void hton_LLCP_ImageDataMsg_t(LLCP_ImageDataMsg_t* msg) {
  hton_ImageData_t(&msg->payload);
}

void ntoh_LLCP_ImageDataMsg_t(LLCP_ImageDataMsg_t* msg) {
  ntoh_ImageData_t(&msg->payload);
}

//}

/* MeasureFrameReq_t //{ */

void hton_MeasureFrameReq_t(MeasureFrameReq_t* data) {
  data->acquisition_time_ms = llcp_hton16((uint16_t)data->acquisition_time_ms);
}

void ntoh_MeasureFrameReq_t(MeasureFrameReq_t* data) {
  data->acquisition_time_ms = llcp_ntoh16((uint16_t)data->acquisition_time_ms);
}

//}

/* LLCP_MeasureFrameReqMsg_t //{ */

void hton_LLCP_MeasureFrameReqMsg_t(LLCP_MeasureFrameReqMsg_t* data) {

  hton_MeasureFrameReq_t(&data->payload);
}

void ntoh_LLCP_MeasureFrameReqMsg_t(LLCP_MeasureFrameReqMsg_t* data) {

  ntoh_MeasureFrameReq_t(&data->payload);
}

//}

/* LLCP_FrameDataAckMsg_t //{ */

void hton_LLCP_FrameDataAckMsg_t(LLCP_FrameDataAckMsg_t* data) {
}

void ntoh_LLCP_FrameDataAckMsg_t(LLCP_FrameDataAckMsg_t* data) {
}

//}

/* Status_t //{ */

void hton_Status_t(Status_t* data) {
  data->boot_count = llcp_hton16(data->boot_count);
}

void ntoh_Status_t(Status_t* data) {
  data->boot_count = llcp_ntoh16(data->boot_count);
}

//}

/* LLCP_StatusMsg_t //{ */

void hton_LLCP_StatusMsg_t(LLCP_StatusMsg_t* data) {

  hton_Status_t(&data->payload);
}

void ntoh_LLCP_StatusMsg_t(LLCP_StatusMsg_t* data) {

  ntoh_Status_t(&data->payload);
}

//}

/* LLCP_GetStatusMsg_t //{ */

void hton_LLCP_GetStatusMsg_t(LLCP_GetStatusMsg_t* data) {
}

void ntoh_LLCP_GetStatusMsg_t(LLCP_GetStatusMsg_t* data) {
}

//}
