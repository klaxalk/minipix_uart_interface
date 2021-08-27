#include <gatherer.h>

/* constructor Gatherer() //{ */

Gatherer::Gatherer(const std::string data_path) {

  // | --------------- initialize the LLCP handler -------------- |

  llcp_initialize(&llcp_receiver);

  // | ----------- prepare OpenCV windows for plotting ---------- |
#if GUI == 1
  /* int flags = cv::WINDOW_NORMAL | cv::WINDOW_FREERATIO | cv::WINDOW_GUI_EXPANDED; */
  int flags = cv::WINDOW_NORMAL;
  cv::namedWindow("gatherer_gui", flags);

  frame_top_left  = cv::Mat(256, 256, CV_32FC3);
  frame_top_right = cv::Mat(256, 256, CV_32FC3);
  frame_bot_left  = cv::Mat(256, 256, CV_32FC3);
  frame_bot_right = cv::Mat(256, 256, CV_32FC3);
#endif

  // | ---------------------- start threads --------------------- |

#if GUI == 1
  thread_plot_ = std::thread(&Gatherer::threadPlot, this);
#endif

  thread_main_ = std::thread(&Gatherer::threadMain, this);

  // | ------------ open the file for saving the data ----------- |

  measured_data_file_ = fopen(data_path.c_str(), "w");

  if (measured_data_file_ == NULL) {
    printf("Error: cannot open the data output file '%s' for writing!\n", data_path.c_str());
  }

  // | ---------------- finish the initialization --------------- |

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

#if MUI_USER_HANDSHAKES == 1
              sendAck(true);
#endif

              break;
            };

            case LLCP_STATUS_MSG_ID: {

              callbackStatus(message_in);

#if MUI_USER_HANDSHAKES == 1
              sendAck(true);
#endif

              break;
            };

            case LLCP_TEMPERATURE_MSG_ID: {

              callbackTemperature(message_in);

#if MUI_USER_HANDSHAKES == 1
              sendAck(true);
#endif

              break;
            };

            case LLCP_FRAME_DATA_TERMINATOR_MSG_ID: {

              callbackFrameTerminator(message_in);

#if MUI_USER_HANDSHAKES == 1
              sendAck(true);
#endif

              break;
            };

            case LLCP_ACK_MSG_ID: {

              callbackAck(message_in);

              break;
            };

            case LLCP_FRAME_MEASUREMENT_FINISHED_MSG_ID: {

              callbackFrameMeasurementFinished(message_in);

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

#if GUI == 1
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

    showManyImages<CV_32FC3>("gatherer_gui", images);

    cv::waitKey(30);
  }
}

//}
#endif

/* saveFrameDataToFile() //{ */

void Gatherer::saveFrameDataToFile(const LLCP_FrameDataMsg_t& msg) {

  // max llcp message size * 2
  uint8_t out_buffer[3];
  memset(out_buffer, 0, 3);

  // fill in the out buffer with the message in HEX form
  for (size_t i = 0; i < sizeof(LLCP_FrameDataMsg_t); i++) {

    bin2hex(*(((uint8_t*)&msg) + i), out_buffer);

    fprintf(measured_data_file_, "%s", out_buffer);
  }

  fprintf(measured_data_file_, "\n");

  // probably not neccessary, but to be sure...
  fflush(measured_data_file_);
}

//}

// | ------- Callbacks for processing data from MiniPIX ------- |

/* callbackFrameData() //{ */

void Gatherer::callbackFrameData(const LLCP_Message_t* message_in) {

  LLCP_FrameDataMsg_t* msg = (LLCP_FrameDataMsg_t*)message_in;
  ntoh_LLCP_FrameDataMsg_t(msg);

  saveFrameDataToFile(*msg);

  LLCP_FrameData_t* image = (LLCP_FrameData_t*)&msg->payload;

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
      printf("pixel mode mode is wrong, %d!\n", image->mode);

      break;
    }
  }

  printf("received frame data, id %d, packet %d, n_pixels %d\n", image->frame_id, image->packet_id, image->n_pixels);

#if GUI == 1
  /* Plotting //{ */

  // we received the first packet in an image, so clear the old image data
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

  // for all the pixels in the packet
  for (int pix = 0; pix < image->n_pixels; pix++) {

    // derandomize and deserialize the pixel data
    decodePixelData((uint8_t*)&image->pixel_data[pix], 4, TPX3_TOA_TOT);

    uint8_t  x, y;
    uint16_t tot, toa, ftoa, mpx, itot;

    uint16_t value1, value2, value3;

    // decode the pixel values based on the measurement mode
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

    // put the pixel values into the OpenCV images
    {
      cv::Vec3f value1_color(0, 0, 0);  // BGR
      if (value1 > 0) {
        value1_color.val[2] = value1;
      }

      cv::Vec3f value2_color(0, 0, 0);  // BGR
      if (value2 > 0) {
        value2_color.val[1] = value2;
      }

      cv::Vec3f value3_color(0, 0, 0);  // BGR
      if (value3 > 0) {
        value3_color.val[0] = value3;
      }

      {
        std::scoped_lock lock(mutex_cv_frames_);

        frame_top_left.at<cv::Vec3f>(cv::Point(x, y))  = value1_color;
        frame_top_right.at<cv::Vec3f>(cv::Point(x, y)) = value2_color;
        frame_bot_left.at<cv::Vec3f>(cv::Point(x, y))  = value3_color;
      }
    }
  }

  //}
#endif
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

/* callbackFrameMeasurementFinished() //{ */

void Gatherer::callbackFrameMeasurementFinished([[maybe_unused]] const LLCP_Message_t* message_in) {

  printf("frame measurement finished\n");

  getFrameData();
}

//}

/* callbackError() //{ */

void Gatherer::callbackError(const LLCP_Message_t* message_in) {

  LLCP_MinipixErrorMsg_t* msg = (LLCP_MinipixErrorMsg_t*)message_in;
  ntoh_LLCP_MinipixErrorMsg_t(msg);
  LLCP_MinipixError_t* error = (LLCP_MinipixError_t*)&msg->payload;

  switch (error->error_id) {

    case LLCP_MINIPIX_ERROR_MEASUREMENT_FAILED: {

      printf("Error: '%s'\n", LLCP_MinipixErrors[LLCP_MINIPIX_ERROR_MEASUREMENT_FAILED]);

      measuring_frame_ = false;

      break;
    }

    case LLCP_MINIPIX_ERROR_POWERUP_FAILED: {

      printf("Error: '%s'\n", LLCP_MinipixErrors[LLCP_MINIPIX_ERROR_POWERUP_FAILED]);

      break;
    }

    case LLCP_MINIPIX_ERROR_POWERUP_TPX3_RESET_SYNC: {

      printf("Error: '%s'\n", LLCP_MinipixErrors[LLCP_MINIPIX_ERROR_POWERUP_TPX3_RESET_SYNC]);

      break;
    }

    case LLCP_MINIPIX_ERROR_POWERUP_TPX3_RESET_RECVDATA: {

      printf("Error: '%s'\n", LLCP_MinipixErrors[LLCP_MINIPIX_ERROR_POWERUP_TPX3_RESET_RECVDATA]);

      break;
    }

    case LLCP_MINIPIX_ERROR_POWERUP_TPX3_INIT_RESETS: {

      printf("Error: '%s'\n", LLCP_MinipixErrors[LLCP_MINIPIX_ERROR_POWERUP_TPX3_INIT_RESETS]);

      break;
    }

    case LLCP_MINIPIX_ERROR_POWERUP_TPX3_INIT_CHIPID: {

      printf("Error: '%s'\n", LLCP_MinipixErrors[LLCP_MINIPIX_ERROR_POWERUP_TPX3_INIT_CHIPID]);

      break;
    }

    case LLCP_MINIPIX_ERROR_POWERUP_TPX3_INIT_DACS: {

      printf("Error: '%s'\n", LLCP_MinipixErrors[LLCP_MINIPIX_ERROR_POWERUP_TPX3_INIT_DACS]);

      break;
    }

    case LLCP_MINIPIX_ERROR_POWERUP_TPX3_INIT_PIXCFG: {

      printf("Error: '%s'\n", LLCP_MinipixErrors[LLCP_MINIPIX_ERROR_POWERUP_TPX3_INIT_PIXCFG]);

      break;
    }

    case LLCP_MINIPIX_ERROR_POWERUP_TPX3_INIT_MATRIX: {

      printf("Error: '%s'\n", LLCP_MinipixErrors[LLCP_MINIPIX_ERROR_POWERUP_TPX3_INIT_MATRIX]);

      break;
    }

    case LLCP_MINIPIX_ERROR_INVALID_PRESET: {

      printf("Error: '%s'\n", LLCP_MinipixErrors[LLCP_MINIPIX_ERROR_INVALID_PRESET]);

      break;
    }

    default: {
      printf("Error: received unhandled error message, id %d\n", error->error_id);
    }
  }
}

//}

// | ----------------- MiniPIX control methods ---------------- |

/* getFrameData() //{ */

void Gatherer::getFrameData(void) {

  // create the message
  LLCP_GetFrameDataReqMsg_t msg;
  init_LLCP_GetFrameDataReqMsg_t(&msg);

  // convert to network endian
  hton_LLCP_GetFrameDataReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), tx_buffer);

  {
    std::scoped_lock lock(mutex_serial_port_);

    serial_port_.sendCharArray(tx_buffer, n_bytes);
  }
}

//}

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

void Gatherer::measureFrame(const uint16_t& acquisition_time_ms, const uint8_t& mode) {

  // create the message
  LLCP_MeasureFrameReqMsg_t msg;
  init_LLCP_MeasureFrameReqMsg_t(&msg);

  // fill in the payload
  msg.payload.acquisition_time_ms = acquisition_time_ms;
  msg.payload.mode                = mode;

  // convert to network endian
  hton_LLCP_MeasureFrameReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), tx_buffer);

  measuring_frame_ = true;

  {
    std::scoped_lock lock(mutex_serial_port_);

    serial_port_.sendCharArray(tx_buffer, n_bytes);
  }

  while (bool(measuring_frame_)) {

    /* printf("data readout in progress: %d\n", bool(measuring_frame_)); */
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
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

  waiting_for_ack_ = true;

  while (waiting_for_ack_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
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

  waiting_for_ack_ = true;

  while (waiting_for_ack_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
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

  waiting_for_ack_ = true;

  while (waiting_for_ack_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

//}

// | --------------------- Helper routines -------------------- |

/* bin2hex() //{ */

void Gatherer::bin2hex(const uint8_t x, uint8_t* buffer) {

  if (x >= 16) {
    *buffer       = "0123456789ABCDEF"[x / 16];
    *(buffer + 1) = "0123456789ABCDEF"[x & 15];
  } else {
    *buffer       = '0';
    *(buffer + 1) = "0123456789ABCDEF"[x];
  }
}

//}
