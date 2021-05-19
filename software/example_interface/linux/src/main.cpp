#include <gatherer_interface_linux.h>
#include <minipix_interface_linux.h>

std::string serial_port_minipix;
int         baud_rate_minipix;
bool        serial_port_minipix_virtual;

std::string serial_port_gatherer;
int         baud_rate_gatherer;
bool        serial_port_gatherer_virtual;

int main(int argc, char *argv[]) {

  if (argc == 7) {
    serial_port_minipix         = argv[1];
    baud_rate_minipix           = atoi(argv[2]);
    serial_port_minipix_virtual = argv[3];

    serial_port_gatherer         = argv[4];
    baud_rate_gatherer           = atoi(argv[5]);
    serial_port_gatherer_virtual = argv[6];

    printf("loaded params:\n");
    printf("minipix: %s, %d, %s\n", serial_port_minipix.c_str(), baud_rate_minipix, serial_port_minipix_virtual ? "VIRTUAL" : "REAL");
    printf("gatherer: %s, %d, %s\n", serial_port_gatherer.c_str(), baud_rate_gatherer, serial_port_gatherer_virtual ? "VIRTUAL" : "REAL");
  } else {
    printf("params not supplied\n");
    return 0;
  }

  // open the serial ports
  serial_port_minipix_.connect(serial_port_minipix, baud_rate_minipix, serial_port_minipix_virtual);
  serial_port_gatherer_.connect(serial_port_gatherer, baud_rate_gatherer, serial_port_gatherer_virtual);

  // | -------- initialize the MiniPIX interface library -------- |

  MUI_Handler_t mui_handler;

  mui_handler.fcns.ledSetHW                   = &mui_linux_ledSetHW;
  mui_handler.fcns.sleepHW                    = &mui_linux_sleepHW;
  mui_handler.fcns.processFrameData           = &mui_linux_processFrameData;
  mui_handler.fcns.processFrameDataTerminator = &mui_linux_processFrameDataTerminator;
  mui_handler.fcns.processStreamData          = &mui_linux_processStreamData;
  mui_handler.fcns.processStatus              = &mui_linux_processStatus;
  mui_handler.fcns.processTemperature         = &mui_linux_processTemperature;
  mui_handler.fcns.processAck                 = &mui_linux_processAck;
  mui_handler.fcns.processMinipixError        = &mui_linux_processMinipixError;
  mui_handler.fcns.sendChar                   = &mui_linux_sendChar;
  mui_handler.fcns.sendString                 = &mui_linux_sendString;

  mui_initialize(&mui_handler);

  // | -------- initialize the interface to the Gatherer -------- |

  // pass the platform-specific gatherer functions to the gatherer
  // handler, so it can call them to communicate
  gatherer_handler_.fcns.sendChar   = &gatherer_linux_sendChar;
  gatherer_handler_.fcns.sendString = &gatherer_linux_sendString;

  // gatherer wants the pointer to the mui_handler, so it can call
  // its methods, regardless of the platform.
  gatherer_handler_.mui_handler_ptr_ = &mui_handler;

  gatherer_initialize(&gatherer_handler_);

  printf("Linux example MUI started\n");

  while (true) {

    {
      // | --------- receive data from the minipix interface -------- |

      uint8_t  buffer[SERIAL_BUFFER_SIZE];
      uint16_t bytes_read = serial_port_minipix_.readSerial(buffer, SERIAL_BUFFER_SIZE);

      // feed all the incoming bytes (from the MiniPIX) into the MUI
      for (uint16_t i = 0; i < bytes_read; i++) {
        mui_receiveCharCallback(&mui_handler, buffer[i]);
      }
    }

    // | -------------- receive data from the lander -------------- |

    {

      uint8_t  buffer[SERIAL_BUFFER_SIZE];
      uint16_t bytes_read = serial_port_gatherer_.readSerial(buffer, SERIAL_BUFFER_SIZE);

      // feed all the incoming bytes (From PC) into the gatherer interface
      for (uint16_t i = 0; i < bytes_read; i++) {
        gatherer_receiveCharCallback(&gatherer_handler_, buffer[i]);
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  return 0;
}
