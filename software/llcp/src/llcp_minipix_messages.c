#include <llcp_minipix_messages.h>

/* ImageData_t //{ */

void hton_ImageData_t(ImageData_t* data) {
  data->frame_id = llcp_hton16(data->frame_id);
}

void ntoh_ImageData_t(ImageData_t* data) {
  data->frame_id = llcp_ntoh16(data->frame_id);
}

//}

/* ImageDataMsg_t //{ */

void hton_ImageDataMsg_t(ImageDataMsg_t* msg) {
  hton_ImageData_t(&msg->payload);
}

void ntoh_ImageDataMsg_t(ImageDataMsg_t* msg) {
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

/* MeasureFrameReqMsg_t //{ */

void hton_MeasureFrameReqMsg_t(MeasureFrameReqMsg_t* data) {

  hton_MeasureFrameReq_t(&data->payload);
}

void ntoh_MeasureFrameReqMsg_t(MeasureFrameReqMsg_t* data) {

  ntoh_MeasureFrameReq_t(&data->payload);
}

//}

/* FrameDataAckMsg_t //{ */

void hton_FrameDataAckMsg_t(FrameDataAckMsg_t* data) {
}

void ntoh_FrameDataAckMsg_t(FrameDataAckMsg_t* data) {
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

/* StatusMsg_t //{ */

void hton_StatusMsg_t(StatusMsg_t* data) {

  hton_Status_t(&data->payload);
}

void ntoh_StatusMsg_t(StatusMsg_t* data) {

  ntoh_Status_t(&data->payload);
}

//}

/* GetStatusMsg_t //{ */

void hton_GetStatusMsg_t(GetStatusMsg_t* data) {
}

void ntoh_GetStatusMsg_t(GetStatusMsg_t* data) {
}

//}
