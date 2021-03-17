#include <gatherer_interface_linux.h>
#include <minipix_interface_linux.h>

int main() {

  // | ----------- initialize the MiniPIX serial port ----------- |

  bool serial_port_minipix_virtual = true;
  serial_port_minipix_.connect("/tmp/ttyS1", 115200, serial_port_minipix_virtual);

  // | ------------ initialize the lander serial port ----------- |

  bool serial_port_lander_virtual = true;
  serial_port_gatherer_.connect("/tmp/ttyS2", 115200, serial_port_lander_virtual);

  // | -------- initialize the MiniPIX interface library -------- |

  MUI_Handler_t mui_handler;

  mui_handler.fcns.ledSetHW           = &mui_ledSetHW;
  mui_handler.fcns.processImagePacket = &mui_processImagePacket;
  mui_handler.fcns.processStatus      = &mui_processStatus;
  mui_handler.fcns.sendChar           = &mui_sendChar;
  mui_handler.fcns.sendString         = &mui_sendString;

  mui_initialize(&mui_handler);

  // | -------- initialize the interface to the Gatherer -------- |

  // hw support
  gatherer_handler_.fcns.sendChar   = &gatherer_sendChar;
  gatherer_handler_.fcns.sendString = &gatherer_sendString;
  // gatherer->minipix
  gatherer_handler_.fcns.getStatus    = &gatherer_getStatus;
  gatherer_handler_.fcns.measureFrame = &gatherer_measureFrame;

  gatherer_handler_.mui_handler_ptr_ = &mui_handler;

  gatherer_initialize(&gatherer_handler_);

  printf("Example interface started\n");

  while (true) {

    {
      // | --------- receive data from the minipix interface -------- |

      uint8_t  buffer[SERIAL_BUFFER_SIZE];
      uint16_t bytes_read = serial_port_minipix_.readSerial(buffer, SERIAL_BUFFER_SIZE);

      // feed all the incoming bytes into the minipix interface
      for (uint16_t i = 0; i < bytes_read; i++) {
        mui_receiveCharCallback(&mui_handler, buffer[i]);
      }
    }

    // | -------------- receive data from the lander -------------- |

    {

      uint8_t  buffer[SERIAL_BUFFER_SIZE];
      uint16_t bytes_read = serial_port_gatherer_.readSerial(buffer, SERIAL_BUFFER_SIZE);

      // feed all the incoming bytes into the minipix interface
      for (uint16_t i = 0; i < bytes_read; i++) {
        gatherer_receiveCharCallback(&gatherer_handler_, buffer[i]);
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  return 0;
}
