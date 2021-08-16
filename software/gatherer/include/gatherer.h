#ifndef GATHERER_H
#define GATHERER_H

#define LLCP_DEBUG_PRINT 0
#define SERIAL_BUFFER_SIZE 2048

/* includes //{ */

#include <llcp.h>
#include <llcp_minipix_messages.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv_helpers.hpp>

#include <mutex>
#include <chrono>
#include <thread>
#include <atomic>
#include <math.h>

#include <serial_port.h>

#include <pixel_decoder.h>

//}

class Gatherer {

public:
  Gatherer();

  void connect(const std::string& serial_port, const int& baud_rate, const bool& virtual_port);

  void charCallback(const char in);

  // | --------------------- MiniPIX control -------------------- |

  void getStatus(void);
  void getTemperature(void);
  void measureFrame(const uint16_t& acquisition_time_ms);
  void pwr(const bool& state);
  void maskPixel(const uint8_t& x, const uint8_t& y);
  void setThreshold(const uint16_t& coarse, const uint16_t& fine);
  void setConfigurationPreset(const uint16_t& preset);
  void sendAck(bool ack);

public:
  bool measuring_frame_    = false;
  bool waiting_for_ack_    = false;
  bool waiting_for_tmp_    = false;
  bool waiting_for_status_ = false;

private:
  SerialPort serial_port_;
  std::mutex mutex_serial_port_;

  LLCP_Receiver_t llcp_receiver;

  uint8_t tx_buffer[SERIAL_BUFFER_SIZE];

  std::thread thread_main_;
  void        threadMain(void);

  std::thread thread_plot_;
  void        threadPlot(void);

  std::atomic<bool> initialized_ = false;

  // | ------------------------ callbacks ----------------------- |

  void callbackFrameData(const LLCP_Message_t* message_in);
  void callbackStatus(const LLCP_Message_t* message_in);
  void callbackTemperature(const LLCP_Message_t* message_in);
  void callbackFrameTerminator(const LLCP_Message_t* message_in);
  void callbackAck(const LLCP_Message_t* message_in);
  void callbackError(const LLCP_Message_t* message_in);

  // | ------------------------ plotting ------------------------ |

  cv::Mat frame_top_left;
  cv::Mat frame_top_right;
  cv::Mat frame_bot_left;
  cv::Mat frame_bot_right;

  std::mutex mutex_cv_frames_;
};

#endif  // GATHERER_H
