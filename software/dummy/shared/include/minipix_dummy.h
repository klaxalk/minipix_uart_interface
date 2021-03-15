#ifndef MINIPIX_DUMMY_H
#define MINIPIX_DUMMY_H

#include <stdint.h>

extern "C" {
#include <llcp.h>
#include <llcp_minipix_messages.h>
}

class MinipixDummy {

public:
  MinipixDummy();

  virtual void sendByte([[maybe_unused]] const uint8_t &byte_out){};
  virtual void sendString([[maybe_unused]] const uint8_t *bytes_out, [[maybe_unused]] const uint16_t &len){};
  void         serialDataCallback(const uint8_t *bytes_in, const uint16_t &len);

  virtual void update(void){};

protected:
  LLCP_Receiver_t llcp_receiver_;

  uint16_t boot_count_ = 0;

private:
  uint8_t tx_buffer_[LLCP_RX_TX_BUFFER_SIZE];
};

#endif  // MINIPIX_DUMMY_H
