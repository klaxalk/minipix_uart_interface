#include <chrono>
#include <thread>
#include <atomic>

#include <serial_port.h>

#include <mui.h>

// This is better to be large for Linux, in case the serial driver
// fills in more than one packet.
#define RX_SERIAL_BUFFER_SIZE 5 * LLCP_RX_TX_BUFFER_SIZE

#define TX_SERIAL_BUFFER_SIZE LLCP_RX_TX_BUFFER_SIZE

SerialPort serial_port_minipix_;
uint8_t    tx_buffer[TX_SERIAL_BUFFER_SIZE];

MUI_Handler_t mui_handler_;

std::thread thread_serial_;

std::atomic<bool> got_ack_         = false;
std::atomic<bool> measuring_frame_ = false;

std::atomic<bool> counting_pixels_ = false;
int               pixel_count_     = 0;

int16_t           temperature_;
std::atomic<bool> measuring_temperature_ = false;

FILE *measured_data_file_;

// --------------------------------------------------------------
// |               Method for saving data to file               |
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

void saveFrameDataToFile(const LLCP_FrameData_t *data) {

  // I am putting the data back into our communication packet
  // ... to be able to decode it fully from the file.

  LLCP_FrameDataMsg_t msg;
  init_LLCP_FrameDataMsg_t(&msg);

  // fill in the payload
  msg.payload = *data;

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
// |          method which encapsulate the MUI methods          |
// --------------------------------------------------------------

/* powerOn() //{ */

void powerOn(void) {

  LLCP_PwrReqMsg_t msg;
  init_LLCP_PwrReqMsg_t(&msg);

  // fill in the payload
  msg.payload.state = 1;

  // convert to network endian
  hton_LLCP_PwrReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), tx_buffer);

  serial_port_minipix_.sendCharArray(tx_buffer, n_bytes);
}

//}

/* powerOff() //{ */

void powerOff(void) {

  LLCP_PwrReqMsg_t msg;
  init_LLCP_PwrReqMsg_t(&msg);

  // fill in the payload
  msg.payload.state = 0;

  // convert to network endian
  hton_LLCP_PwrReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), tx_buffer);

  serial_port_minipix_.sendCharArray(tx_buffer, n_bytes);
}

//}

/* measureFrame() //{ */

void measureFrame(int acquisition_time_ms, int pixel_mode) {

  measuring_frame_ = true;

  // create the message
  LLCP_MeasureFrameReqMsg_t msg;
  init_LLCP_MeasureFrameReqMsg_t(&msg);

  // fill in the payload
  msg.payload.acquisition_time_ms = acquisition_time_ms;
  msg.payload.mode                = pixel_mode;

  // convert to network endian
  hton_LLCP_MeasureFrameReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), tx_buffer);

  serial_port_minipix_.sendCharArray(tx_buffer, n_bytes);
}

//}

/* setConfiguration() //{ */

void setConfiguration(int configuration_id) {

  // create the message
  LLCP_SetConfigurationPresetReqMsg_t msg;
  init_LLCP_SetConfigurationPresetReqMsg_t(&msg);

  // fill in the payload
  msg.payload.preset = configuration_id;

  // convert to network endian
  hton_LLCP_SetConfigurationPresetReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), tx_buffer);

  serial_port_minipix_.sendCharArray(tx_buffer, n_bytes);
}

//}

/* setThreshold() //{ */

void setThreshold(const uint16_t &coarse, const uint16_t &fine) {

  // create the message
  LLCP_SetThresholdReqMsg_t msg;
  init_LLCP_SetThresholdReqMsg_t(&msg);

  // fill in the payload
  msg.payload.threshold_coarse = coarse;
  msg.payload.threshold_fine   = fine;

  // convert to network endian
  hton_LLCP_SetThresholdReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), tx_buffer);

  serial_port_minipix_.sendCharArray(tx_buffer, n_bytes);
}

//}

/* getTemperature() //{ */

void getTemperature() {

  measuring_temperature_ = true;

  // create the message
  LLCP_GetTemperatureReqMsg_t msg;
  init_LLCP_GetTemperatureReqMsg_t(&msg);

  // convert to network endian
  hton_LLCP_GetTemperatureReqMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t *)&msg, sizeof(msg), tx_buffer);

  serial_port_minipix_.sendCharArray(tx_buffer, n_bytes);
}

//}

// --------------------------------------------------------------
// |                    callbacks for the MUI                   |
// --------------------------------------------------------------

/* mui_linux_processFrameData() //{ */

void mui_linux_processFrameData(const LLCP_FrameData_t *data) {

  // measurement mode a1
  if (counting_pixels_) {

    // just record how many pixels were hit
    // and do NOT save the data
    pixel_count_ += data->n_pixels;

  } else {

    // | ------ here, process/save the data however you need ------ |

    saveFrameDataToFile(data);
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

void mui_linux_processFrameDataTerminator([[maybe_unused]] const LLCP_FrameDataTerminator_t *data) {

  measuring_frame_ = false;
}

//}

/* mui_linux_processStatus() //{ */

void mui_linux_processStatus(const LLCP_Status_t *data) {

  printf("received status message:\n");

  // TODO for OneWeb
  // possibly gather this regularly using some payload housekeeping collector
}

//}

/* mui_linux_processTemperature() //{ */

void mui_linux_processTemperature(const LLCP_Temperature_t *data) {

  temperature_           = data->temperature;
  measuring_temperature_ = false;
}

//}

/* mui_linux_processAck() //{ */

void mui_linux_processAck([[maybe_unused]] const LLCP_Ack_t *data) {

  got_ack_ = true;
}

//}

/* mui_linux_processMinipixError() //{ */

void mui_linux_processMinipixError(const LLCP_MinipixError_t *data) {

  // TODO OneWeb
  // please save these errors
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

/* measurementA1() //{ */

void measurementA1(uint16_t desired_occupancy, int pixel_mode, uint16_t threshold_coarse, uint16_t threshold_fine, uint16_t default_acquisition_time,
                   int configuration_id, bool use_temp_for_config, uint16_t temp_threshold) {

  printf("Measurement mode A1\n");

  // this may be changed down below by logic
  int configuration = configuration_id;

  // | --------------------- PWR ON Minipix --------------------- |

  {

    printf("[A1] powering on\n");

    powerOn();

    // wait for acknowledge
    while (!got_ack_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    got_ack_ = false;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  // | ------------------- measure temperatur ------------------- |

  {
    printf("[A1] getting temperature\n");

    getTemperature();

    // wait for data
    while (measuring_temperature_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  if (use_temp_for_config) {
    if (temperature_ > temp_threshold) {
      // configuration preset for high temperature
      configuration = 1;
    } else {
      // configuration preset for low temperature
      configuration = 0;
    }
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  // | -------------------- Set Configuration ------------------- |

  {
    printf("[A1] setting configuration\n");

    setConfiguration(configuration);

    // wait for acknowledge
    while (!got_ack_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    got_ack_ = false;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  // | ---------------------- set threshold --------------------- |

  {
    printf("[A1] setting threshold\n");

    setThreshold(threshold_coarse, threshold_fine);

    // wait for acknowledge
    while (!got_ack_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    got_ack_ = false;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  // | ---------------- do short testing exposure --------------- |

  uint16_t short_acquisition_time = 10;

  {

    printf("[A1] measuring 1st test frame\n");

    counting_pixels_ = true;
    pixel_count_     = 0;

    measureFrame(short_acquisition_time, LLCP_TPX3_PXL_MODE_MPX_ITOT);

    // wait for the measurement to finish
    while (measuring_frame_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    counting_pixels_ = false;
  }

  // save how many pixels were hit during the 9 ms acquisition
  int n_pixels_short = pixel_count_;

  printf("[A1] short test acquisition: %d pixels\n", n_pixels_short);

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  uint16_t long_acquisition_time = 1000;

  {
    printf("[A1] measuring long test frame\n");

    counting_pixels_ = true;

    measureFrame(long_acquisition_time, LLCP_TPX3_PXL_MODE_MPX_ITOT);

    // wait for the measurement to finish
    while (measuring_frame_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    counting_pixels_ = false;
  }

  // save how many pixels were hit during the 1 ms acquisition
  int n_pixels_long = pixel_count_;

  printf("[A1] long test acquisition: %d pixels\n", n_pixels_long);

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  // calculate the desired exposure time for the whole frame
  // aka, the slope of the affine function, a in Pixels = a*Time + b
  uint16_t pixels_per_ms = int(float(n_pixels_long - n_pixels_short) / float(long_acquisition_time - short_acquisition_time));

  uint16_t desired_ms;

  if (pixels_per_ms > 0) {

    // calculate the offset caused by "dead and active"
    // aka the b in Pixels = a*Time + b
    uint16_t pixels_0_ms = n_pixels_long - pixels_per_ms * long_acquisition_time;

    // calculate the desired acquisition time based on the curve a*Time + b
    desired_ms = int(float(desired_occupancy - pixels_0_ms) / float(pixels_per_ms));

    if (desired_ms <= 0) {
      desired_ms = default_acquisition_time;
    }

  } else {
    desired_ms = default_acquisition_time;
  }

  printf("[A1] desired A1 acquisition_time = %d ms\n", desired_ms);

  {
    printf("[A1] measuring A1 main frame\n");

    measureFrame(desired_ms, pixel_mode);

    // wait for the measurement to finish
    while (measuring_frame_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  {

    printf("[A1] powering off\n");

    powerOff();

    // wait for acknowledge
    while (!got_ack_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    got_ack_ = false;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  printf("[A1] finished\n");
}

//}

/* measurementA2() //{ */

void measurementA2(int pixel_mode, uint16_t acquisition_time_ms, uint16_t threshold_coarse, uint16_t threshold_fine, int configuration_id,
                   bool use_temp_for_config, uint16_t temp_threshold) {

  printf("Measurement mode A2\n");

  // this may be changed down below by logic
  int configuration = configuration_id;

  // | --------------------- PWR ON Minipix --------------------- |

  {

    printf("[A2] powering on\n");

    powerOn();

    // wait for acknowledge
    while (!got_ack_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    got_ack_ = false;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  // | ------------------- measure temperatur ------------------- |

  {
    printf("[A1] getting temperature\n");

    getTemperature();

    // wait for data
    while (measuring_temperature_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  if (use_temp_for_config) {
    if (temperature_ > temp_threshold) {
      // configuration preset for high temperature
      configuration = 1;
    } else {
      // configuration preset for low temperature
      configuration = 0;
    }
  }

  // | -------------------- Set Configuration ------------------- |

  {
    printf(" setting configuration\n");

    setConfiguration(configuration);

    // wait for acknowledge
    while (!got_ack_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    got_ack_ = false;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  // | ---------------------- set threshold --------------------- |

  {
    printf("[A2] setting threshold\n");

    setThreshold(threshold_coarse, threshold_fine);

    // wait for acknowledge
    while (!got_ack_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    got_ack_ = false;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  // | -------- Do 1 ms exposure and save the pixel count ------- |

  {
    printf("[A2] measuring image\n");

    measureFrame(acquisition_time_ms, pixel_mode);

    // wait for the measurement to finish
    while (measuring_frame_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  {

    printf("[A2] powering off\n");

    powerOff();

    // wait for acknowledge
    while (!got_ack_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    got_ack_ = false;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  printf("[A2] finished\n");
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

  // --------------------------------------------------------------
  // |                   let's measure something                  |
  // --------------------------------------------------------------

  // the following parameters should be configurable from Earth

  // global parameters
  uint16_t PARAM_THRESHOLD_COARSE      = 120;
  uint16_t PARAM_THRESHOLD_FINE        = 350;
  bool     PARAM_SET_CONFIG_USING_TEMP = false;
  uint16_t PARAM_CONFIG_TEMP_THRESHOLD = 35;

  // A1-specific parameters
  uint16_t PARAM_A1_DESIRED_OCCUPANCY_PX     = 1500;
  uint16_t PARAM_A1_DEFAULT_ACQUISITION_TIME = 1000;
  uint8_t  PARAM_A1_CONFIGURATION_ID         = 0;
  uint8_t  PARAM_A1_PXL_MODE                 = LLCP_TPX3_PXL_MODE_TOA_TOT;  // {0, 1, 2}

  // A2-specific parameters
  uint8_t  PARAM_A2_CONFIGURATION_ID = 0;
  uint16_t PARAM_A2_ACQUISITION_TIME = 60000;
  uint8_t  PARAM_A2_PXL_MODE         = LLCP_TPX3_PXL_MODE_TOA_TOT;  // {0, 1, 2}

  for (int i = 0; i < 10; i++) {

    measurementA1(PARAM_A1_DESIRED_OCCUPANCY_PX, PARAM_A1_PXL_MODE, PARAM_THRESHOLD_COARSE, PARAM_THRESHOLD_FINE, PARAM_A1_DEFAULT_ACQUISITION_TIME,
                  PARAM_A1_CONFIGURATION_ID, PARAM_SET_CONFIG_USING_TEMP, PARAM_CONFIG_TEMP_THRESHOLD);

    for (int j = 0; j < 6; j++) {
      measurementA2(PARAM_A2_PXL_MODE, PARAM_A2_ACQUISITION_TIME, PARAM_THRESHOLD_COARSE, PARAM_THRESHOLD_FINE, PARAM_A2_CONFIGURATION_ID,
                    PARAM_SET_CONFIG_USING_TEMP, PARAM_CONFIG_TEMP_THRESHOLD);
    }
  }

  return 0;
}

//}
