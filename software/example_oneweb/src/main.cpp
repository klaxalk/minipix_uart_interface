#include <chrono>
#include <thread>
#include <atomic>

#include <serial_port.h>

#include <mui.h>

#define RX_SERIAL_BUFFER_SIZE 2048
#define TX_SERIAL_BUFFER_SIZE 512

SerialPort serial_port_minipix_;
uint8_t    tx_buffer[TX_SERIAL_BUFFER_SIZE];

MUI_Handler_t mui_handler_;

std::thread thread_serial_;

std::atomic<bool> got_ack_         = false;
std::atomic<bool> measuring_frame_ = false;

std::atomic<bool> counting_pixels_ = false;
int               pixel_count_     = 0;

FILE *measured_data_file_;

#define CONFIGURATION_PRESET 0

// how many pixels should be measured in the A1 mode
#define DESIRED_OCCUPANCY_N_PIXELS 1500

// --------------------------------------------------------------
// |                     saving data to file                    |
// --------------------------------------------------------------

/* bin2hex() //{ */

void bin2hex(const uint8_t x, uint8_t *buffer) {

  if (x >= 16) {
    *buffer       = "0123456789ABCDEF"[x / 16];
    *(buffer + 1) = "0123456789ABCDEF"[x & 15];
  } else {
    *buffer       = '0';
    *(buffer + 1) = "0123456789ABCDEF"[x];
  }
}


//}

/* saveFrameDataToFile() //{ */

void saveFrameDataToFile(const LLCP_FrameDataMsg_t &msg) {

  // max llcp message size * 2
  uint8_t out_buffer[3];
  memset(out_buffer, 0, 3);

  // fill in the out buffer with the message in HEX form
  for (size_t i = 0; i < sizeof(LLCP_FrameDataMsg_t); i++) {

    bin2hex(*(((uint8_t *)&msg) + i), out_buffer);

    fprintf(measured_data_file_, "%s", out_buffer);
  }

  fprintf(measured_data_file_, "\n");

  // probably not neccessary, but to be sure...
  fflush(measured_data_file_);
}

//}

// --------------------------------------------------------------
// |                    callbacks for the MUI                   |
// --------------------------------------------------------------

/* mui_linux_processFrameData() //{ */

void mui_linux_processFrameData(const LLCP_FrameData_t *data) {

  // measurement mode a1
  if (counting_pixels_) {

    pixel_count_ += data->n_pixels;

  } else {

    // | ------ here, process/save the data however you need ------ |
    printf("received data, saving them\n");

    LLCP_FrameDataMsg_t msg;
    init_LLCP_FrameDataMsg_t(&msg);

    // fill in the payload
    msg.payload = *data;

    saveFrameDataToFile(msg);
  }

  // | ----- send ack, this will make minipix send more data ---- |

  {
    // create the message
    LLCP_AckMsg_t msg;
    init_LLCP_AckMsg_t(&msg);

    msg.payload.success = 1;

    // convert to network endian
    hton_LLCP_AckMsg_t(&msg);

    uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), tx_buffer);

    serial_port_minipix_.sendCharArray(tx_buffer, n_bytes);
  }
}

//}

/* mui_linux_processFrameDataTerminator() //{ */

void mui_linux_processFrameDataTerminator(const LLCP_FrameDataTerminator_t *data) {

  measuring_frame_ = false;
}

//}

/* mui_linux_processStatus() //{ */

void mui_linux_processStatus(const LLCP_Status_t *data) {
}

//}

/* mui_linux_processTemperature() //{ */

void mui_linux_processTemperature(const LLCP_Temperature_t *data) {
}

//}

/* mui_linux_processAck() //{ */

void mui_linux_processAck(const LLCP_Ack_t *data) {

  LLCP_AckMsg_t *msg = (LLCP_AckMsg_t *)data;
  ntoh_LLCP_AckMsg_t(msg);
  LLCP_Ack_t *ack = (LLCP_Ack_t *)&msg->payload;

  /* printf("received ack: %s\n", ack->success ? "true" : "false"); */

  got_ack_ = true;
}

//}

/* mui_linux_processMinipixError() //{ */

void mui_linux_processMinipixError(const LLCP_MinipixError_t *data) {
}

//}

/* mui_linux_processMeasurementFinished() //{ */

void mui_linux_processMeasurementFinished() {

  // | ------------------- request frame data ------------------- |

  // create the message
  LLCP_GetFrameDataReqMsg_t msg;
  init_LLCP_GetFrameDataReqMsg_t(&msg);

  // convert to network endian
  hton_LLCP_GetFrameDataReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), tx_buffer);

  serial_port_minipix_.sendCharArray(tx_buffer, n_bytes);
}

//}

/* mui_linux_ledSetHW() //{ */

void mui_linux_ledSetHW([[maybe_unused]] bool state) {
  // do nothing, we don't have LEDs on LINUX
}

//}

// --------------------------------------------------------------
// |                     methods for the MUI                    |
// --------------------------------------------------------------

/* mui_linux_sleepHW() //{ */

void mui_linux_sleepHW(const uint16_t milliseconds) {

  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

//}

/* mui_linux_sendChar() //{ */

void mui_linux_sendChar(const uint8_t char_out) {

  serial_port_minipix_.sendChar(char_out);
}

//}

/* mui_linux_sendString() //{ */

void mui_linux_sendString(const uint8_t *str_out, const uint16_t len) {

  if (!serial_port_minipix_.sendCharArray((unsigned char *)str_out, len)) {
    printf("FAILED sending message with %d bytes\n", len);
  }
}

//}

// --------------------------------------------------------------
// |              OneWeb-specific measurement modes             |
// --------------------------------------------------------------

/* a1() //{ */

void a1() {

  printf("Measurement mode 1\n");

  // | --------------------- PWR ON Minipix --------------------- |

  {

    printf("powering on\n");

    // create the message
    LLCP_PwrReqMsg_t msg;
    init_LLCP_PwrReqMsg_t(&msg);

    // fill in the payload
    msg.payload.state = 1;

    // convert to network endian
    hton_LLCP_PwrReqMsg_t(&msg);

    uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), tx_buffer);

    serial_port_minipix_.sendCharArray(tx_buffer, n_bytes);

    // wait for acknowledge
    while (!got_ack_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    got_ack_ = false;
  }

  // | -------------------- Set Configuration ------------------- |

  {
    printf("setting configuration\n");

    // create the message
    LLCP_SetConfigurationPresetReqMsg_t msg;
    init_LLCP_SetConfigurationPresetReqMsg_t(&msg);

    // fill in the payload
    msg.payload.preset = CONFIGURATION_PRESET;

    // convert to network endian
    hton_LLCP_SetConfigurationPresetReqMsg_t(&msg);

    uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), tx_buffer);

    serial_port_minipix_.sendCharArray(tx_buffer, n_bytes);

    // wait for acknowledge
    while (!got_ack_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    got_ack_ = false;
  }

  // | -------- Do 1 ms exposure and save the pixel count ------- |

  {
    // create the message
    LLCP_MeasureFrameReqMsg_t msg;
    init_LLCP_MeasureFrameReqMsg_t(&msg);

    // fill in the payload
    msg.payload.acquisition_time_ms = 1;
    msg.payload.mode                = LLCP_TPX3_PXL_MODE_MPX_ITOT;

    // convert to network endian
    hton_LLCP_MeasureFrameReqMsg_t(&msg);

    uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), tx_buffer);

    measuring_frame_ = true;
    counting_pixels_ = true;

    serial_port_minipix_.sendCharArray(tx_buffer, n_bytes);

    // wait for the measurement to finish
    while (measuring_frame_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    measuring_frame_ = false;
    counting_pixels_ = false;
  }

  // save how many pixels were hit during the 9 ms acquisition
  int n_pixels_1ms = pixel_count_;

  printf("[A1] 1 ms test acquisition: %d\n", n_pixels_1ms);

  {
    // create the message
    LLCP_MeasureFrameReqMsg_t msg;
    init_LLCP_MeasureFrameReqMsg_t(&msg);

    // fill in the payload
    msg.payload.acquisition_time_ms = 11;
    msg.payload.mode                = LLCP_TPX3_PXL_MODE_MPX_ITOT;

    // convert to network endian
    hton_LLCP_MeasureFrameReqMsg_t(&msg);

    uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), tx_buffer);

    measuring_frame_ = true;
    counting_pixels_ = true;

    serial_port_minipix_.sendCharArray(tx_buffer, n_bytes);

    // wait for the measurement to finish
    while (measuring_frame_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    measuring_frame_ = false;
    counting_pixels_ = false;
  }

  // save how many pixels were hit during the 1 ms acquisition
  int n_pixels_9ms = pixel_count_;

  printf("[A1] 9 ms test acquisition: %d\n", n_pixels_9ms);

  // calculate the desired exposure time for the whole frame
  int pixels_per_ms = (n_pixels_9ms - n_pixels_1ms) / 8;  // implement this using bit shift if you need to
  int desired_ms    = DESIRED_OCCUPANCY_N_PIXELS / pixels_per_ms;

  printf("[A1] desired A1 acquisition_time = %d ms\n", desired_ms);

  {
    // create the message
    LLCP_MeasureFrameReqMsg_t msg;
    init_LLCP_MeasureFrameReqMsg_t(&msg);

    // fill in the payload
    msg.payload.acquisition_time_ms = desired_ms;
    msg.payload.mode                = LLCP_TPX3_PXL_MODE_MPX_ITOT;

    // convert to network endian
    hton_LLCP_MeasureFrameReqMsg_t(&msg);

    uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), tx_buffer);

    measuring_frame_ = true;

    serial_port_minipix_.sendCharArray(tx_buffer, n_bytes);

    // wait for the measurement to finish
    while (measuring_frame_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    measuring_frame_ = false;
  }

  printf("[A1] finished\n");
}

//}

// --------------------------------------------------------------
// |                            main                            |
// --------------------------------------------------------------

/* threadSerial() //{ */

// reading out data from the UART and giving them to the MUI
void threadSerial(void) {

  while (true) {

    {
      // | --------- receive data from the minipix interface -------- |

      uint8_t  buffer[RX_SERIAL_BUFFER_SIZE];
      uint16_t bytes_read = serial_port_minipix_.readSerial(buffer, RX_SERIAL_BUFFER_SIZE);

      // feed all the incoming bytes (from the MiniPIX) into the MUI
      for (uint16_t i = 0; i < bytes_read; i++) {
        mui_receiveCharCallback(&mui_handler_, buffer[i]);
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

//}

/* main() //{ */

int main(int argc, char *argv[]) {

  std::string data_path;
  std::string serial_port_name;
  int         baud_rate;
  bool        serial_port_virtual;

  if (argc == 5) {

    serial_port_name    = argv[1];
    baud_rate           = atoi(argv[2]);
    serial_port_virtual = atoi(argv[3]);
    data_path           = argv[4];

    printf("loaded params:\n");
    printf("minipix: %s, %d, %s, data path %s\n", serial_port_name.c_str(), baud_rate, serial_port_virtual ? "VIRTUAL" : "REAL", data_path.c_str());

  } else {
    printf("params not supplied\n");
    return 0;
  }

  // open the serial ports
  serial_port_minipix_.connect(serial_port_name, baud_rate, serial_port_virtual);

  // | -------- initialize the MiniPIX interface library -------- |

  mui_handler_.fcns.ledSetHW                        = &mui_linux_ledSetHW;
  mui_handler_.fcns.sleepHW                         = &mui_linux_sleepHW;
  mui_handler_.fcns.processFrameData                = &mui_linux_processFrameData;
  mui_handler_.fcns.processFrameDataTerminator      = &mui_linux_processFrameDataTerminator;
  mui_handler_.fcns.processStatus                   = &mui_linux_processStatus;
  mui_handler_.fcns.processTemperature              = &mui_linux_processTemperature;
  mui_handler_.fcns.processAck                      = &mui_linux_processAck;
  mui_handler_.fcns.processMinipixError             = &mui_linux_processMinipixError;
  mui_handler_.fcns.processFrameMeasurementFinished = &mui_linux_processMeasurementFinished;

  // the user can supply either sendChar or sendString method
  // MUI needs this compiler preprocessor definition to build itself using the right one
  // please supply the definition during compilation, e.g., in CMakeLists
#if MUI_SEND_CHAR == 1
  mui_handler.fcns.sendChar = &mui_linux_sendChar;
#elif MUI_SEND_STRING == 1
  mui_handler_.fcns.sendString = &mui_linux_sendString;
#endif

  mui_initialize(&mui_handler_);

  // | --------------- create file for saving data -------------- |

  measured_data_file_ = fopen(data_path.c_str(), "w");

  if (measured_data_file_ == NULL) {
    printf("Error: cannot open the data output file '%s' for writing!\n", data_path.c_str());
  }

  // | -------- start the thread reading out serial data -------- |

  thread_serial_ = std::thread(threadSerial);

  printf("Linux example MUI started\n");

  a1();

  return 0;
}

//}
