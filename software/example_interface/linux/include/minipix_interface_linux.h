#ifndef MINIPIX_INTERFACE_LINUX_H
#define MINIPIX_INTERFACE_LINUX_H

#include <chrono>
#include <thread>

#include <serial_port.h>

#include <minipix_interface.h>

SerialPort serial_port_minipix_;
SerialPort serial_port_lander_;

/* minipix_interface_sleepHW() //{ */

inline void minipix_interface_sleepHW(const uint16_t milliseconds) {

  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

//}

/* minipix_interface_minipixSendChar() //{ */

inline void minipix_interface_minipixSendChar(const uint8_t char_out) {

  serial_port_minipix_.sendChar(char_out);
}

//}

/* minipix_interface_minipixSendString() //{ */

inline void minipix_interface_minipixSendString(const uint8_t *str_out, const uint16_t len) {

  if (!serial_port_minipix_.sendCharArray((unsigned char *)str_out, len)) {
    printf("FAILED sending message with %d bytes\n", len);
  }
}

//}

/* minipix_interface_processImagePacket() //{ */

inline void minipix_interface_processImagePacket(const ImageData_t *image_data) {

  uint8_t n_pixels = image_data->n_pixels;

  printf("received image data, n_pixels %d, last_pixel_x: %d\n", n_pixels, image_data->pixel_data[n_pixels - 1].x_coordinate);
}

//}

/* minipix_interface_processStatus() //{ */

inline void minipix_interface_processStatus(const Status_t *status) {

  printf("received status: boot count = %d, string: '%s'\n", status->boot_count, status->status_str);
}

//}

#endif  // MINIPIX_INTERFACE_LINUX_H
