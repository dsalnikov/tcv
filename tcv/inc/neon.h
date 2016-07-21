#ifndef __NEON_H__
#define __NEON_H__

#ifdef __ARM_NEON
	#include "arm_neon.h"
#else

typedef unsigned char uint8_t;

typedef struct {
	uint8_t m[16];
} uint8x16_t;


uint8x16_t vld1q_u8(uint8_t const * ptr);
void vst1q_u8(uint8_t * ptr, uint8x16_t val);

uint8x16_t vminq_u8(uint8x16_t a, uint8x16_t b);
uint8x16_t vmaxq_u8(uint8x16_t a, uint8x16_t b);

#endif // __ARM_NEON

#endif // __NEON_H__