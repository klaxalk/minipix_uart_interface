#include <llcp.h>
#include <llcp_minipix_messages.h>

#include <opencv2/imgproc.hpp>
#include <thread>
#include <chrono>

#include <math.h>

#include <serial_port.h>

#define GUI 1

#ifdef GUI
/* some OpenCV includes */
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#endif

#define SERIAL_BUFFER_SIZE 2048

std::string serial_port_file;
int         baud_rate;
bool        serial_port_virtual;

SerialPort serial_port;

uint8_t tx_buffer[SERIAL_BUFFER_SIZE];

/* getStatus() //{ */

void getStatus() {

  // create the message
  LLCP_GetStatusReqMsg_t msg;
  init_LLCP_GetStatusReqMsg_t(&msg);

  // convert to network endian
  hton_LLCP_GetStatusReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), tx_buffer);

  serial_port.sendCharArray(tx_buffer, n_bytes);
}

//}

/* measureFrame() //{ */

void measureFrame(const uint16_t& acquisition_time_ms) {

  // create the message
  LLCP_MeasureFrameReqMsg_t msg;
  init_LLCP_MeasureFrameReqMsg_t(&msg);

  // fill in the payload
  msg.payload.acquisition_time_ms = acquisition_time_ms;

  // convert to network endian
  hton_LLCP_MeasureFrameReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), tx_buffer);

  serial_port.sendCharArray(tx_buffer, n_bytes);
}

//}

/* startStream() //{ */

void startStream(const uint16_t& duty_cycle) {

  // create the message
  LLCP_MeasureStreamReqMsg_t msg;
  init_LLCP_MeasureStreamReqMsg_t(&msg);

  // fill in the payload
  msg.payload.duty_cycle_ms = duty_cycle;

  // convert to network endian
  hton_LLCP_MeasureStreamReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), tx_buffer);

  serial_port.sendCharArray(tx_buffer, n_bytes);
}

//}

/* pwr() //{ */

void pwr(const bool& state) {

  // create the message
  LLCP_PwrReqMsg_t msg;
  init_LLCP_PwrReqMsg_t(&msg);

  // fill in the payload
  msg.payload.state = state;

  // convert to network endian
  hton_LLCP_PwrReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), tx_buffer);

  serial_port.sendCharArray(tx_buffer, n_bytes);
}

//}

/* maskPixel() //{ */

void maskPixel(const uint8_t& x, const uint8_t& y) {

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

  serial_port.sendCharArray(tx_buffer, n_bytes);
}

//}

int main(int argc, char* argv[]) {

  if (argc == 4) {
    serial_port_file    = argv[1];
    baud_rate           = atoi(argv[2]);
    serial_port_virtual = atoi(argv[3]);

    printf("loaded params: %s, %d, %s\n", serial_port_file.c_str(), baud_rate, serial_port_virtual ? "VIRTUAL" : "REAL");
  } else {
    printf("params not supplied\n");
    return 0;
  }

  serial_port.connect(serial_port_file, baud_rate, serial_port_virtual);

  uint8_t  read_buffer[SERIAL_BUFFER_SIZE];
  uint16_t bytes_read;

  LLCP_Receiver_t llcp_receiver;
  llcp_initialize(&llcp_receiver);

#ifdef GUI
  /* int flags = cv::WINDOW_NORMAL | cv::WINDOW_FREERATIO | cv::WINDOW_GUI_EXPANDED; */
  int flags = cv::WINDOW_NORMAL;
  cv::namedWindow("frame", flags);

  cv::Mat frame_tot(256, 256, CV_32FC3);
  cv::Mat frame_toa(256, 256, CV_32FC3);
#endif

  printf("Starting while loop\n");

  pwr(true);

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  getStatus();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  measureFrame(100);
  /* maskPixel(10, 20); */
  /* startStream(200); */

  /* std::this_thread::sleep_for(std::chrono::milliseconds(100)); */

  pwr(false);

  while (true) {

    // | --------- receive data from the minipix interface -------- |

    bytes_read = serial_port.readSerial(read_buffer, SERIAL_BUFFER_SIZE);

    if (bytes_read > 0) {

      // feed all the incoming bytes into the minipix interface
      for (uint16_t i = 0; i < bytes_read; i++) {

        LLCP_Message_t message_in;

        if (llcp_processChar(read_buffer[i], &llcp_receiver, &message_in)) {

          switch (message_in.id) {

            case LLCP_FRAME_DATA_MSG_ID: {

              LLCP_FrameDataMsg_t* msg = (LLCP_FrameDataMsg_t*)&message_in.payload;
              ntoh_LLCP_FrameDataMsg_t(msg);

              LLCP_FrameData_t* image = (LLCP_FrameData_t*)&msg->payload;

              uint8_t n_pixels = image->n_pixels;

              printf("received frame datam, id %d, n_pixels %d, last_pixel_x: %d\n", image->frame_id, n_pixels, image->pixel_data[n_pixels - 1].x_coordinate);

#ifdef GUI
              for (int pix = 0; pix < n_pixels; pix++) {

                uint8_t  x   = image->pixel_data[pix].x_coordinate;
                uint8_t  y   = image->pixel_data[pix].y_coordinate;
                float toa = float(image->pixel_data[pix].toa);
                float tot = float(image->pixel_data[pix].tot);

                cv::Vec3f tot_color(0, 0, 1e3+pow(tot, 2.0)); // BGR

                cv::Vec3f toa_color(0, 1e3+pow(toa, 2.0), 0); // BGR

                printf("toa %.2f\n", toa);

                frame_tot.at<cv::Vec3f>(cv::Point(x, y)) = tot_color;
                frame_toa.at<cv::Vec3f>(cv::Point(x, y)) = toa_color;
              }
#endif

              break;
            };

            case LLCP_STREAM_DATA_MSG_ID: {

              LLCP_StreamDataMsg_t* msg = (LLCP_StreamDataMsg_t*)&message_in.payload;
              ntoh_LLCP_StreamDataMsg_t(msg);

              LLCP_StreamData_t* image = (LLCP_StreamData_t*)&msg->payload;

              uint8_t n_pixels = image->n_pixels;

              printf("received stream data, n_pixels %d, last_pixel_x: %d\n", n_pixels, image->pixel_data[n_pixels - 1].x_coordinate);

              break;
            };

            case LLCP_STATUS_MSG_ID: {

              LLCP_StatusMsg_t* msg = (LLCP_StatusMsg_t*)&message_in.payload;
              ntoh_LLCP_StatusMsg_t(msg);
              LLCP_Status_t* status = (LLCP_Status_t*)&msg->payload;

              printf("received status: boot count = %d, string: '%s'\n", status->boot_count, status->status_str);

              break;
            };

            default: {

              printf("Received unsupported message with id = %d\n", message_in.id);
            }
          }
        }
      }
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));

#ifdef GUI
      cv::Mat frame_toa_norm(256, 256, CV_32FC3);
      cv::Mat frame_tot_norm(256, 256, CV_32FC3);
      cv::Mat frame_plot(256, 256, CV_32FC3);

      cv::normalize(frame_tot, frame_tot_norm, 0.05, 1.0, cv::NORM_MINMAX);
      cv::normalize(frame_toa, frame_toa_norm, 0.05, 1.0, cv::NORM_MINMAX);

      frame_plot = frame_toa_norm + frame_tot_norm;

      cv::resize(frame_plot, frame_plot, cv::Size(800, 800), cv::INTER_CUBIC);

      cv::imshow("frame", frame_plot);

      cv::waitKey(1);
#endif
    }
  }
}
