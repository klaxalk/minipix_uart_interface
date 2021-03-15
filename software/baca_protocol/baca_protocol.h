#ifndef BACA_PROTOCOL_H
#define BACA_PROTOCOL_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>

// | ------------------- BEGIN: USER CONFIG ------------------- |

// If COMM_HEXADECIMAL = 1, the protocol encodes data into HEXADECIMAL
// ASCII characters, which do not spooke serial line drivers. It is also
// much easier to debug, if all the data is human-readible.
#define COMM_HEXADECIMAL 0

// should we send '\n' after each packet?
#define APPEND_ENDL 0

// max payload length in bytes, max 255
#define MAX_PAYLOAD_LEN 255

// | -------------------- END: USER CONFIG -------------------- |

// definition of the byte lenghts of the various section of the comm message
#define INIT_LEN 1
#define PAYLOAD_SIZE_LEN 1
#define CHECKSUM_LEN 1

#ifdef APPEND_ENDL
#define ENDL_LEN 1
#else
#define ENDL_LEN 0
#endif

// the total max size of the message defines the size of the buffer we need
#if COMM_HEXADECIMAL == 0
#define LLCP_RX_TX_BUFFER_SIZE INIT_LEN + PAYLOAD_SIZE_LEN + MAX_PAYLOAD_LEN + CHECKSUM_LEN + ENDL_LEN
#else
#define LLCP_RX_TX_BUFFER_SIZE INIT_LEN + (PAYLOAD_SIZE_LEN + MAX_PAYLOAD_LEN + CHECKSUM_LEN) * 2 + ENDL_LEN
#endif

/* #define DEBUG_PRINT 1 */

// | ------------------------- structs ------------------------ |

/* struct Receiver_State_t //{ */

#if COMM_HEXADECIMAL == 0
enum Receiver_State_t
{
  WAITING_FOR_MESSSAGE = 0,
  EXPECTING_SIZE       = 1,
  EXPECTING_PAYLOAD    = 2,
  EXPECTING_CHECKSUM   = 3,
};
#else
enum Receiver_State_t
{
  WAITING_FOR_MESSSAGE = 0,
  EXPECTING_SIZE       = 1,
  EXPECTING_SIZE_2     = 2,
  EXPECTING_PAYLOAD    = 3,
  EXPECTING_CHECKSUM   = 4,
  EXPECTING_CHECKSUM_2 = 5,
};
#endif

//}

/* struct Receiver_t //{ */

#if COMM_HEXADECIMAL == 0
typedef struct
{
  Receiver_State_t state;
  uint16_t         payload_size;
  uint8_t          rx_buffer[LLCP_RX_TX_BUFFER_SIZE];
  uint16_t         buffer_counter;
  uint8_t          checksum;
} Receiver_t;
#else
typedef struct
{
  Receiver_State_t state;
  uint16_t         payload_size;
  uint8_t          rx_buffer[LLCP_RX_TX_BUFFER_SIZE];
  uint16_t         buffer_counter;
  uint8_t          checksum;
  uint8_t          hexmem;  // for remembering the first one-byt of the 2-byte HEX byte
} Receiver_t;
#endif

//}

/* struct Message_t //{ */

typedef struct
{
  uint8_t id;
  uint8_t payload_size;
  uint8_t payload[MAX_PAYLOAD_LEN];
} Message_t;

//}

// | ----------------------- hex <-> bin ---------------------- |

/* hex2bin() //{ */

inline uint8_t hex2bin(const uint8_t* ptr) {

  uint8_t value = 0;
  uint8_t ch    = *ptr;

  for (uint8_t i = 0; i < 2; i++) {

    if (ch >= '0' && ch <= '9')
      value = (value << 4) + (ch - '0');
    else if (ch >= 'A' && ch <= 'F')
      value = (value << 4) + (ch - 'A' + 10);
    else if (ch >= 'a' && ch <= 'f')
      value = (value << 4) + (ch - 'a' + 10);
    else
      return value;
    ch = *(++ptr);
  }

  return value;
}

//}

/* bin2hex() //{ */

inline void bin2hex(const uint8_t byte, uint8_t* buffer) {

  sprintf((char*)buffer, "%02X", byte);
}

//}

// | ------------------------ routines ------------------------ |

/* llcpInitialize() //{ */

inline void llcpInitialize(Receiver_t* receiver) {

  receiver->checksum       = 0;
  receiver->buffer_counter = 0;
  receiver->state          = WAITING_FOR_MESSSAGE;
}

//}

/* llcpProcessChar() //{ */

inline bool llcpProcessChar(const uint8_t char_in, Receiver_t* receiver, Message_t* message) {

#ifdef DEBUG_PRINT
  printf("got char '%c'\n", char_in);
#endif

  switch (receiver->state) {

    case WAITING_FOR_MESSSAGE: {

      if (char_in == 'b') {

#ifdef DEBUG_PRINT
        printf("got 'b'\n");
#endif

        receiver->checksum       = char_in;
        receiver->buffer_counter = 0;
        receiver->state          = EXPECTING_SIZE;
      }

      break;
    }

    case EXPECTING_SIZE: {

#if COMM_HEXADECIMAL == 0
      uint8_t payload_size = char_in;

      if (payload_size == 0) {
        receiver->state = WAITING_FOR_MESSSAGE;
      } else {
        receiver->payload_size = payload_size;
        receiver->state        = EXPECTING_PAYLOAD;
        receiver->checksum += payload_size;

#ifdef DEBUG_PRINT
        printf("got size = %d bytes\n", payload_size);
#endif
      }
#else
      receiver->hexmem = char_in;
      receiver->state  = EXPECTING_SIZE_2;
      receiver->checksum += char_in;
#endif

      break;
    }

#if COMM_HEXADECIMAL == 1
    case EXPECTING_SIZE_2: {

      uint8_t minibuf[2];
      minibuf[0] = receiver->hexmem;
      minibuf[1] = char_in;

      uint16_t payload_size = hex2bin(minibuf) * 2;

      if (payload_size == 0) {
        receiver->state = WAITING_FOR_MESSSAGE;
      } else {
        receiver->payload_size = payload_size;
        receiver->state        = EXPECTING_PAYLOAD;
        receiver->checksum += char_in;

#ifdef DEBUG_PRINT
        printf("got size = %d bytes\n", payload_size);
#endif
      }

      break;
    }
#endif

    case EXPECTING_PAYLOAD: {

#ifdef DEBUG_PRINT
      printf("getting payload %d/%d\n", receiver->buffer_counter + 1, receiver->payload_size);
#endif

      receiver->rx_buffer[receiver->buffer_counter] = char_in;
      receiver->checksum += char_in;
      receiver->buffer_counter++;

      if (receiver->buffer_counter >= receiver->payload_size) {
        receiver->state = EXPECTING_CHECKSUM;
      }

      break;
    }

    case EXPECTING_CHECKSUM: {

#if COMM_HEXADECIMAL == 1
      receiver->hexmem = char_in;
      receiver->state  = EXPECTING_CHECKSUM_2;
      /* receiver->checksum += char_in; */
      break;
#else
      if (receiver->checksum == char_in) {

#ifdef DEBUG_PRINT
        printf("getting checksum, OK\n");
#endif

        memcpy(&(message->payload), &(receiver->rx_buffer), receiver->payload_size);
        message->payload_size = receiver->payload_size;
        message->id           = message->payload[0];

        receiver->state = WAITING_FOR_MESSSAGE;
        return true;
      } else {

#ifdef DEBUG_PRINT
        printf("getting checksum, FAIL\n");
#endif
      }
#endif
      break;
    }

#if COMM_HEXADECIMAL == 1
    case EXPECTING_CHECKSUM_2: {

      uint8_t minibuf[2];
      minibuf[0] = receiver->hexmem;
      minibuf[1] = char_in;

      uint8_t checksum = hex2bin(minibuf);

      if (receiver->checksum == checksum) {

#ifdef DEBUG_PRINT
        printf("getting checksum, OK\n");
#endif

        /* memcpy(&(message->payload), &(receiver->rx_buffer), receiver->payload_size); */
        uint16_t counter = 0;
        for (uint8_t i = 0; i < receiver->payload_size / 2; i++) {
          message->payload[i] = hex2bin(receiver->rx_buffer + counter);
          counter += 2;
        }

        message->payload_size = receiver->payload_size / 2;
        message->id           = message->payload[0];

        receiver->state = WAITING_FOR_MESSSAGE;
        return true;
      } else {

#ifdef DEBUG_PRINT
        printf("getting checksum, FAIL\n");
#endif
      }

      break;
    }
#endif
  }

  return false;
}

//}

/*  llcpPrepareMessage() //{ */

inline uint16_t llcpPrepareMessage(uint8_t* what, uint8_t len, uint8_t* buffer) {

#ifdef DEBUG_PRINT
  printf("Preparing message, id = %d, size = %d bytes\n", id, len);
#endif

  uint8_t  checksum = 0;
  uint16_t it       = 0;

  buffer[it++] = 'b';
  checksum += 'b';

#if COMM_HEXADECIMAL == 0
  buffer[it++] = len;
  checksum += len;
#else
  bin2hex(len, buffer + it);
  checksum += buffer[it++];
  checksum += buffer[it++];
#endif

  uint8_t payload_size = len;

  for (uint16_t i = 0; i < payload_size; i++) {

#if COMM_HEXADECIMAL == 0
    buffer[it++] = what[i];
    checksum += what[i];
#else
    bin2hex(what[i], buffer + it);
    checksum += buffer[it++];
    checksum += buffer[it++];
#endif
  }

#if COMM_HEXADECIMAL == 0
  buffer[it++] = checksum;
#else
  bin2hex(checksum, buffer + it);
  it += 2;
#endif

  buffer[it++] = '\n';

  return it;
}

//}

#endif  // BACA_PROTOCOL_H
