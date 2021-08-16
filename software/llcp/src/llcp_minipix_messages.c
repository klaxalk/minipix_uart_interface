#include <llcp_minipix_messages.h>

#define UNUSED(x) (void)(x)

/* LLCP_MinipixErrors //{ */

const char* LLCP_MinipixErrors[LLCP_MINIPIX_ERROR_COUNT] = {"MiniPIX: Frame measurement failed.",       "MiniPIX: Powerup failed.",
                                                            "MiniPIX: Powerup TPX3 reset sync error.",  "MiniPIX: Powerup TPX3 reset recv data error.",
                                                            "MiniPIX: Powerup TPX3 init resets error.", "MiniPIX: Powerup TPX3 init chip ID error.",
                                                            "MiniPIX: Powerup TPX3 init DACs error.",   "MiniPIX: Powerup TPX3 init PixCfg error.",
                                                            "MiniPIX: Powerup TPX3 init matrix error.", "MiniPIX: Invalid preset parameter."};

//}

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

  data->frame_id         = 0;
  data->packet_id        = 0;
  data->mode             = 0;
  data->checksum_matched = 0;

  data->n_pixels = 0;

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
  // data->mode = data->mode;
}

void ntoh_MeasureFrameReq_t(LLCP_MeasureFrameReq_t* data) {

  data->acquisition_time_ms = llcp_ntoh16((uint16_t)data->acquisition_time_ms);
  // data->mode = data->mode;
}

void init_LLCP_MeasureFrameReq_t(LLCP_MeasureFrameReq_t* data) {

  data->acquisition_time_ms = 0;
  data->mode                = 0;
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

/* LLCP_FrameMeasurementFinishedMsg_t //{ */

void hton_LLCP_FrameMeasurementFinishedMsg_t(LLCP_FrameMeasurementFinishedMsg_t* msg) {

  UNUSED(msg);
}

void ntoh_LLCP_FrameMeasurementFinishedMsg_t(LLCP_FrameMeasurementFinishedMsg_t* msg) {

  UNUSED(msg);
}

void init_LLCP_FrameMeasurementFinishedMsg_t(LLCP_FrameMeasurementFinishedMsg_t* msg) {

  msg->message_id = LLCP_FRAME_MEASUREMENT_FINISHED_MSG_ID;
}

//}

/* LLCP_GetFrameDataReqMsg_t //{ */

void hton_LLCP_GetFrameDataReqMsg_t(LLCP_GetFrameDataReqMsg_t* msg) {

  UNUSED(msg);
}

void ntoh_LLCP_GetFrameDataReqMsg_t(LLCP_GetFrameDataReqMsg_t* msg) {

  UNUSED(msg);
}

void init_LLCP_GetFrameDataReqMsg_t(LLCP_GetFrameDataReqMsg_t* msg) {

  msg->message_id = LLCP_GET_FRAME_DATA_REQ_MSG_ID;
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

  data->threshold_fine   = llcp_hton16(data->threshold_fine);
  data->threshold_coarse = llcp_hton16(data->threshold_coarse);
}

void ntoh_LLCP_SetThresholdReq_t(LLCP_SetThresholdReq_t* data) {

  data->threshold_fine   = llcp_ntoh16(data->threshold_fine);
  data->threshold_coarse = llcp_ntoh16(data->threshold_coarse);
}

void init_LLCP_SetThresholdReq_t(LLCP_SetThresholdReq_t* data) {

  data->threshold_fine   = 0;
  data->threshold_coarse = 0;
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

/* LLCP_TemperatureMsg_t //{ */

/* LLCP_Temperature_t //{ */

void hton_LLCP_Temperature_t(LLCP_Temperature_t* data) {

  data->temperature = llcp_hton16(data->temperature);
}

void ntoh_LLCP_Temperature_t(LLCP_Temperature_t* data) {

  data->temperature = llcp_ntoh16(data->temperature);
}

void init_LLCP_Temperature_t(LLCP_Temperature_t* data) {

  data->temperature = 0;
}

//}

void hton_LLCP_TemperatureMsg_t(LLCP_TemperatureMsg_t* msg) {

  hton_LLCP_Temperature_t(&msg->payload);
}

void ntoh_LLCP_TemperatureMsg_t(LLCP_TemperatureMsg_t* msg) {

  ntoh_LLCP_Temperature_t(&msg->payload);
}

void init_LLCP_TemperatureMsg_t(LLCP_TemperatureMsg_t* msg) {

  msg->message_id = LLCP_TEMPERATURE_MSG_ID;

  init_LLCP_Temperature_t(&msg->payload);
}

//}

/* LLCP_MinipixErrorMsg_t //{ */

/* LLCP_MinipixError_t //{ */

void hton_LLCP_MinipixError_t(LLCP_MinipixError_t* data) {

  UNUSED(data);
}

void ntoh_LLCP_MinipixError_t(LLCP_MinipixError_t* data) {

  UNUSED(data);
}

void init_LLCP_MinipixError_t(LLCP_MinipixError_t* data) {

  data->error_id = 0;
}

//}

void hton_LLCP_MinipixErrorMsg_t(LLCP_MinipixErrorMsg_t* msg) {

  hton_LLCP_MinipixError_t(&msg->payload);
}

void ntoh_LLCP_MinipixErrorMsg_t(LLCP_MinipixErrorMsg_t* msg) {

  ntoh_LLCP_MinipixError_t(&msg->payload);
}

void init_LLCP_MinipixErrorMsg_t(LLCP_MinipixErrorMsg_t* msg) {

  msg->message_id = LLCP_MINIPIX_ERROR_MSG_ID;

  init_LLCP_MinipixError_t(&msg->payload);
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

/* LLCP_GetTemperatureReqMsg_t //{ */

void hton_LLCP_GetTemperatureReqMsg_t(LLCP_GetTemperatureReqMsg_t* msg) {

  UNUSED(msg);
}

void ntoh_LLCP_GetTemperatureReqMsg_t(LLCP_GetTemperatureReqMsg_t* msg) {

  UNUSED(msg);
}

void init_LLCP_GetTemperatureReqMsg_t(LLCP_GetTemperatureReqMsg_t* msg) {

  msg->message_id = LLCP_GET_TEMPERATURE_REQ_MSG_ID;
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
