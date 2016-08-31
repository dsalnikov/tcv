#include "neon.h"



uint8x16_t vld1q_u8(uint8_t const * ptr)
{
	char i;
	uint8x16_t q;

	for (i = 0; i < 16; i++)
		q.m[i] = *(ptr++);

	return q;
}

uint8x16_t vminq_u8(uint8x16_t a, uint8x16_t b)
{
	char i;
	uint8x16_t q;

	for (i = 0; i < 16; i++)
	{
		q.m[i] = (a.m[i] > b.m[i]) ? b.m[i] : a.m[i];
	}

	return q;
}

uint8x16_t vmaxq_u8(uint8x16_t a, uint8x16_t b)
{
	char i;
	uint8x16_t q;

	for (i = 0; i < 16; i++)
	{
		q.m[i] = (a.m[i] > b.m[i]) ? a.m[i] : b.m[i];
	}

	return q;
}

void vst1q_u8(uint8_t * ptr, uint8x16_t val)
{
	char i;
	for (i = 0; i < 16; i++)
		*(ptr++) = val.m[i];
}