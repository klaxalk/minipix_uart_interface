#ifndef BACA_PROTOCOL_H
#define BACA_PROTOCOL_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define MAX_PAYLOAD_LEN 128
#define BUFFER_SIZE MAX_PAYLOAD_LEN + 1 + 1 + 1

enum Receiver_State_t
{
  WAITING_FOR_MESSSAGE = 0,
  EXPECTING_SIZE       = 1,
  EXPECTING_PAYLOAD    = 2,
  EXPECTING_CHECKSUM   = 3,
};

typedef struct
{
  Receiver_State_t state;
  uint8_t          payload_size;
  uint8_t          input_buffer[BUFFER_SIZE];
  uint8_t          buffer_counter;
  uint8_t          checksum;
} Receiver_t;

typedef struct
{
  uint8_t id;
  uint8_t payload_size;
  uint8_t payload[MAX_PAYLOAD_LEN];
} Message_t;

/* llcpInitialize() //{ */

inline void llcpInitialize(Receiver_t* receiver) {

  receiver->checksum = 0;
  receiver->buffer_counter = 0;
  receiver->state = WAITING_FOR_MESSSAGE;
}

//}

/* llcpProcessChar() //{ */

inline bool llcpProcessChar(const uint8_t char_in, Receiver_t* receiver, Message_t* message) {

  printf("char in %c\n", char_in);

  switch (receiver->state) {

    case WAITING_FOR_MESSSAGE: {

      if (char_in == 'b') {

        printf("got b\n");

        receiver->checksum       = char_in;
        receiver->buffer_counter = 0;
        receiver->state          = EXPECTING_SIZE;
      }

      break;
    }

    case EXPECTING_SIZE: {

      if (char_in == 0) {
        receiver->state = WAITING_FOR_MESSSAGE;
      } else {
        receiver->payload_size = char_in;
        receiver->checksum += char_in;
        receiver->state = EXPECTING_PAYLOAD;

        printf("getting size %d\n", char_in);
      }

      break;
    }

    case EXPECTING_PAYLOAD: {

      printf("getting payload\n");

      receiver->input_buffer[receiver->buffer_counter] = char_in;
      receiver->checksum += char_in;
      receiver->buffer_counter++;

      if (receiver->buffer_counter >= receiver->payload_size) {
        receiver->state = EXPECTING_CHECKSUM;
      }

      break;
    }

    case EXPECTING_CHECKSUM: {

      printf("waiting for checksum\n");

      if (receiver->checksum == char_in) {

        memcpy(&(message->payload), &(receiver->input_buffer), receiver->payload_size);
        message->payload_size = receiver->payload_size;

        receiver->state = WAITING_FOR_MESSSAGE;
        return true;
      }

      break;
    }
  }

  return false;
}

//}

/*  llcpPrepareMessage() //{ */

inline void llcpPrepareMessage(uint8_t* what, uint8_t len, Message_t* where) {

  uint8_t  payload_size = len;
  uint8_t  checksum     = 0;
  uint16_t it           = 0;

  where->payload[it++] = 'b';
  checksum += 'b';
  where->payload[it++] = payload_size;
  checksum += payload_size;

  for (int i = 0; i < payload_size; i++) {
    where->payload[it++] = what[i];
    checksum += what[i];
  }

  where->payload[it] = checksum;

  where->payload_size = payload_size + 3;
}

//}

#endif  // BACA_PROTOCOL_H
