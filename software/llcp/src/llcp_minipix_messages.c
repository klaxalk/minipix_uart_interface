#include <llcp_minipix_messages.h>

#define UNUSED(x) (void)(x)

/* LLCP_PixelData_t //{ */

void hton_LLCP_PixelData_t(LLCP_PixelData_t* data) {

  UNUSED(data);
}

void ntoh_LLCP_PixelData_t(LLCP_PixelData_t* data) {

  UNUSED(data);
}

void init_LLCP_PixelData_t(LLCP_PixelData_t* data) {

  memset(data, 0, sizeof(LLCP_PixelData_t));
}

//}

/* LLCP_FrameDataMsg_t //{ */

/* LLCP_FrameData_t //{ */

void hton_LLCP_FrameData_t(LLCP_FrameData_t* data) {

  data->frame_id  = llcp_hton16(data->frame_id);
  data->packet_id = llcp_hton16(data->packet_id);

  for (uint8_t i = 0; i < data->n_pixels; i++) {
    hton_LLCP_PixelData_t(&(data->pixel_data[i]));
  }
}

void ntoh_LLCP_FrameData_t(LLCP_FrameData_t* data) {

  data->frame_id  = llcp_ntoh16(data->frame_id);
  data->packet_id = llcp_ntoh16(data->packet_id);

  for (uint8_t i = 0; i < data->n_pixels; i++) {
    hton_LLCP_PixelData_t(&(data->pixel_data[i]));
  }
}

void init_LLCP_FrameData_t(LLCP_FrameData_t* data) {

  data->frame_id  = 0;
  data->packet_id = 0;
  data->mode      = 0;

  data->n_pixels  = 0;

  for (uint8_t i = 0; i < LLCP_FRAME_DATA_N_PIXELS; i++) {
    init_LLCP_PixelData_t(&data->pixel_data[i]);
  }
}

//}

void hton_LLCP_FrameDataMsg_t(LLCP_FrameDataMsg_t* msg) {

  hton_LLCP_FrameData_t(&msg->payload);
}

void ntoh_LLCP_FrameDataMsg_t(LLCP_FrameDataMsg_t* msg) {

  ntoh_LLCP_FrameData_t(&msg->payload);
}

void init_LLCP_FrameDataMsg_t(LLCP_FrameDataMsg_t* msg) {

  msg->message_id = LLCP_FRAME_DATA_MSG_ID;

  init_LLCP_FrameData_t(&msg->payload);
}

//}

/* LLCP_FrameDataTerminatorMsg_t //{ */

/* LLCP_FrameDataTerminator_t //{ */

void hton_LLCP_FrameDataTerminator_t(LLCP_FrameDataTerminator_t* data) {

  data->frame_id  = llcp_hton16(data->frame_id);
  data->n_packets = llcp_hton16(data->n_packets);
}

void ntoh_LLCP_FrameDataTerminator_t(LLCP_FrameDataTerminator_t* data) {

  data->frame_id  = llcp_ntoh16(data->frame_id);
  data->n_packets = llcp_ntoh16(data->n_packets);
}

void init_LLCP_FrameDataTerminator_t(LLCP_FrameDataTerminator_t* data) {

  data->frame_id  = 0;
  data->n_packets = 0;
}

//}

void hton_LLCP_FrameDataTerminatorMsg_t(LLCP_FrameDataTerminatorMsg_t* msg) {

  hton_LLCP_FrameDataTerminator_t(&msg->payload);
}

void ntoh_LLCP_FrameDataTerminatorMsg_t(LLCP_FrameDataTerminatorMsg_t* msg) {

  ntoh_LLCP_FrameDataTerminator_t(&msg->payload);
}

void init_LLCP_FrameDataTerminatorMsg_t(LLCP_FrameDataTerminatorMsg_t* msg) {

  msg->message_id = LLCP_FRAME_DATA_TERMINATOR_MSG_ID;

  init_LLCP_FrameDataTerminator_t(&msg->payload);
}

//}

/* LLCP_MeasureFrameReqMsg_t //{ */

/* LLCP_MeasureFrameReq_t //{ */

void hton_MeasureFrameReq_t(LLCP_MeasureFrameReq_t* data) {

  data->acquisition_time_ms = llcp_hton16((uint16_t)data->acquisition_time_ms);
}

void ntoh_MeasureFrameReq_t(LLCP_MeasureFrameReq_t* data) {

  data->acquisition_time_ms = llcp_ntoh16((uint16_t)data->acquisition_time_ms);
}

void init_LLCP_MeasureFrameReq_t(LLCP_MeasureFrameReq_t* data) {

  data->acquisition_time_ms = 0;
}

//}

void hton_LLCP_MeasureFrameReqMsg_t(LLCP_MeasureFrameReqMsg_t* msg) {

  hton_MeasureFrameReq_t(&msg->payload);
}

void ntoh_LLCP_MeasureFrameReqMsg_t(LLCP_MeasureFrameReqMsg_t* msg) {

  ntoh_MeasureFrameReq_t(&msg->payload);
}

void init_LLCP_MeasureFrameReqMsg_t(LLCP_MeasureFrameReqMsg_t* msg) {

  msg->message_id = LLCP_MEASURE_FRAME_REQ_MSG_ID;

  init_LLCP_MeasureFrameReq_t(&msg->payload);
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

void init_LLCP_StreamData_t(LLCP_StreamData_t* data) {

  data->full_buffer = 0;
  data->n_pixels    = 0;

  for (uint8_t i = 0; i < LLCP_STREAM_DATA_N_PIXELS; i++) {
    init_LLCP_PixelData_t(&data->pixel_data[i]);
  }
}

//}

void hton_LLCP_StreamDataMsg_t(LLCP_StreamDataMsg_t* msg) {

  hton_LLCP_StreamData_t(&msg->payload);
}

void ntoh_LLCP_StreamDataMsg_t(LLCP_StreamDataMsg_t* msg) {

  ntoh_LLCP_StreamData_t(&msg->payload);
}

void init_LLCP_StreamDataMsg_t(LLCP_StreamDataMsg_t* msg) {

  msg->message_id = LLCP_STREAM_DATA_MSG_ID;

  init_LLCP_StreamData_t(&msg->payload);
}

//}

/* LLCP_MeasureStreamReqMsg_t //{ */

/* LLCP_MeasureStreamReq_t //{ */

void hton_LLCP_MeasureStreamReq_t(LLCP_MeasureStreamReq_t* data) {

  data->duty_cycle_ms = llcp_hton16(data->duty_cycle_ms);
}

void ntoh_LLCP_MeasureStreamReq_t(LLCP_MeasureStreamReq_t* data) {

  data->duty_cycle_ms = llcp_ntoh16(data->duty_cycle_ms);
}

void init_LLCP_MeasureStreamReq_t(LLCP_MeasureStreamReq_t* data) {

  data->duty_cycle_ms = 0;
}

//}

void hton_LLCP_MeasureStreamReqMsg_t(LLCP_MeasureStreamReqMsg_t* msg) {

  hton_LLCP_MeasureStreamReq_t(&msg->payload);
}

void ntoh_LLCP_MeasureStreamReqMsg_t(LLCP_MeasureStreamReqMsg_t* msg) {

  ntoh_LLCP_MeasureStreamReq_t(&msg->payload);
}

void init_LLCP_MeasureStreamReqMsg_t(LLCP_MeasureStreamReqMsg_t* msg) {

  msg->message_id = LLCP_MEASURE_STREAM_REQ_MSG_ID;

  init_LLCP_MeasureStreamReq_t(&msg->payload);
}

//}

/* LLCP_FlushBufferReqMsg_t //{ */

void hton_LLCP_FlushBufferReqMsg_t(LLCP_FlushBufferReqMsg_t* msg) {

  UNUSED(msg);
}

void ntoh_LLCP_FlushBufferReqMsg_t(LLCP_FlushBufferReqMsg_t* msg) {

  UNUSED(msg);
}

void init_LLCP_FlushBufferReqMsg_t(LLCP_FlushBufferReqMsg_t* msg) {

  msg->message_id = LLCP_FLUSH_BUFFER_REQ_MSG_ID;
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

void init_LLCP_UpdatePixelMaskReq_t(LLCP_UpdatePixelMaskReq_t* data) {

  data->x_coordinate = 0;
  data->y_coordinate = 0;
  data->masked       = 0;
}

//}

void hton_LLCP_UpdatePixelMaskReqMsg_t(LLCP_UpdatePixelMaskReqMsg_t* data) {

  hton_LLCP_UpdatePixelMaskReq_t(&data->payload);
}

void ntoh_LLCP_UpdatePixelMaskReqMsg_t(LLCP_UpdatePixelMaskReqMsg_t* data) {

  ntoh_LLCP_UpdatePixelMaskReq_t(&data->payload);
}

void init_LLCP_UpdatePixelMaskReqMsg_t(LLCP_UpdatePixelMaskReqMsg_t* msg) {

  msg->message_id = LLCP_UPDATE_PIXEL_MASK_REQ_MSG_ID;

  init_LLCP_UpdatePixelMaskReq_t(&msg->payload);
}

//}

/* LLCP_SetThresholdReqMsg_t //{ */

/* LLCP_SetThresholdReq_t //{ */

void hton_LLCP_SetThresholdReq_t(LLCP_SetThresholdReq_t* data) {

  data->threshold = llcp_hton16(data->threshold);
}

void ntoh_LLCP_SetThresholdReq_t(LLCP_SetThresholdReq_t* data) {

  data->threshold = llcp_ntoh16(data->threshold);
}

void init_LLCP_SetThresholdReq_t(LLCP_SetThresholdReq_t* data) {

  data->threshold = 0;
}

//}

void hton_LLCP_SetThresholdReqMsg_t(LLCP_SetThresholdReqMsg_t* msg) {

  hton_LLCP_SetThresholdReq_t(&msg->payload);
}

void ntoh_LLCP_SetThresholdReqMsg_t(LLCP_SetThresholdReqMsg_t* msg) {

  ntoh_LLCP_SetThresholdReq_t(&msg->payload);
}

void init_LLCP_SetThresholdReqMsg_t(LLCP_SetThresholdReqMsg_t* msg) {

  msg->message_id = LLCP_SET_THRESHOLD_REQ_MSG_ID;

  init_LLCP_SetThresholdReq_t(&msg->payload);
}

//}

/* LLCP_SetConfigurationPresetReqMsg_t //{ */

/* LLCP_SetConfigurationPresetReq_t //{ */

void hton_LLCP_SetConfigurationPresetReq_t(LLCP_SetConfigurationPresetReq_t* data) {

  UNUSED(data);
}

void ntoh_LLCP_SetConfigurationPresetReq_t(LLCP_SetConfigurationPresetReq_t* data) {

  UNUSED(data);
}

void init_LLCP_SetConfigurationPresetReq_t(LLCP_SetConfigurationPresetReq_t* data) {

  data->preset = 0;
}

//}

void hton_LLCP_SetConfigurationPresetReqMsg_t(LLCP_SetConfigurationPresetReqMsg_t* msg) {

  hton_LLCP_SetConfigurationPresetReq_t(&msg->payload);
}

void ntoh_LLCP_SetConfigurationPresetReqMsg_t(LLCP_SetConfigurationPresetReqMsg_t* msg) {

  ntoh_LLCP_SetConfigurationPresetReq_t(&msg->payload);
}

void init_LLCP_SetConfigurationPresetReqMsg_t(LLCP_SetConfigurationPresetReqMsg_t* msg) {

  msg->message_id = LLCP_SET_CONFIGURATION_PRESET_REQ_MSG_ID;

  init_LLCP_SetConfigurationPresetReq_t(&msg->payload);
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

void init_Ack_t(LLCP_Ack_t* data) {

  data->success = 1;
}

//}

void hton_LLCP_AckMsg_t(LLCP_AckMsg_t* msg) {

  hton_LLCP_Ack_t(&msg->payload);
}

void ntoh_LLCP_AckMsg_t(LLCP_AckMsg_t* msg) {

  ntoh_LLCP_Ack_t(&msg->payload);
}

void init_LLCP_AckMsg_t(LLCP_AckMsg_t* msg) {

  msg->message_id = LLCP_ACK_MSG_ID;

  init_Ack_t(&msg->payload);
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

void init_LLCP_Status_t(LLCP_Status_t* data) {

  data->boot_count = 0;

  memset(&data->status_str, 0, LLCP_STATUS_STR_LEN);
}

//}

void hton_LLCP_StatusMsg_t(LLCP_StatusMsg_t* msg) {

  hton_LLCP_Status_t(&msg->payload);
}

void ntoh_LLCP_StatusMsg_t(LLCP_StatusMsg_t* msg) {

  ntoh_LLCP_Status_t(&msg->payload);
}

void init_LLCP_StatusMsg_t(LLCP_StatusMsg_t* msg) {

  msg->message_id = LLCP_STATUS_MSG_ID;

  init_LLCP_Status_t(&msg->payload);
}

//}

/* LLCP_GetStatusReqMsg_t //{ */

void hton_LLCP_GetStatusReqMsg_t(LLCP_GetStatusReqMsg_t* msg) {

  UNUSED(msg);
}

void ntoh_LLCP_GetStatusReqMsg_t(LLCP_GetStatusReqMsg_t* msg) {

  UNUSED(msg);
}

void init_LLCP_GetStatusReqMsg_t(LLCP_GetStatusReqMsg_t* msg) {

  msg->message_id = LLCP_GET_STATUS_REQ_MSG_ID;
}

//}

/* LLCP_PwrReqMsg_t //{ */

/* LLCP_PwrReq_t //{ */

void hton_PwrReq_t(LLCP_PwrReq_t* data) {

  UNUSED(data);
}

void ntoh_PwrReq_t(LLCP_PwrReq_t* data) {

  UNUSED(data);
}

void init_LLCP_PwrReq_t(LLCP_PwrReq_t* data) {

  data->state = 0;
}

//}

void hton_LLCP_PwrReqMsg_t(LLCP_PwrReqMsg_t* msg) {

  hton_PwrReq_t(&msg->payload);
}

void ntoh_LLCP_PwrReqMsg_t(LLCP_PwrReqMsg_t* msg) {

  ntoh_PwrReq_t(&msg->payload);
}

void init_LLCP_PwrReqMsg_t(LLCP_PwrReqMsg_t* msg) {

  msg->message_id = LLCP_PWR_REQ_MSG_ID;

  init_LLCP_PwrReq_t(&msg->payload);
}

//}
