#include <minipix_interface_linux.h>

#define SERIAL_BUFFER_SIZE 2048

int main() {

  // | ----------- initialize the MiniPIX serial port ----------- |

  bool serial_port_minipix_virtual = true;
  serial_port_minipix_.connect("/tmp/ttyS1", 115200, serial_port_minipix_virtual);

  uint8_t  read_buffer[SERIAL_BUFFER_SIZE];
  uint16_t bytes_read;

  // | ------------ initialize the lander serial port ----------- |

  bool serial_port_lander_virtual = true;
  serial_port_lander_.connect("/tmp/ttyS3", 115200, serial_port_lander_virtual);

  // | -------- initialize the MiniPIX interface library -------- |

  MUI_Handler_t mui_handler;

  mui_handler.fcns.ledSetHW           = &mui_ledSetHW;
  mui_handler.fcns.processImagePacket = &mui_processImagePacket;
  mui_handler.fcns.processStatus      = &mui_processStatus;
  mui_handler.fcns.sendChar           = &mui_sendChar;
  mui_handler.fcns.sendString         = &mui_sendString;

  mui_initialize(&mui_handler);

  printf("Example interface started\n");

  while (true) {

    mui_measureFrame(&mui_handler, 13);

    mui_getStatus(&mui_handler);

    // | --------- receive data from the minipix interface -------- |

    bytes_read = serial_port_minipix_.readSerial(read_buffer, SERIAL_BUFFER_SIZE);

    // feed all the incoming bytes into the minipix interface
    for (uint16_t i = 0; i < bytes_read; i++) {
      mui_receiveCharCallback(&mui_handler, read_buffer[i]);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  return 0;
}
