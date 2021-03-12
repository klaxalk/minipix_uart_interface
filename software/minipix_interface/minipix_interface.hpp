#ifndef EXAMPLE_INTERFACE_HPP
#define EXAMPLE_INTERFACE_HPP

#include <string>
#include <string.h>
#include <list>

// definition of packet structures of the measured data
#include <data_definitions.h>

/* class MinipixInterface //{ */

class MinipixInterface {

public:
  MinipixInterface(void);

  // will be called by the integrator to drive the logic of the interface
  void update(void);

  // will be called by the integrator to pass incoming UART data
  void minipixReceiveCharCallback(const char &char_in);

  // initiate acquisition
  void getFrame(const int16_t &acquisition_time);

  // get status, to be called by the integrator
  Status_t getStatus(void);

  // mask pixel, to be called by the integrator
  void getStatus(const PixelCoordinates_t &pixel);

private:

  // | --------------------- LED signalling --------------------- |

  void ledToggle(void);
  void ledSet(const bool &new_state);
  bool led_state_ = false;

  virtual void ledSetHW(const bool &new_state){};

  // | -------------------------- USART ------------------------- |

  virtual void       minipixSendChar(const char &char_out){};
  virtual void       minipixSendString(const char *str_out, const char &len){};
  std::list<uint8_t> minipix_receive_buffer_;

  // | ----------- virtual callbacks of the interface ----------- |

  // will be called when image packet is ready to be processed by the integrator
  virtual void processImagePacket(const ImagePacket_t &image_packet){};

  // | --------------------- other routines --------------------- |

  virtual void sleepHW(const int &milliseconds){};

};

//}

/* constructor MinipixInterface //{ */

inline MinipixInterface::MinipixInterface(void) {
}

//}

// | --------------- the main update() function --------------- |

/* update() //{ */

void inline MinipixInterface::update(void) {

  if (!minipix_receive_buffer_.empty()) {

    // TODO process chars from the buffer

  } else {

    ledToggle();
    sleepHW(100);
  }
}

//}

// | ------------- UART communication with MiniPIX ------------ |

/* minipixReceiveCharCallback() //{ */

inline void MinipixInterface::minipixReceiveCharCallback(const char &char_in) {

  minipix_receive_buffer_.push_back(char_in);
}

//}

// | -------------------- LED signalization ------------------- |

/* ledToggle() //{ */

inline void MinipixInterface::ledToggle(void) {

  ledSet(!led_state_);
}

//}

/* ledSet() //{ */

inline void MinipixInterface::ledSet(const bool &new_state) {

  ledSetHW(new_state);

  led_state_ = new_state;
}

//}

#endif  // EXAMPLE_INTERFACE_HPP
