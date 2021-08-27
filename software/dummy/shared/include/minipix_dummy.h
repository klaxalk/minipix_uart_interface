#ifndef MINIPIX_DUMMY_H
#define MINIPIX_DUMMY_H

#include <atomic>
#include <list>
#include <mutex>

#include <stdint.h>

#include <llcp.h>
#include <llcp_minipix_messages.h>

#include <pixel_encoder.h>

class MinipixDummy {

public:
  MinipixDummy();

  virtual void sendByte([[maybe_unused]] const uint8_t &byte_out){};
  virtual void sendString([[maybe_unused]] const uint8_t *bytes_out, [[maybe_unused]] const uint16_t &len){};
  void         serialDataCallback(const uint8_t *bytes_in, const uint16_t &len);
  virtual void sleep([[maybe_unused]] const uint16_t &milliseconds){};
  virtual void getFrameData(void){};
  virtual void simulateFrameAcquisition([[maybe_unused]] const uint16_t &acquisition_time){};

protected:
  LLCP_Receiver_t llcp_receiver_;

  uint16_t boot_count_ = 0;
  uint16_t frame_id_   = 41;  // it has to start somewhere, let it be 41
  uint8_t  mode_       = 0; // current pixel mode, LLCP_TPX3_PXL_MODE_{TOA_TOT,TOA,MPX_ITOT}

  void update(void);

  uint8_t tx_buffer_[LLCP_RX_TX_BUFFER_SIZE];

  void sendMessage([[maybe_unused]] const uint8_t *bytes_out, [[maybe_unused]] const uint16_t &len);
  void sendMessageNoAck([[maybe_unused]] const uint8_t *bytes_out, [[maybe_unused]] const uint16_t &len);
  void sendAck(void);
  void sendError(const uint8_t &id);

private:
  // send an image with a diagonal test stripe
  void simulatedTestStripeAcquisition();

  void              clearToSend(void);
  std::atomic<bool> clear_to_send_ = true;

  std::list<LLCP_Message_t> message_buffer_;
  std::mutex                mutex_message_buffer_;

  std::atomic<bool> powered_ = false;
};

#endif  // MINIPIX_DUMMY_H
