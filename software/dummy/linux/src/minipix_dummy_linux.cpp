#include <minipix_dummy_linux.h>

/* constructor MinipixDummyLinux() //{ */

MinipixDummyLinux::MinipixDummyLinux(void) {

  thread_serial_port_ = std::thread(&MinipixDummyLinux::threadSerialPort, this);
  /* thread_serial_port_.join(); */
}

//}

/* initializeSerialPort() //{ */

void MinipixDummyLinux::initializeSerialPort(const std::string& file, const int& baud, const bool virtual_port) {

  serial_port_.connect(file, baud, virtual_port);
  serial_port_initialized_ = true;
}

//}

// | ------------ virtual functions --- HW-specific ----------- |

/* sendByte() //{ */

void MinipixDummyLinux::sendByte(const uint8_t& byte_out) {

  if (!serial_port_.sendCharArray((unsigned char*)&byte_out, 1)) {
    printf("FAILED sending message with %d bytes\n", 1);
  }
}

//}

/* sendByte() //{ */

void MinipixDummyLinux::sleep(const uint16_t& milliseconds) {

  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

//}

/* sendString() //{ */

void MinipixDummyLinux::sendString(const uint8_t* bytes_out, const uint16_t& len) {

  if (!serial_port_.sendCharArray((unsigned char*)bytes_out, len)) {
    printf("FAILED sending message with %d bytes\n", len);
  }
}

//}

/* threadSerialPort() //{ */

void MinipixDummyLinux::threadSerialPort(void) {

  printf("waiting for serial\n");

  while (!serial_port_initialized_) {
    sleep(100);
  }

  printf("starting serial thread\n");

  while (true) {

    // | --------- receive data from the minipix interface -------- |

    uint16_t bytes_read = serial_port_.readSerial(rx_buffer_, SERIAL_BUFFER_SIZE);

    if (bytes_read > 0) {
      // feed all the incoming bytes into the minipix interface
      serialDataCallback(rx_buffer_, bytes_read);
    } else {
      sleep(1);
    }
  }
}

//}

/* update() //{ */

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

  return floor(to - from) * zero_to_one + from;
}

//}

/* setDataFolder() //{ */

void MinipixDummyLinux::setDataFolder(const std::string& data_folder) {
  this->data_folder = data_folder;
}

//}

/* simulateImageAcquisition() //{ */

void MinipixDummyLinux::simulateImageAcquisition(const uint16_t& acquisition_time) {

  printf("simulating acquisition, time %f s\n", double(acquisition_time) / 1000);

  sleep(acquisition_time);

  // select image from the database
  uint16_t image_id = randi(0, 992);
  /* uint16_t image_id = 998; */

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

  image_data.payload.frame_id  = frame_id_++;
  image_data.payload.mode      = LLCP_TPX3_PXL_MODE_TOA_TOT;
  image_data.payload.n_pixels  = 0;
  image_data.payload.packet_id = 0;

  for (size_t j = 0; j < image.size(); j++) {

    for (size_t i = 0; i < image[j].size(); i++) {

      double pixel_value = image[j][i];

      if (pixel_value > 0) {

        LLCP_PixelDataToAToT_t* pixel = (LLCP_PixelDataToAToT_t*)&image_data.payload.pixel_data[n_pixels_counter++];
        pixel->address                = i + j * 256;
        pixel->ftoa                   = 0;
        pixel->tot                    = uint8_t(pixel_value);
        pixel->toa                    = 500 - int(pixel_value) >= 0 ? 500 - int(pixel_value) : 0;

        // pixel->mode_mask = // TODO
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
}

//}
