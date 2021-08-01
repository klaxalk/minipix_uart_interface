/* includes //{ */

#define LLCP_DEBUG_PRINT 1

#include <llcp.h>
#include <llcp_minipix_messages.h>

#include <opencv2/imgproc.hpp>
#include <thread>
#include <chrono>
#include <string>
#include <mutex>

#include <atomic>

#include <opencv_helpers.hpp>

#include <math.h>

#include <serial_port.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <thread>

#include <pixel_decoder.h>

//}

#define SERIAL_BUFFER_SIZE 2048

/* class Gatherer //{ */

class Gatherer {

public:
  Gatherer();

  void connect(const std::string& serial_port, const int& baud_rate, const bool& virtual_port);

  void charCallback(const char in);

  // | --------------------- MiniPIX control -------------------- |

  void getStatus(void);
  void getTemperature(void);
  void measureFrame(const uint16_t& acquisition_time_ms);
  void startStream(const uint16_t& duty_cycle);
  void pwr(const bool& state);
  void maskPixel(const uint8_t& x, const uint8_t& y);
  void setThreshold(const uint16_t& coarse, const uint16_t& fine);
  void setConfigurationPreset(const uint16_t& preset);
  void sendAck(bool ack);

private:
  SerialPort serial_port_;
  std::mutex mutex_serial_port_;

  LLCP_Receiver_t llcp_receiver;

  uint8_t tx_buffer[SERIAL_BUFFER_SIZE];

  std::thread thread_main_;
  void        threadMain(void);

  std::thread thread_plot_;
  void        threadPlot(void);

  bool measuring_frame_;

  std::atomic<bool> initialized_ = false;

  // | ------------------------ plotting ------------------------ |

  cv::Mat    frame_top;
  cv::Mat    frame_bot;
  std::mutex mutex_cv_frames_;
};

//}

/* constructor Gatherer() //{ */

Gatherer::Gatherer() {

  llcp_initialize(&llcp_receiver);

  /* int flags = cv::WINDOW_NORMAL | cv::WINDOW_FREERATIO | cv::WINDOW_GUI_EXPANDED; */
  int flags = cv::WINDOW_NORMAL;
  cv::namedWindow("frame", flags);

  frame_top = cv::Mat(256, 256, CV_32FC3);
  frame_bot = cv::Mat(256, 256, CV_32FC3);

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

      // feed all the incoming bytes into the minipix interface
      for (uint16_t i = 0; i < bytes_read; i++) {

        LLCP_Message_t* message_in;

        bool checksum_matched;

        if (llcp_processChar(rx_buffer[i], &llcp_receiver, &message_in, &checksum_matched)) {

          switch (message_in->id) {

            case LLCP_FRAME_DATA_MSG_ID: {

              LLCP_FrameDataMsg_t* msg = (LLCP_FrameDataMsg_t*)message_in;
              ntoh_LLCP_FrameDataMsg_t(msg);

              LLCP_FrameData_t* image = (LLCP_FrameData_t*)&msg->payload;

              uint8_t n_pixels = image->n_pixels;

              // clear the old images
              if (image->packet_id == 0) {

                std::scoped_lock lock(mutex_cv_frames_);

                for (int i = 0; i < 256; i++) {
                  for (int j = 0; j < 256; j++) {
                    frame_top.at<cv::Vec3f>(cv::Point(i, j)) = 0;
                    frame_bot.at<cv::Vec3f>(cv::Point(i, j)) = 0;
                  }
                }
              }

              /* std::string mode_str; */
              /* switch (image->mode) { */

              /*   case LLCP_TPX3_PXL_MODE_TOA_TOT: { */
              /*     mode_str = "TOA_TOT"; */
              /*     break; */
              /*   } */

              /*   case LLCP_TPX3_PXL_MODE_TOA: { */
              /*     mode_str = "TOA"; */
              /*     break; */
              /*   } */

              /*   case LLCP_TPX3_PXL_MODE_MPX_ITOT: { */
              /*     mode_str = "MPX_ITOT"; */
              /*     break; */
              /*   } */

              /*   default: { */
              /*     mode_str = "UNKNOWN"; */
              /*     printf("pixel mode mode undefined, %d!\n", image->mode); */
              /*     break; */
              /*   } */
              /* } */

              /* image->mode = LLCP_TPX3_PXL_MODE_TOA_TOT; */

              printf("received frame data, id %d, packet %d, n_pixels %d\n", image->frame_id, image->packet_id, n_pixels);

              for (int pix = 0; pix < n_pixels; pix++) {

                decodePixelData((uint8_t*)&image->pixel_data[pix], 4, false);

                std::scoped_lock lock(mutex_cv_frames_);

                /* if (image->mode == LLCP_TPX3_PXL_MODE_TOA_TOT) { */

                  uint8_t x = ((LLCP_PixelDataToAToT_t*)&image->pixel_data[pix])->address % 256;
                  uint8_t y = (((LLCP_PixelDataToAToT_t*)&image->pixel_data[pix])->address - x) / 256;

                  float tot = float(((LLCP_PixelDataToAToT_t*)&image->pixel_data[pix])->tot);
                  float toa = float(((LLCP_PixelDataToAToT_t*)&image->pixel_data[pix])->toa);

                  cv::Vec3f tot_color(0, 0, tot);    // BGR
                  cv::Vec3f toa_color(0, pow(toa, 2), 0);  // BGR

                  frame_top.at<cv::Vec3f>(cv::Point(x, y)) = tot_color;
                  frame_bot.at<cv::Vec3f>(cv::Point(x, y)) = toa_color;

                /* } */

                /* if (image->mode == LLCP_TPX3_PXL_MODE_TOA) { */

                /*   uint8_t x = ((LLCP_PixelDataToA_t*)&image->pixel_data[pix])->address % 256; */
                /*   uint8_t y = (((LLCP_PixelDataToA_t*)&image->pixel_data[pix])->address - x) / 256; */

                /*   float toa = float(((LLCP_PixelDataToA_t*)&image->pixel_data[pix])->toa); */

                /*   cv::Vec3f toa_color(0, pow(toa, 2), 0);  // BGR */

                /*   frame_bot.at<cv::Vec3f>(cv::Point(x, y)) = toa_color; */
                /* } */

                /* if (image->mode == LLCP_TPX3_PXL_MODE_MPX_ITOT) { */

                /*   uint8_t x = ((LLCP_PixelDataMpxiToT_t*)&image->pixel_data[pix])->address % 256; */
                /*   uint8_t y = (((LLCP_PixelDataMpxiToT_t*)&image->pixel_data[pix])->address - x) / 256; */

                /*   float tot = float(((LLCP_PixelDataMpxiToT_t*)&image->pixel_data[pix])->itot); */
                /*   int   mpx = float(((LLCP_PixelDataMpxiToT_t*)&image->pixel_data[pix])->event_counter); */

                /*   cv::Vec3f tot_color(0, 0, log2(tot));    // BGR */
                /*   cv::Vec3f mpx_color(0, pow(mpx, 2), 0);  // BGR */

                /*   frame_top.at<cv::Vec3f>(cv::Point(x, y)) = tot_color; */
                /*   frame_bot.at<cv::Vec3f>(cv::Point(x, y)) = mpx_color; */
                /* } */
              }

              /* sendAck(true); */

              break;
            };

            case LLCP_STREAM_DATA_MSG_ID: {

              LLCP_StreamDataMsg_t* msg = (LLCP_StreamDataMsg_t*)message_in;
              ntoh_LLCP_StreamDataMsg_t(msg);

              LLCP_StreamData_t* image = (LLCP_StreamData_t*)&msg->payload;

              uint8_t n_pixels = image->n_pixels;

              printf("received stream data, n_pixels %d\n", n_pixels);

              sendAck(true);

              break;
            };

            case LLCP_STATUS_MSG_ID: {

              LLCP_StatusMsg_t* msg = (LLCP_StatusMsg_t*)message_in;
              ntoh_LLCP_StatusMsg_t(msg);
              LLCP_Status_t* status = (LLCP_Status_t*)&msg->payload;

              printf("received status: boot count = %d, string: '%s'\n", status->boot_count, status->status_str);

              sendAck(true);

              break;
            };

            case LLCP_TEMPERATURE_MSG_ID: {

              LLCP_TemperatureMsg_t* msg = (LLCP_TemperatureMsg_t*)message_in;
              ntoh_LLCP_TemperatureMsg_t(msg);
              LLCP_Temperature_t* temperature = (LLCP_Temperature_t*)&msg->payload;

              printf("received temperature: %d deg\n", temperature->temperature);

              sendAck(true);

              break;
            };

            case LLCP_FRAME_DATA_TERMINATOR_MSG_ID: {

              LLCP_FrameDataTerminatorMsg_t* msg = (LLCP_FrameDataTerminatorMsg_t*)message_in;
              ntoh_LLCP_FrameDataTerminatorMsg_t(msg);
              LLCP_FrameDataTerminator_t* terminator = (LLCP_FrameDataTerminator_t*)&msg->payload;

              measuring_frame_ = false;

              printf("received frame data terminator: frame id %d, packet count: %d\n", terminator->frame_id, terminator->n_packets);

              sendAck(true);

              break;
            };

            case LLCP_ACK_MSG_ID: {

              LLCP_AckMsg_t* msg = (LLCP_AckMsg_t*)message_in;
              ntoh_LLCP_AckMsg_t(msg);
              LLCP_Ack_t* ack = (LLCP_Ack_t*)&msg->payload;

              printf("received ack: %s\n", ack->success ? "true" : "false");

              break;
            };

            case LLCP_MINIPIX_ERROR_MSG_ID: {

              LLCP_MinipixErrorMsg_t* msg = (LLCP_MinipixErrorMsg_t*)message_in;
              ntoh_LLCP_MinipixErrorMsg_t(msg);
              LLCP_MinipixError_t* error = (LLCP_MinipixError_t*)&msg->payload;

              switch (error->error_id) {

                case LLCP_MINIPIX_ERROR_MEASUREMENT_FAILED: {

                  measuring_frame_ = false;

                  break;
                }

                default: {
                  printf("received unhandled error message, id %d\n", error->error_id);
                }
              }

              printf("received MiniPIX error %d: %s\n", error->error_id, LLCP_MinipixErrors[error->error_id]);

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

    cv::Mat frame_top_plot(256, 256, CV_32FC3);
    cv::Mat frame_bot_plot(256, 256, CV_32FC3);

    {
      std::scoped_lock lock(mutex_cv_frames_);

      cv::normalize(frame_top, frame_top_plot, 0.05, 1.0, cv::NORM_MINMAX);
      cv::normalize(frame_bot, frame_bot_plot, 0.05, 1.0, cv::NORM_MINMAX);
    }

    /* cv::subtract(cv::Scalar::all(1.0), frame_top_plot, frame_top_plot); */
    /* cv::subtract(cv::Scalar::all(1.0), frame_bot_plot, frame_bot_plot); */

    ShowManyImages<CV_32FC3>("frame", 2, frame_top_plot, frame_bot_plot);

    cv::waitKey(30);
  }
}

//}

// | --------------------- MiniPIX control -------------------- |

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

/* startStream() //{ */

void Gatherer::startStream(const uint16_t& duty_cycle) {

  // create the message
  LLCP_MeasureStreamReqMsg_t msg;
  init_LLCP_MeasureStreamReqMsg_t(&msg);

  // fill in the payload
  msg.payload.duty_cycle_ms = duty_cycle;

  // convert to network endian
  hton_LLCP_MeasureStreamReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), tx_buffer);

  {
    std::scoped_lock lock(mutex_serial_port_);

    serial_port_.sendCharArray(tx_buffer, n_bytes);
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

int main(int argc, char* argv[]) {

  std::string serial_port_file;
  int         baud_rate;
  bool        serial_port_virtual;

  if (argc == 4) {
    serial_port_file    = argv[1];
    baud_rate           = atoi(argv[2]);
    serial_port_virtual = atoi(argv[3]);

    printf("loaded params: %s, %d, %s\n", serial_port_file.c_str(), baud_rate, serial_port_virtual ? "VIRTUAL" : "REAL");
  } else {
    printf("params not supplied\n");
    return 0;
  }

  Gatherer gatherer;

  gatherer.connect(serial_port_file, baud_rate, serial_port_virtual);

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  printf("powering on\n");

  gatherer.pwr(true);

  std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  /* gatherer.maskPixel(10, 20); */

  /* std::this_thread::sleep_for(std::chrono::milliseconds(100)); */

  /* gatherer.setThreshold(333, 555); */

  /* std::this_thread::sleep_for(std::chrono::milliseconds(100)); */

  /* gatherer.setConfigurationPreset(2); */

  /* std::this_thread::sleep_for(std::chrono::milliseconds(100)); */

  gatherer.getStatus();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  gatherer.getTemperature();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  while (true) {

    gatherer.measureFrame(1000);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  gatherer.pwr(false);
}
