#ifndef EXAMPLE_INTERFACE_H
#define EXAMPLE_INTERFACE_H

// definition of packet structures of the measured data
#include <data_definitions.h>
#include <stdbool.h>

/* extern void ledSet(const bool new_state); */
bool led_state_;

// initiate acquisition
void getFrame(const uint16_t acquisition_time);

// get status, to be called by the integrator
Status_t getStatus(void);

// mask pixel, to be called by the integrator
void maskPixel(const PixelCoordinates_t pixel);

// | --------------------- LED signalling --------------------- |

extern void ledSetHW(const bool new_state);

// | -------------------------- USART ------------------------- |

extern void minipixSendChar(const char char_out);
extern void minipixSendString(const char *str_out, const char len);

// | ----------- virtual callbacks of the interface ----------- |

// will be called when image packet is ready to be processed by the integrator
extern void processImagePacket(const ImagePacket_t image_packet);

// | --------------------- other routines --------------------- |

extern void sleepHW(const int milliseconds);

//}

// | ------------- UART communication with MiniPIX ------------ |

/* minipixReceiveCharCallback() //{ */

inline void minipixReceiveCharCallback(const char char_in) {

}

//}

// | -------------------- LED signalization ------------------- |

/* ledSet() //{ */

inline void ledSet(const bool new_state) {

  ledSetHW(new_state);

  led_state_ = new_state;
}

//}

/* ledToggle() //{ */

inline void ledToggle(void) {

  ledSet(!led_state_);
}

//}

// | --------------- the main update() function --------------- |

/* update() //{ */

void inline update(void) {

  // TODO process chars from the buffer

  ledToggle();
  sleepHW(100);
}

//}

#endif  // EXAMPLE_INTERFACE_H
