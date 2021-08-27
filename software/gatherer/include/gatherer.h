#ifndef GATHERER_H
#define GATHERER_H

#define LLCP_DEBUG_PRINT 0
#define SERIAL_BUFFER_SIZE 2048
#define MUI_USER_HANDSHAKES 0

#ifndef GUI
#define GUI 0
#endif

/* includes //{ */

#include <llcp.h>
#include <llcp_minipix_messages.h>

#if GUI == 1
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv_helpers.hpp>
#endif

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
  Gatherer(const std::string data_path);

  void connect(const std::string& serial_port, const int& baud_rate, const bool& virtual_port);

  void charCallback(const char in);

  // | --------------------- MiniPIX control -------------------- |

  void getStatus(void);
  void getTemperature(void);
  void measureFrame(const uint16_t& acquisition_time_ms, const uint8_t &mode);
  void pwr(const bool& state);
  void maskPixel(const uint8_t& x, const uint8_t& y);
  void setThreshold(const uint16_t& coarse, const uint16_t& fine);
  void setConfigurationPreset(const uint16_t& preset);
  void sendAck(bool ack);
  void getFrameData(void);

public:
  bool measuring_frame_    = false;
  bool waiting_for_ack_    = false;
  bool waiting_for_tmp_    = false;
  bool waiting_for_status_ = false;

private:

  SerialPort serial_port_;
  std::mutex mutex_serial_port_;
  uint8_t tx_buffer[SERIAL_BUFFER_SIZE];

  LLCP_Receiver_t llcp_receiver;

  std::thread thread_main_;
  void        threadMain(void);

  std::atomic<bool> initialized_ = false;

  // | ------------------------ callbacks ----------------------- |

  void callbackFrameData(const LLCP_Message_t* message_in);
  void callbackStatus(const LLCP_Message_t* message_in);
  void callbackTemperature(const LLCP_Message_t* message_in);
  void callbackFrameTerminator(const LLCP_Message_t* message_in);
  void callbackAck(const LLCP_Message_t* message_in);
  void callbackFrameMeasurementFinished(const LLCP_Message_t* message_in);
  void callbackError(const LLCP_Message_t* message_in);

  // | --------------------- helper routines -------------------- |

  void saveFrameDataToFile(const LLCP_FrameDataMsg_t& msg);
  void bin2hex(const uint8_t x, uint8_t* buffer);

  // | ------------ saving measured frames to a file ------------ |

  FILE* measured_data_file_;

#if GUI == 1
  // | ------------------------ plotting ------------------------ |

  std::thread thread_plot_;
  void        threadPlot(void);


  cv::Mat frame_top_left;
  cv::Mat frame_top_right;
  cv::Mat frame_bot_left;
  cv::Mat frame_bot_right;

  std::mutex mutex_cv_frames_;

#endif
};

#endif  // GATHERER_H
