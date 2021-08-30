#include <minipix_dummy_linux.h>

/* constructor MinipixDummyLinux() //{ */

MinipixDummyLinux::MinipixDummyLinux(void) {

  srand(time(NULL));

  thread_serial_port_ = std::thread(&MinipixDummyLinux::threadSerialPort, this);
}

//}

/* initializeSerialPort() //{ */

void MinipixDummyLinux::initializeSerialPort(const std::string& file, const int& baud, const bool virtual_port) {

  std::scoped_lock lock(mutex_serial_port_);

  serial_port_.connect(file, baud, virtual_port);
}

//}

// | ------------ virtual functions --- HW-specific ----------- |

/* sendByte() //{ */

void MinipixDummyLinux::sendByte(const uint8_t& byte_out) {

  std::scoped_lock lock(mutex_serial_port_);

  if (!serial_port_.sendCharArray((unsigned char*)&byte_out, 1)) {
    printf("FAILED sending message with %d bytes\n", 1);
  }
}

//}

/* sleep() //{ */

void MinipixDummyLinux::sleep(const uint16_t& milliseconds) {

  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

//}

/* sendString() //{ */

void MinipixDummyLinux::sendString(const uint8_t* bytes_out, const uint16_t& len) {

  std::scoped_lock lock(mutex_serial_port_);

  if (!serial_port_.sendCharArray((unsigned char*)bytes_out, len)) {
    printf("FAILED sending message with %d bytes\n", len);
  }
}

//}

/* threadSerialPort() //{ */

void MinipixDummyLinux::threadSerialPort(void) {

  printf("waiting for serial\n");

  {
    std::scoped_lock lock(mutex_serial_port_);

    while (!serial_port_.checkConnected()) {
      sleep(100);
      printf("Serial port not connected\n");
    }

    printf("Serial port connected\n");
  }

  printf("starting serial thread\n");

  while (true) {

    // | --------- receive data from the minipix interface -------- |

    uint16_t bytes_read;

    {
      std::scoped_lock lock(mutex_serial_port_);

      bytes_read = serial_port_.readSerial(rx_buffer_, SERIAL_BUFFER_SIZE);
    }

    if (bytes_read > 0) {
      // feed all the incoming bytes into the minipix interface
      serialDataCallback(rx_buffer_, bytes_read);
    } else {
      sleep(1);
    }
  }
}

//}

/* update_linux() //{ */

void MinipixDummyLinux::update_linux(void) {
  this->update();
}

//}

/* loadImage() //{ */

std::vector<std::vector<double>> MinipixDummyLinux::loadImage(const std::string& file_path) {

  printf("loading image from '%s'\n", file_path.c_str());

  std::ifstream data(file_path);

  std::vector<std::vector<double>> parsed_csv;

  if (!data.good()) {
    printf("could not load the file %s\n", file_path.c_str());
    return parsed_csv;
  }

  std::string line;

  while (std::getline(data, line)) {

    std::stringstream   lineStream(line);
    std::string         cell;
    std::vector<double> parsedRow;

    while (std::getline(lineStream, cell, ' ')) {
      parsedRow.push_back(stod(cell));
    }

    parsed_csv.push_back(parsedRow);
  }

  return parsed_csv;
}

//}

/* randi() //{ */

int MinipixDummyLinux::randi(const int& from, const int& to) {

  double zero_to_one = double((float)rand()) / double(RAND_MAX);

  return int(floor(to - from) * zero_to_one + from);
}

//}

/* randd() //{ */

double MinipixDummyLinux::randd(const double& from, const double& to) {

  double zero_to_one = double((float)rand()) / double(RAND_MAX);

  return floor(to - from) * zero_to_one + from;
}

//}

/* setDataFolder() //{ */

void MinipixDummyLinux::setDataFolder(const std::string& data_folder) {
  this->data_folder = data_folder;
}

//}

/* simulateFrameAcquisition() //{ */

void MinipixDummyLinux::simulateFrameAcquisition(const uint16_t& acquisition_time) {

  printf("simulating acquisition, time %f s\n", double(acquisition_time) / 1000);

  frame_id_++;

  sleep(acquisition_time);

  // simulate error

  if (randd(0, 1) > 0.9) {

    printf("Error: measurement failed\n");

    sendError(LLCP_MINIPIX_ERROR_MEASUREMENT_FAILED);
    return;
  }

  // send confirmation of the finished measurement

  LLCP_FrameMeasurementFinishedMsg_t msg;
  init_LLCP_FrameMeasurementFinishedMsg_t(&msg);

  hton_LLCP_FrameMeasurementFinishedMsg_t(&msg);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&msg, sizeof(msg), tx_buffer_);
  sendMessageNoAck(tx_buffer_, n_bytes);
}

//}

/* getFrameData() //{ */

void MinipixDummyLinux::getFrameData(void) {

  // select image from the database
  uint16_t image_id = randi(0, 992);
  /* uint16_t image_id = 666; */

  std::stringstream ss;

  ss << data_folder << "/" << image_id << "_fullres.txt";
  std::string filename = ss.str();

  std::vector<std::vector<double>> image = loadImage(filename);

  // initialize the packet we will be sending
  LLCP_FrameDataMsg_t image_data;
  init_LLCP_FrameDataMsg_t(&image_data);

  int n_pixels_counter  = 0;
  int packet_id_counter = 0;

  // | ------------------- fill in the payload ------------------ |

  image_data.payload.frame_id  = frame_id_;
  image_data.payload.mode      = mode_;
  image_data.payload.n_pixels  = 0;
  image_data.payload.packet_id = 0;

  for (size_t j = 0; j < image.size(); j++) {

    for (size_t i = 0; i < image[j].size(); i++) {

      double pixel_value = image[j][i];

      if (pixel_value > 0) {

        LLCP_PixelDataCommon_t* pixel_common = (LLCP_PixelDataCommon_t*)&image_data.payload.pixel_data[n_pixels_counter];
        pixel_common->address                = i + j * 256;

        TPX3PixelMode_t pixel_mode;

        switch (mode_) {

          case LLCP_TPX3_PXL_MODE_TOA_TOT: {

            LLCP_PixelDataToAToT_t* pixel = (LLCP_PixelDataToAToT_t*)(&image_data.payload.pixel_data[n_pixels_counter]);
            pixel->ftoa                   = pixel_value > 0 ? 1 : 0;
            pixel->tot                    = uint8_t(pixel_value);
            pixel->toa                    = 500 - uint8_t(pixel_value) >= 0 ? 500 - uint8_t(pixel_value) : 0;

            pixel_mode = TPX3_TOA_TOT;

            break;
          }

          case LLCP_TPX3_PXL_MODE_TOA: {

            LLCP_PixelDataToA_t* pixel = (LLCP_PixelDataToA_t*)&image_data.payload.pixel_data[n_pixels_counter];
            pixel->ftoa                = pixel_value > 0 ? 1 : 0;
            pixel->toa                 = 500 - uint8_t(pixel_value) >= 0 ? 500 - uint8_t(pixel_value) : 0;

            pixel_mode = TPX3_TOA;

            break;
          }

          case LLCP_TPX3_PXL_MODE_MPX_ITOT: {

            LLCP_PixelDataMpxiToT_t* pixel = (LLCP_PixelDataMpxiToT_t*)&image_data.payload.pixel_data[n_pixels_counter];
            pixel->mpx                     = uint8_t(pixel_value);
            pixel->itot                    = 500 - uint8_t(pixel_value) >= 0 ? 500 - uint8_t(pixel_value) : 0;

            pixel_mode = TPX3_MPX_ITOT;

            break;
          }
        }

        pixel_common->header = 10;

        encodePixelData((uint8_t*)pixel_common, 4, pixel_mode);

        n_pixels_counter++;
      }

      // send the packet when it is full
      if (n_pixels_counter == LLCP_FRAME_DATA_N_PIXELS) {

        image_data.payload.n_pixels  = n_pixels_counter;
        image_data.payload.packet_id = packet_id_counter++;

        // convert to network endian
        hton_LLCP_FrameDataMsg_t(&image_data);

        uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&image_data, sizeof(image_data), tx_buffer_);

        // convert it back, since we are gonna use the structure again
        ntoh_LLCP_FrameDataMsg_t(&image_data);

        sendMessage(tx_buffer_, n_bytes);

        printf("packet %d full, sending\n", image_data.payload.packet_id);

        n_pixels_counter = 0;
      }
    }
  }

  // send the last unfinished packet
  if (n_pixels_counter > 0) {

    image_data.payload.n_pixels  = n_pixels_counter;
    image_data.payload.packet_id = packet_id_counter++;

    // convert to network endian
    hton_LLCP_FrameDataMsg_t(&image_data);

    uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&image_data, sizeof(image_data), tx_buffer_);

    // convert it back, since we are gonna use the structure again
    ntoh_LLCP_FrameDataMsg_t(&image_data);

    sendMessage(tx_buffer_, n_bytes);

    printf("sent the last packet (%d)\n", packet_id_counter);

    n_pixels_counter = 0;
  }

  // | ---------------- send FrameDataTerminator ---------------- |

  // create the message
  LLCP_FrameDataTerminatorMsg_t terminator;
  init_LLCP_FrameDataTerminatorMsg_t(&terminator);

  terminator.payload.frame_id  = image_data.payload.frame_id;
  terminator.payload.n_packets = packet_id_counter;

  hton_LLCP_FrameDataTerminatorMsg_t(&terminator);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&terminator, sizeof(terminator), tx_buffer_);

  sendMessage(tx_buffer_, n_bytes);

  printf("Simulated acquisition finished\n");
}

//}
