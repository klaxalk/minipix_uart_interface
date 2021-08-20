#ifndef PIXEL_ENCODER_H
#define PIXEL_ENCODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <llcp_minipix_messages.h>

#define MAX_LUT_EVENT 16
#define MAX_LUT_TOT 1024
#define MAX_LUT_ITOT 16384
#define MAX_LUT_TOA 16384

#define WRONG_LUT_EVENT 15
#define WRONG_LUT_TOT 1023
#define WRONG_LUT_ITOT 16383
#define WRONG_LUT_TOA 16383

typedef enum {
  TPX3_TOA_TOT,
  TPX3_TOA,
  TPX3_MPX_ITOT,
} TPX3PixelMode_t;

void encodePixelData(uint8_t* data, const uint8_t col_shift_num, const TPX3PixelMode_t pixel_mode);

extern const int16_t LUT_EVENT[16];

extern const uint16_t LUT_ITOT[16384];

extern const uint16_t LUT_TOA[16384];

extern const uint16_t LUT_TOT[1024];

extern const uint16_t LUT_COLSHIFTS[1280];

#ifdef __cplusplus
}
#endif

#endif // PIXEL_ENCODER_H
