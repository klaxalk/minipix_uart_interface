#include <llcp_endian.h>

// the "network" layer runs on BIG_ENDIAN

/* Convert 16-bit number from host byte order to network byte order */
uint16_t __attribute__((__const__)) llcp_hton16(uint16_t h16) {
#ifdef LLCP_BIG_ENDIAN
  return h16;
#else
  return (((h16 & 0xff00) >> 8) | ((h16 & 0x00ff) << 8));
#endif
}

/* Convert 16-bit number from network byte order to host byte order */
uint16_t __attribute__((__const__)) llcp_ntoh16(uint16_t n16) {
  return llcp_hton16(n16);
}

/* Convert 32-bit number from host byte order to network byte order */
uint32_t __attribute__((__const__)) llcp_hton32(uint32_t h32) {
#ifdef LLCP_BIG_ENDIAN
  return h32;
#else
  return (((h32 & 0xff000000) >> 24) | ((h32 & 0x000000ff) << 24) | ((h32 & 0x0000ff00) << 8) | ((h32 & 0x00ff0000) >> 8));
#endif
}

/* Convert 32-bit number from network byte order to host byte order */
uint32_t __attribute__((__const__)) llcp_ntoh32(uint32_t n32) {
  return llcp_hton32(n32);
}

/* Convert 64-bit number from host byte order to network byte order */
uint64_t __attribute__((__const__)) llcp_hton64(uint64_t h64) {
#ifdef LLCP_BIG_ENDIAN
  return h64;
#else
  return (((h64 & 0xff00000000000000LL) >> 56) | ((h64 & 0x00000000000000ffLL) << 56) | ((h64 & 0x00ff000000000000LL) >> 40) |
          ((h64 & 0x000000000000ff00LL) << 40) | ((h64 & 0x0000ff0000000000LL) >> 24) | ((h64 & 0x0000000000ff0000LL) << 24) |
          ((h64 & 0x000000ff00000000LL) >> 8) | ((h64 & 0x00000000ff000000LL) << 8));
#endif
}

/* Convert 64-bit number from host byte order to network byte order */
uint64_t __attribute__((__const__)) llcp_ntoh64(uint64_t n64) {
  return llcp_hton64(n64);
}

/* Convert 16-bit number from host byte order to big endian byte order */
uint16_t __attribute__((__const__)) llcp_htobe16(uint16_t h16) {
  return llcp_hton16(h16);
}

/* Convert 16-bit number from host byte order to little endian byte order */
uint16_t __attribute__((__const__)) llcp_htole16(uint16_t h16) {
#ifdef LLCP_LITTLE_ENDIAN
  return h16;
#else
  return (((h16 & 0xff00) >> 8) | ((h16 & 0x00ff) << 8));
#endif
}

/* Convert 16-bit number from big endian byte order to little endian byte order */
uint16_t __attribute__((__const__)) llcp_betoh16(uint16_t be16) {
  return llcp_ntoh16(be16);
}

/* Convert 16-bit number from little endian byte order to host byte order */
uint16_t __attribute__((__const__)) llcp_letoh16(uint16_t le16) {
  return llcp_htole16(le16);
}

/* Convert 32-bit number from host byte order to big endian byte order */
uint32_t __attribute__((__const__)) llcp_htobe32(uint32_t h32) {
  return llcp_hton32(h32);
}

/* Convert 32-bit number from little endian byte order to host byte order */
uint32_t __attribute__((__const__)) llcp_htole32(uint32_t h32) {
#ifdef LLCP_LITTLE_ENDIAN
  return h32;
#else
  return (((h32 & 0xff000000) >> 24) | ((h32 & 0x000000ff) << 24) | ((h32 & 0x0000ff00) << 8) | ((h32 & 0x00ff0000) >> 8));
#endif
}

/* Convert 32-bit number from big endian byte order to host byte order */
uint32_t __attribute__((__const__)) llcp_betoh32(uint32_t be32) {
  return llcp_ntoh32(be32);
}

/* Convert 32-bit number from little endian byte order to host byte order */
uint32_t __attribute__((__const__)) llcp_letoh32(uint32_t le32) {
  return llcp_htole32(le32);
}

/* Convert 64-bit number from host byte order to big endian byte order */
uint64_t __attribute__((__const__)) llcp_htobe64(uint64_t h64) {
  return llcp_hton64(h64);
}

/* Convert 64-bit number from host byte order to little endian byte order */
uint64_t __attribute__((__const__)) llcp_htole64(uint64_t h64) {
#ifdef LLCP_LITTLE_ENDIAN
  return h64;
#else
  return (((h64 & 0xff00000000000000LL) >> 56) | ((h64 & 0x00000000000000ffLL) << 56) | ((h64 & 0x00ff000000000000LL) >> 40) |
          ((h64 & 0x000000000000ff00LL) << 40) | ((h64 & 0x0000ff0000000000LL) >> 24) | ((h64 & 0x0000000000ff0000LL) << 24) |
          ((h64 & 0x000000ff00000000LL) >> 8) | ((h64 & 0x00000000ff000000LL) << 8));
#endif
}

/* Convert 64-bit number from big endian byte order to host byte order */
uint64_t __attribute__((__const__)) llcp_betoh64(uint64_t be64) {
  return llcp_ntoh64(be64);
}

/* Convert 64-bit number from little endian byte order to host byte order */
uint64_t __attribute__((__const__)) llcp_letoh64(uint64_t le64) {
  return llcp_htole64(le64);
}
