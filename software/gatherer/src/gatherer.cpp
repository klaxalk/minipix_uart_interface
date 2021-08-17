#include <gatherer.h>

/* constructor Gatherer() //{ */

Gatherer::Gatherer() {

  llcp_initialize(&llcp_receiver);

  /* int flags = cv::WINDOW_NORMAL | cv::WINDOW_FREERATIO | cv::WINDOW_GUI_EXPANDED; */
  int flags = cv::WINDOW_NORMAL;
  cv::namedWindow("frame", flags);

  frame_top_left  = cv::Mat(256, 256, CV_32FC3);
  frame_top_right = cv::Mat(256, 256, CV_32FC3);
  frame_bot_left  = cv::Mat(256, 256, CV_32FC3);
  frame_bot_right = cv::Mat(256, 256, CV_32FC3);

  thread_plot_ = std::thread(&Gatherer::threadPlot, this);
  thread_main_ = std::thread(&Gatherer::threadMain, this);

  initialized_ = true;
}

//}

/* connect() //{ */

void Gatherer::connect(const std::string& serial_port, const int& baud_rate, const bool& virtual_port) {

  std::scoped_lock lock(mutex_serial_port_);

  serial_port_.connect(serial_port, baud_rate, virtual_port);
}

//}

/* threadMain() //{ */

void Gatherer::threadMain(void) {

  uint8_t  rx_buffer[SERIAL_BUFFER_SIZE];
  uint16_t bytes_read;

  while (!initialized_) {
    printf("threadMain() waiting for initialization\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  {
    std::scoped_lock lock(mutex_serial_port_);

    while (!serial_port_.checkConnected()) {
      printf("threadMain() waiting for serial line\n");
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }

  printf("threadMain started\n");

  while (true) {

    {
      std::scoped_lock lock(mutex_serial_port_);

      bytes_read = serial_port_.readSerial(rx_buffer, SERIAL_BUFFER_SIZE);
    }

    if (bytes_read > 0) {

      // feed all the incoming bytes into the MiniPIX interface
      for (uint16_t i = 0; i < bytes_read; i++) {

        LLCP_Message_t* message_in;

        bool checksum_matched;

        if (llcp_processChar(rx_buffer[i], &llcp_receiver, &message_in, &checksum_matched)) {

          switch (message_in->id) {

            case LLCP_FRAME_DATA_MSG_ID: {

              callbackFrameData(message_in);

              // TODO: now brakes MiniPIX
              /* sendAck(true); */

              break;
            };

            case LLCP_STATUS_MSG_ID: {

              callbackStatus(message_in);

              // TODO: brakes MiniPIX
              /* sendAck(true); */

              break;
            };

            case LLCP_TEMPERATURE_MSG_ID: {

              callbackTemperature(message_in);

              // TODO: brakes MiniPIX
              /* sendAck(true); */

              break;
            };

            case LLCP_FRAME_DATA_TERMINATOR_MSG_ID: {

              callbackFrameTerminator(message_in);

              // TODO: brakes MiniPIX
              /* sendAck(true); */

              break;
            };

            case LLCP_ACK_MSG_ID: {

              callbackAck(message_in);

              break;
            };

            case LLCP_MINIPIX_ERROR_MSG_ID: {

              callbackError(message_in);

              break;
            };

            default: {

              printf("Received unsupported message with id = %d\n", message_in->id);
            }
          }
        }
      }
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }
}

//}

/* threadPlot() //{ */

void Gatherer::threadPlot(void) {

  while (!initialized_) {
    printf("threadPlot() waiting for initialization\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  printf("threadPlot started\n");

  while (true) {

    cv::Mat frame_top_left_plot(256, 256, CV_32FC3);
    cv::Mat frame_top_right_plot(256, 256, CV_32FC3);
    cv::Mat frame_bot_left_plot(256, 256, CV_32FC3);
    cv::Mat frame_bot_right_plot(256, 256, CV_32FC3);

    {
      std::scoped_lock lock(mutex_cv_frames_);

      cv::normalize(frame_top_left, frame_top_left_plot, 0.05, 1.0, cv::NORM_MINMAX);
      cv::normalize(frame_top_right, frame_top_right_plot, 0.05, 1.0, cv::NORM_MINMAX);
      cv::normalize(frame_bot_left, frame_bot_left_plot, 0.05, 1.0, cv::NORM_MINMAX);
      cv::normalize(frame_bot_right, frame_bot_right_plot, 0.05, 1.0, cv::NORM_MINMAX);
    }

    /* cv::subtract(cv::Scalar::all(1.0), frame_top_plot, frame_top_plot); */
    /* cv::subtract(cv::Scalar::all(1.0), frame_bot_plot, frame_bot_plot); */

    std::vector<cv::Mat> images;
    images.push_back(frame_top_left_plot);
    images.push_back(frame_top_right_plot);
    images.push_back(frame_bot_left_plot);
    images.push_back(frame_bot_right_plot);

    showManyImages<CV_32FC3>("frame", images);

    cv::waitKey(30);
  }
}

//}

// | ------- Callbacks for processing data from MiniPIX ------- |

/* callbackFrameData() //{ */

void Gatherer::callbackFrameData(const LLCP_Message_t* message_in) {

  LLCP_FrameDataMsg_t* msg = (LLCP_FrameDataMsg_t*)message_in;
  ntoh_LLCP_FrameDataMsg_t(msg);

  LLCP_FrameData_t* image = (LLCP_FrameData_t*)&msg->payload;

  uint8_t n_pixels = image->n_pixels;

  {  // FOR PLOTTING
    // clear the old images
    if (image->packet_id == 0) {

      std::scoped_lock lock(mutex_cv_frames_);

      for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
          frame_top_left.at<cv::Vec3f>(cv::Point(i, j))  = 0;
          frame_top_right.at<cv::Vec3f>(cv::Point(i, j)) = 0;
          frame_bot_left.at<cv::Vec3f>(cv::Point(i, j))  = 0;
          frame_bot_right.at<cv::Vec3f>(cv::Point(i, j)) = 0;
        }
      }
    }
  }

  std::string mode_str;
  switch (image->mode) {

    case LLCP_TPX3_PXL_MODE_TOA_TOT: {
      mode_str = "TOA_TOT";
      break;
    }

    case LLCP_TPX3_PXL_MODE_TOA: {
      mode_str = "TOA";
      break;
    }

    case LLCP_TPX3_PXL_MODE_MPX_ITOT: {
      mode_str = "MPX_ITOT";
      break;
    }

    default: {
      mode_str = "UNKNOWN";
      printf("pixel mode mode undefined, %d!\n", image->mode);

      // TODO: this is here because MiniPIX does not fill this properly
      image->mode = LLCP_TPX3_PXL_MODE_TOA_TOT;

      break;
    }
  }

  printf("received frame data, id %d, packet %d, n_pixels %d\n", image->frame_id, image->packet_id, n_pixels);

  // for all the pixels in the packet
  for (int pix = 0; pix < n_pixels; pix++) {

    // derandomize and deserialize the pixel data
    decodePixelData((uint8_t*)&image->pixel_data[pix], 4);

    std::scoped_lock lock(mutex_cv_frames_);

    uint8_t  x, y;
    uint16_t tot, toa, ftoa, mpx, itot;

    // FOR PLOTTING
    uint16_t value1, value2, value3;

    switch (image->mode) {

      case LLCP_TPX3_PXL_MODE_TOA_TOT: {

        x = ((LLCP_PixelDataToAToT_t*)&image->pixel_data[pix])->address % 256;
        y = ((LLCP_PixelDataToAToT_t*)&image->pixel_data[pix])->address / 256;

        ftoa = float(((LLCP_PixelDataToAToT_t*)&image->pixel_data[pix])->ftoa);
        tot  = float(((LLCP_PixelDataToAToT_t*)&image->pixel_data[pix])->tot);
        toa  = float(((LLCP_PixelDataToAToT_t*)&image->pixel_data[pix])->toa);

        mpx  = 0;
        itot = 0;

        // FOR PLOTTING
        {
          value1 = toa;
          value2 = tot;
          value3 = ftoa;
        }

        break;
      }

      case LLCP_TPX3_PXL_MODE_TOA: {

        x = ((LLCP_PixelDataToA_t*)&image->pixel_data[pix])->address % 256;
        y = ((LLCP_PixelDataToA_t*)&image->pixel_data[pix])->address / 256;

        ftoa = float(((LLCP_PixelDataToA_t*)&image->pixel_data[pix])->ftoa);
        toa  = float(((LLCP_PixelDataToA_t*)&image->pixel_data[pix])->toa);

        tot  = 0;
        mpx  = 0;
        itot = 0;

        // FOR PLOTTING
        {
          value1 = toa;
          value2 = ftoa;
          value3 = 0;
        }

        break;
      }

      case LLCP_TPX3_PXL_MODE_MPX_ITOT: {

        x = ((LLCP_PixelDataMpxiToT_t*)&image->pixel_data[pix])->address % 256;
        y = ((LLCP_PixelDataMpxiToT_t*)&image->pixel_data[pix])->address / 256;

        mpx  = ((LLCP_PixelDataMpxiToT_t*)&image->pixel_data[pix])->event_counter;
        itot = float(((LLCP_PixelDataMpxiToT_t*)&image->pixel_data[pix])->itot);

        toa  = 0;
        ftoa = 0;
        tot  = 0;

        // FOR PLOTTING
        {
          value1 = mpx;
          value2 = itot;
          value3 = 0;
        }

        break;
      }
    }

    // FOR PLOTTING
    {
      cv::Vec3f value1_color(0, 0, 0);  // BGR
      if (value1 > 0) {
        value1_color.val[2] = log2(value1);
      }

      cv::Vec3f value2_color(0, 0, 0);  // BGR
      if (value2 > 0) {
        value2_color.val[1] = log2(value2);
      }

      cv::Vec3f value3_color(0, 0, 0);  // BGR
      if (value3 > 0) {
        value3_color.val[0] = log2(value3);
      }

      frame_top_left.at<cv::Vec3f>(cv::Point(x, y))  = value1_color;
      frame_top_right.at<cv::Vec3f>(cv::Point(x, y)) = value2_color;
      frame_bot_left.at<cv::Vec3f>(cv::Point(x, y))  = value3_color;
    }
  }
}

//}

/* callbackFrameTerminator() //{ */

void Gatherer::callbackFrameTerminator(const LLCP_Message_t* message_in) {

  LLCP_FrameDataTerminatorMsg_t* msg = (LLCP_FrameDataTerminatorMsg_t*)message_in;
  ntoh_LLCP_FrameDataTerminatorMsg_t(msg);
  LLCP_FrameDataTerminator_t* terminator = (LLCP_FrameDataTerminator_t*)&msg->payload;

  measuring_frame_ = false;

  printf("received frame data terminator: frame id %d, packet count: %d\n", terminator->frame_id, terminator->n_packets);
}

//}

/* callbackStatus() //{ */

void Gatherer::callbackStatus(const LLCP_Message_t* message_in) {

  LLCP_StatusMsg_t* msg = (LLCP_StatusMsg_t*)message_in;
  ntoh_LLCP_StatusMsg_t(msg);
  LLCP_Status_t* status = (LLCP_Status_t*)&msg->payload;

  printf("received status: boot count = %d, string: '%s'\n", status->boot_count, status->status_str);

  waiting_for_status_ = false;
}

//}

/* callbackTemperature() //{ */

void Gatherer::callbackTemperature(const LLCP_Message_t* message_in) {

  LLCP_TemperatureMsg_t* msg = (LLCP_TemperatureMsg_t*)message_in;
  ntoh_LLCP_TemperatureMsg_t(msg);
  LLCP_Temperature_t* temperature = (LLCP_Temperature_t*)&msg->payload;

  printf("received temperature: %d deg\n", temperature->temperature);

  waiting_for_tmp_ = false;
}

//}

/* callbackAck() //{ */

void Gatherer::callbackAck(const LLCP_Message_t* message_in) {

  LLCP_AckMsg_t* msg = (LLCP_AckMsg_t*)message_in;
  ntoh_LLCP_AckMsg_t(msg);
  LLCP_Ack_t* ack = (LLCP_Ack_t*)&msg->payload;

  printf("received ack: %s\n", ack->success ? "true" : "false");

  waiting_for_ack_ = false;
}

//}

/* callbackError() //{ */

void Gatherer::callbackError(const LLCP_Message_t* message_in) {

  LLCP_MinipixErrorMsg_t* msg = (LLCP_MinipixErrorMsg_t*)message_in;
  ntoh_LLCP_MinipixErrorMsg_t(msg);
  LLCP_MinipixError_t* error = (LLCP_MinipixError_t*)&msg->payload;

  switch (error->error_id) {

    case LLCP_MINIPIX_ERROR_MEASUREMENT_FAILED: {

      printf("error measuring frame\n");

      measuring_frame_ = false;

      break;
    }

    default: {
      printf("received unhandled error message, id %d\n", error->error_id);
    }
  }

  printf("received MiniPIX error %d: %s\n", error->error_id, LLCP_MinipixErrors[error->error_id]);
}

//}

// | ----------------- MiniPIX control methods ---------------- |

/* sendAck() //{ */

void Gatherer::sendAck(bool ack) {

  // create the message
  LLCP_AckMsg_t msg;
  init_LLCP_AckMsg_t(&msg);

  msg.payload.success = ack;

  // convert to network endian
  hton_LLCP_AckMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), tx_buffer);

  {
    std::scoped_lock lock(mutex_serial_port_);

    serial_port_.sendCharArray(tx_buffer, n_bytes);
  }
}

//}

/* getStatus() //{ */

void Gatherer::getStatus(void) {

  // create the message
  LLCP_GetStatusReqMsg_t msg;
  init_LLCP_GetStatusReqMsg_t(&msg);

  // convert to network endian
  hton_LLCP_GetStatusReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), tx_buffer);

  {
    std::scoped_lock lock(mutex_serial_port_);

    serial_port_.sendCharArray(tx_buffer, n_bytes);
  }

  waiting_for_status_ = true;

  while (waiting_for_status_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

//}

/* getTemperature() //{ */

void Gatherer::getTemperature(void) {

  // create the message
  LLCP_GetTemperatureReqMsg_t msg;
  init_LLCP_GetTemperatureReqMsg_t(&msg);

  // convert to network endian
  hton_LLCP_GetTemperatureReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), tx_buffer);

  {
    std::scoped_lock lock(mutex_serial_port_);

    serial_port_.sendCharArray(tx_buffer, n_bytes);
  }

  waiting_for_tmp_ = true;

  while (waiting_for_tmp_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

//}

/* measureFrame() //{ */

void Gatherer::measureFrame(const uint16_t& acquisition_time_ms) {

  // create the message
  LLCP_MeasureFrameReqMsg_t msg;
  init_LLCP_MeasureFrameReqMsg_t(&msg);

  // fill in the payload
  msg.payload.acquisition_time_ms = acquisition_time_ms;

  // convert to network endian
  hton_LLCP_MeasureFrameReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), tx_buffer);

  measuring_frame_ = true;

  {
    std::scoped_lock lock(mutex_serial_port_);

    serial_port_.sendCharArray(tx_buffer, n_bytes);
  }

  while (bool(measuring_frame_)) {

    printf("data readout in progress: %d\n", bool(measuring_frame_));
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}

//}

/* pwr() //{ */

void Gatherer::pwr(const bool& state) {

  // create the message
  LLCP_PwrReqMsg_t msg;
  init_LLCP_PwrReqMsg_t(&msg);

  // fill in the payload
  msg.payload.state = state;

  // convert to network endian
  hton_LLCP_PwrReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), tx_buffer);

  {
    std::scoped_lock lock(mutex_serial_port_);

    serial_port_.sendCharArray(tx_buffer, n_bytes);
  }

  waiting_for_ack_ = true;

  while (waiting_for_ack_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

//}

/* maskPixel() //{ */

void Gatherer::maskPixel(const uint8_t& x, const uint8_t& y) {

  // create the message
  LLCP_UpdatePixelMaskReqMsg_t msg;
  init_LLCP_UpdatePixelMaskReqMsg_t(&msg);

  // fill in the payload
  msg.payload.masked       = 1;
  msg.payload.x_coordinate = x;
  msg.payload.y_coordinate = y;

  // convert to network endian
  hton_LLCP_UpdatePixelMaskReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), tx_buffer);

  {
    std::scoped_lock lock(mutex_serial_port_);

    serial_port_.sendCharArray(tx_buffer, n_bytes);
  }
}

//}

/* setThreshold() //{ */

void Gatherer::setThreshold(const uint16_t& coarse, const uint16_t& fine) {

  // create the message
  LLCP_SetThresholdReqMsg_t msg;
  init_LLCP_SetThresholdReqMsg_t(&msg);

  // fill in the payload
  msg.payload.threshold_coarse = coarse;
  msg.payload.threshold_fine   = fine;

  // convert to network endian
  hton_LLCP_SetThresholdReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), tx_buffer);

  {
    std::scoped_lock lock(mutex_serial_port_);

    serial_port_.sendCharArray(tx_buffer, n_bytes);
  }
}

//}

/* setConfigurationPreset() //{ */

void Gatherer::setConfigurationPreset(const uint16_t& preset) {

  // create the message
  LLCP_SetConfigurationPresetReqMsg_t msg;
  init_LLCP_SetConfigurationPresetReqMsg_t(&msg);

  // fill in the payload
  msg.payload.preset = preset;

  // convert to network endian
  hton_LLCP_SetConfigurationPresetReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), tx_buffer);

  {
    std::scoped_lock lock(mutex_serial_port_);

    serial_port_.sendCharArray(tx_buffer, n_bytes);
  }
}

//}
