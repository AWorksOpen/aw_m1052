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
 * - 1.00 20-10-29  win, first implementation.
 * \endinternal
 */

#include "aw_mp3_internal.h"

/**
 * \brief carry timer fraction into seconds
 */
static void __time_reduce (struct aw_mp3_time *p_time)
{
    p_time->seconds  += p_time->fraction / AW_MP3_TIME_RESOLUTION;
    p_time->fraction %= AW_MP3_TIME_RESOLUTION;
}

/**
 * \brief compute greatest common denominator
 */
static uint32_t __time_gcd (uint32_t num1, uint32_t num2)
{
    uint32_t tmp;

    while (num2) {
        tmp  = num2;
        num2 = num1 % num2;
        num1 = tmp;
    }

    return num1;
}

/**
 * \brief convert rational expression to lowest terms
 */
static void __time_reduce_rational (uint32_t *p_numer,
                                    uint32_t *p_denom)
{
    uint32_t factor;

    factor = __time_gcd(*p_numer, *p_denom);

    aw_assert(factor != 0);

    *p_numer /= factor;
    *p_denom /= factor;
}

/**
 * \brief solve numer/denom == ?/scale avoiding overflowing
 */
static uint32_t __time_scale_rational (uint32_t numer,
                                       uint32_t denom,
                                       uint32_t scale)
{
    __time_reduce_rational(&numer, &denom);
    __time_reduce_rational(&scale, &denom);

    aw_assert(denom != 0);

    if (denom < scale) {
        return numer * (scale / denom) + numer * (scale % denom) / denom;
    }

    if (denom < numer) {
        return scale * (numer / denom) + scale * (numer % denom) / denom;
    }

    return numer * scale / denom;
}

/******************************************************************************/

/**
 * \brief 初始化时间
 */
void aw_mp3_time_init (struct aw_mp3_time *p_time)
{
    p_time->seconds  = 0;
    p_time->fraction = 0;
}

/**
 * \brief 设置时间
 */
void aw_mp3_time_set (struct aw_mp3_time *p_time,
                      int                 seconds,
                      uint32_t            numer,
                      uint32_t            denom)
{
    p_time->seconds = seconds;

    if (numer >= denom && denom > 0) {
        p_time->seconds += numer / denom;
        numer           %= denom;
    }

    switch (denom) {

    case 0:
    case 1:
        p_time->fraction = 0;
        break;

    case AW_MP3_TIME_RESOLUTION:
        p_time->fraction = numer;
        break;

    case 1000:
        p_time->fraction = numer * (AW_MP3_TIME_RESOLUTION / 1000);
        break;

    case 8000:
        p_time->fraction = numer * (AW_MP3_TIME_RESOLUTION / 8000);
        break;

    case 11025:
        p_time->fraction = numer * (AW_MP3_TIME_RESOLUTION / 11025);
        break;

    case 12000:
        p_time->fraction = numer * (AW_MP3_TIME_RESOLUTION / 12000);
        break;

    case 16000:
        p_time->fraction = numer * (AW_MP3_TIME_RESOLUTION / 16000);
        break;

    case 22050:
        p_time->fraction = numer * (AW_MP3_TIME_RESOLUTION / 22050);
        break;

    case 24000:
        p_time->fraction = numer * (AW_MP3_TIME_RESOLUTION / 24000);
        break;

    case 32000:
        p_time->fraction = numer * (AW_MP3_TIME_RESOLUTION / 32000);
        break;

    case 44100:
        p_time->fraction = numer * (AW_MP3_TIME_RESOLUTION / 44100);
        break;

    case 48000:
        p_time->fraction = numer * (AW_MP3_TIME_RESOLUTION / 48000);
        break;

    default:
        p_time->fraction = __time_scale_rational(numer,
                                                 denom,
                                                 AW_MP3_TIME_RESOLUTION);
        break;
    }

    if (p_time->fraction >= AW_MP3_TIME_RESOLUTION) {
        __time_reduce(p_time);
    }
}

/**
 * \brief 增加时间
 */
void aw_mp3_time_add (struct aw_mp3_time *p_time,
                      struct aw_mp3_time *p_incr)
{
    p_time->seconds  += p_incr->seconds;
    p_time->fraction += p_incr->fraction;

    if (p_time->fraction >= AW_MP3_TIME_RESOLUTION) {
        __time_reduce(p_time);
    }
}

/* end of file */
