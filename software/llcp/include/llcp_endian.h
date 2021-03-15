#ifndef LLCP_ENDIAN_H
#define LLCP_ENDIAN_H

#include <stdint.h>

// the "network" layer runs on BIG_ENDIAN

#if !defined(LLCP_BIG_ENDIAN) && !defined(LLCP_LITTLE_ENDIAN)
#error "Must define one of LLCP_BIG_ENDIAN or LLCP_LITTLE_ENDIAN for this platform"
#endif

/* Convert 16-bit number from host byte order to network byte order */
uint16_t __attribute__((__const__)) llcp_hton16(uint16_t h16);

/* Convert 16-bit number from network byte order to host byte order */
uint16_t __attribute__((__const__)) llcp_ntoh16(uint16_t n16);

/* Convert 32-bit number from host byte order to network byte order */
uint32_t __attribute__((__const__)) llcp_hton32(uint32_t h32);

/* Convert 32-bit number from network byte order to host byte order */
uint32_t __attribute__((__const__)) llcp_ntoh32(uint32_t n32);

/* Convert 64-bit number from host byte order to network byte order */
uint64_t __attribute__((__const__)) llcp_hton64(uint64_t h64);

/* Convert 64-bit number from host byte order to network byte order */
uint64_t __attribute__((__const__)) llcp_ntoh64(uint64_t n64);

/* Convert 16-bit number from host byte order to big endian byte order */
uint16_t __attribute__((__const__)) llcp_htobe16(uint16_t h16);

/* Convert 16-bit number from host byte order to little endian byte order */
uint16_t __attribute__((__const__)) llcp_htole16(uint16_t h16);

/* Convert 16-bit number from big endian byte order to little endian byte order */
uint16_t __attribute__((__const__)) llcp_betoh16(uint16_t be16);

/* Convert 16-bit number from little endian byte order to host byte order */
uint16_t __attribute__((__const__)) llcp_letoh16(uint16_t le16);

/* Convert 32-bit number from host byte order to big endian byte order */
uint32_t __attribute__((__const__)) llcp_htobe32(uint32_t h32);

/* Convert 32-bit number from little endian byte order to host byte order */
uint32_t __attribute__((__const__)) llcp_htole32(uint32_t h32);

/* Convert 32-bit number from big endian byte order to host byte order */
uint32_t __attribute__((__const__)) llcp_betoh32(uint32_t be32);

/* Convert 32-bit number from little endian byte order to host byte order */
uint32_t __attribute__((__const__)) llcp_letoh32(uint32_t le32);

/* Convert 64-bit number from host byte order to big endian byte order */
uint64_t __attribute__((__const__)) llcp_htobe64(uint64_t h64);

/* Convert 64-bit number from host byte order to little endian byte order */
uint64_t __attribute__((__const__)) llcp_htole64(uint64_t h64);

/* Convert 64-bit number from big endian byte order to host byte order */
uint64_t __attribute__((__const__)) llcp_betoh64(uint64_t be64);

/* Convert 64-bit number from little endian byte order to host byte order */
uint64_t __attribute__((__const__)) llcp_letoh64(uint64_t le64);

#endif  // _H
