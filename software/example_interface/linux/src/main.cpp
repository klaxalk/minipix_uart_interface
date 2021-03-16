#include <minipix_interface_linux.h>

int main() {

  // | ----------- initialize the MiniPIX serial port ----------- |

  bool serial_port_minipix_virtual = true;
  serial_port_minipix_.connect("/tmp/ttyS1", 115200, serial_port_minipix_virtual);

  uint8_t rx_buffer_minipix[SERIAL_BUFFER_SIZE];

  // | ------------ initialize the lander serial port ----------- |

  bool serial_port_lander_virtual = true;
  serial_port_lander_.connect("/tmp/ttyS2", 115200, serial_port_lander_virtual);

  uint8_t rx_buffer_lander[SERIAL_BUFFER_SIZE];

  llcp_initialize(&llcp_receiver_lander);

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

    /* mui_measureFrame(&mui_handler, 13); */

    {
      // | --------- receive data from the minipix interface -------- |

      uint16_t bytes_read = serial_port_minipix_.readSerial(rx_buffer_minipix, SERIAL_BUFFER_SIZE);

      // feed all the incoming bytes into the minipix interface
      for (uint16_t i = 0; i < bytes_read; i++) {
        mui_receiveCharCallback(&mui_handler, rx_buffer_minipix[i]);
      }
    }

    // | -------------- receive data from the lander -------------- |

    {
      uint16_t bytes_read = serial_port_lander_.readSerial(rx_buffer_lander, SERIAL_BUFFER_SIZE);

      // feed all the incoming bytes into the minipix interface
      for (uint16_t i = 0; i < bytes_read; i++) {

        LLCP_Message_t message_in;

        if (llcp_processChar(rx_buffer_lander[i], &llcp_receiver_lander, &message_in)) {

          switch ((LLCP_MessageId_t)message_in.id) {

            case LLCP_GET_STATUS_MSG_ID: {

              mui_getStatus(&mui_handler);

              printf("received request for status\n");

              break;
            };

            case LLCP_MEASURE_FRAME_MSG_ID: {

              LLCP_MeasureFrameReqMsg_t* msg = (LLCP_MeasureFrameReqMsg_t*)&message_in.payload;
              ntoh_LLCP_MeasureFrameReqMsg_t(msg);
              MeasureFrameReq_t* req = (MeasureFrameReq_t*)&msg->payload;

              mui_measureFrame(&mui_handler, req->acquisition_time_ms);

              printf("received request for a frame\n");

              break;
            };


            default: {

              printf("Received unsupported message with id = %d\n", message_in.id);
            }
          }
        }
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  return 0;
}
