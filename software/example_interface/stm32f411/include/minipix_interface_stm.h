#ifndef EXAMPLE_INTERFACE_STM_H
#define EXAMPLE_INTERFACE_STM_H

// include the "virtual" example interface
#include <minipix_interface.hpp>

// include the Hardware-specific header
#include <main.h>

class MinipixInterfaceSTM : public MinipixInterface {

public:
  MinipixInterfaceSTM();

  void setMinipixUart(UART_HandleTypeDef &huart_minipix);

private:
  UART_HandleTypeDef *huart_minipix_ptr_;

  void ledSetHW(const bool &new_state);
  void sleepHW(const int &milliseconds);
  void minipixSendChar(const char &char_out);
  void minipixSendString(const char *str_out, const char &len);
};

#endif  // EXAMPLE_INTERFACE_STM_H
