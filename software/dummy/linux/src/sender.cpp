#define LLCP_LITTLE_ENDIAN

#include <llcp/llcp.h>
#include <llcp/llcp_minipix_messages.h>
#include <serial_port.h>

#include <thread>
#include <chrono>
#define SERIAL_BUFFER_SIZE 2048

int main() {

  // the hardware serial driver
  SerialPort serial_port;
  serial_port.connect("/tmp/ttyS1", 115200, true);

  printf("Sender started\n");

  while (true) {

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    {
      uint8_t n_pixels = 31;

      ImageDataMsg_t test_data;

      test_data.image_data.n_pixels = n_pixels;

      for (int i = 0; i < n_pixels; i++) {

        PixelData_t* pixel  = (PixelData_t*)&test_data.image_data.pixel_data[i];
        pixel->x_coordinate = i;
        pixel->y_coordinate = i;
        pixel->data[0]      = i;
        pixel->data[1]      = i;
        pixel->data[2]      = i;
        pixel->data[3]      = i;
        pixel->data[4]      = i;
        pixel->data[5]      = i;
      }

      uint8_t  tx_buffer[SERIAL_BUFFER_SIZE];
      uint16_t n_bytes = llcpPrepareMessage((uint8_t*)&test_data, sizeof(test_data), tx_buffer);

      if (serial_port.sendCharArray(tx_buffer, n_bytes)) {
        printf("message with %d bytes was sent\n", n_bytes);
      } else {
        printf("FAILED sending message with %d bytes\n", n_bytes);
      }
    }

    {
      GetFrameMsg_t get_frame;
      get_frame.acquisition_time_ms = 1213;
      hton(&get_frame);

      uint8_t  tx_buffer[SERIAL_BUFFER_SIZE];
      uint16_t n_bytes = llcpPrepareMessage((uint8_t*)&get_frame, sizeof(get_frame), tx_buffer);

      if (serial_port.sendCharArray(tx_buffer, n_bytes)) {
        printf("message with %d bytes was sent\n", n_bytes);
      } else {
        printf("FAILED sending message with %d bytes\n", n_bytes);
      }
    }
  }

  return 0;
}
