#include <llcp.h>

// | ----------------------- hex <-> bin ---------------------- |

/* llcp_hex2bin() //{ */

uint8_t llcp_hex2bin(const uint8_t* ptr) {

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

/* llcp_bin2hex() //{ */


/**
 * @brief convert a single byte to hexadecimal
 *
 * @param x the byte to convert
 * @param buffer pointer to a 2-char buffer for the result
 */
void llcp_bin2hex(const uint8_t x, uint8_t* buffer) {

  if (x >= 16) {
    *buffer = "0123456789ABCDEF"[x / 16];
    *(buffer+1) = "0123456789ABCDEF"[x & 15];
  } else {
    *buffer = '0';
    *(buffer+1) = "0123456789ABCDEF"[x];
  }
}

//}

// | ------------------------ routines ------------------------ |

/* llcp_initialize() //{ */

void llcp_initialize(LLCP_Receiver_t* receiver) {

  receiver->checksum       = 0;
  receiver->buffer_counter = 0;
  receiver->state          = WAITING_FOR_MESSSAGE;
}

//}

/* llcp_processChar() //{ */

bool llcp_processChar(const uint8_t char_in, LLCP_Receiver_t* receiver, LLCP_Message_t* message) {

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

      uint16_t payload_size = llcp_hex2bin(minibuf) * 2;

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

      uint8_t checksum = llcp_hex2bin(minibuf);

      if (receiver->checksum == checksum) {

#ifdef DEBUG_PRINT
        printf("getting checksum, OK\n");
#endif

        /* memcpy(&(message->payload), &(receiver->rx_buffer), receiver->payload_size); */
        uint16_t counter = 0;
        for (uint8_t i = 0; i < receiver->payload_size / 2; i++) {
          message->payload[i] = llcp_hex2bin(receiver->rx_buffer + counter);
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

/*  llcp_prepareMessage() //{ */

uint16_t llcp_prepareMessage(uint8_t* what, uint8_t len, uint8_t* buffer) {

#ifdef DEBUG_PRINT
  printf("Preparing message, id = %d, size = %d bytes\n", what[0], len);
#endif

  uint8_t  checksum = 0;
  uint16_t it       = 0;

  buffer[it++] = 'b';
  checksum += 'b';

#if COMM_HEXADECIMAL == 0
  buffer[it++] = len;
  checksum += len;
#else
  llcp_bin2hex(len, buffer + it);
  checksum += buffer[it++];
  checksum += buffer[it++];
#endif

  uint8_t payload_size = len;

  for (uint16_t i = 0; i < payload_size; i++) {

#if COMM_HEXADECIMAL == 0
    buffer[it++] = what[i];
    checksum += what[i];
#else
    llcp_bin2hex(what[i], buffer + it);
    checksum += buffer[it++];
    checksum += buffer[it++];
#endif
  }

#if COMM_HEXADECIMAL == 0
  buffer[it++] = checksum;
#else
  llcp_bin2hex(checksum, buffer + it);
  it += 2;
#endif

  buffer[it++] = '\n';

  return it;
}

//}
