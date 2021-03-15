#ifndef MINIPIX_DUMMY_H
#define MINIPIX_DUMMY_H

#include <stdint.h>

#include <llcp.h>

class MinipixDummy {

public:
  MinipixDummy();

  virtual void sendByte([[maybe_unused]] const uint8_t &byte_out){};
  virtual void sendString([[maybe_unused]] const uint8_t &byte_out, [[maybe_unused]] const uint16_t &len){};

  virtual void update(void){};

protected:
  LLCP_Receiver_t llcp_receiver_;

  uint16_t boot_count_ = 0;
};

// constructor
inline MinipixDummy::MinipixDummy() {

  llcpInitialize(&llcp_receiver_);

  printf("pes\n");
}

#endif  // MINIPIX_DUMMY_H