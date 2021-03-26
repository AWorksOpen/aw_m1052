/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Stock Co.,  Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief 
 *
 * \internal
 * \par Modification history
 * - 1.00 20-10-15  win, first implementation.
 * \endinternal
 */

#ifndef __AW_MP3_FIXED_H
#define __AW_MP3_FIXED_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_mp3_internal.h"

/**
 * \brief
 *
 * Fixed-point format: 0xABBBBBBB
 * A == whole part      (sign + 3 bits)
 * B == fractional part (28 bits)
 * config.h
 * Values are signed two's complement, so the effective range is:
 * 0x80000000 to 0x7fffffff
 *       -8.0 to +7.9999999962747097015380859375
 *
 * The smallest representable value is:
 * 0x00000001 == 0.0000000037252902984619140625 (i.e. about 3.725e-9)
 *
 * 28 bits of fractional accuracy represent about
 * 8.6 digits of decimal accuracy.
 *
 * Fixed-point numbers can be added or subtracted as normal
 * integers, but multiplication requires shifting the 64-bit result
 * from 56 fractional bits back to 28 (and rounding.)
 *
 * Changing the definition of MAD_F_FRACBITS is only partially
 * supported, and must be done with care.
 */

#define AW_MP3_F_FRACBITS     28

#if AW_MP3_F_FRACBITS == 28
#define AW_MP3_F(x)           ((int32_t) (x##L))
#else
#if AW_MP3_F_FRACBITS < 28
#warning "MAD_F_FRACBITS < 28"
#define AW_MP3_F(x)           ((int32_t)  \
                              (((x##L) +  \
                                (1L << (28 - AW_MP3_F_FRACBITS - 1))) >>  \
                                (28 - AW_MP3_F_FRACBITS)))
#elif AW_MP3_F_FRACBITS > 28
#error "MAD_F_FRACBITS > 28 not currently supported"
#define AW_MP3_F(x)           ((int32_t)  \
                              ((x##L) << (AW_MP3_F_FRACBITS - 28)))
#endif
#endif

#define AW_MP3_F_MIN          ((int32_t)-0x80000000L)
#define AW_MP3_F_MAX          ((int32_t)+0x7fffffffL)

#define AW_MP3_F_ONE           AW_MP3_F(0x10000000)

#define AW_MP3_F_TO_DOUBLE(x) \
    ((double)((x) / (double) (1L << AW_MP3_F_FRACBITS)))

#define AW_MP3_F_SCALEBITS  AW_MP3_F_FRACBITS

/*
 * This version is the most portable but it loses significant accuracy.
 * Furthermore, accuracy is biased against the second argument, so care
 * should be taken when ordering operands.
 *
 * The scale factors are constant as this is not used with SSO.
 *
 * Pre-rounding is required to stay within the limits of compliance.
 */
#if (AW_MP3_FPM_DEFAULT)
#if (AW_MP3_OPT_SPEED)

#define AW_MP3_F_MUL(x, y)  (((x) >> 12) * ((y) >> 16))

#else

#define AW_MP3_F_MUL(x, y)  ((((x) + (1L << 11)) >> 12) *  \
                            (((y) + (1L << 15)) >> 16))

#endif
#endif

#if !defined(AW_MP3_F_MLA)
#define AW_MP3_F_ML0(hi, lo, x, y)   ((lo)  = AW_MP3_F_MUL((x), (y)))
#define AW_MP3_F_MLA(hi, lo, x, y)   ((lo) += AW_MP3_F_MUL((x), (y)))
#define AW_MP3_F_MLN(hi, lo)         ((lo)  = -(lo))
#define AW_MP3_F_MLZ(hi, lo)         ((void)(hi), (int32_t)(lo))
#endif

#ifdef __cplusplus
}
#endif

#endif /* __AW_MP3_FIXED_H */

/* end of file */
