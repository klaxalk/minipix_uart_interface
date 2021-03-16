#include <minipix_interface_linux.h>

SerialPort serial_port_minipix_;
SerialPort serial_port_lander_;

/* mui_sleepHW() //{ */

void mui_sleepHW(const uint16_t milliseconds) {

  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

//}

/* mui_sendChar() //{ */

void mui_sendChar(const uint8_t char_out) {

  serial_port_minipix_.sendChar(char_out);
}

//}

/* mui_sendString() //{ */

void mui_sendString(const uint8_t *str_out, const uint16_t len) {

  if (!serial_port_minipix_.sendCharArray((unsigned char *)str_out, len)) {
    printf("FAILED sending message with %d bytes\n", len);
  }
}

//}

/* mui_processImagePacket() //{ */

void mui_processImagePacket(const ImageData_t *image_data) {

  uint8_t n_pixels = image_data->n_pixels;

  printf("received image data, n_pixels %d, last_pixel_x: %d\n", n_pixels, image_data->pixel_data[n_pixels - 1].x_coordinate);
}

//}

/* mui_processStatus() //{ */

void mui_processStatus(const Status_t *status) {

  printf("received status: boot count = %d, string: '%s'\n", status->boot_count, status->status_str);
}

//}

/* mui_ledSetHW() //{ */

void mui_ledSetHW([[maybe_unused]] bool state) {
  // do nothing, we don't have LEDs on LINUX
}

//}
