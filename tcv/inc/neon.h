#ifndef __NEON_H__
#define __NEON_H__

#ifdef __ARM_NEON
	#include "arm_neon.h"
#else
    #include "NEON_2_SSE.h"
#endif // __ARM_NEON

#endif // __NEON_H__