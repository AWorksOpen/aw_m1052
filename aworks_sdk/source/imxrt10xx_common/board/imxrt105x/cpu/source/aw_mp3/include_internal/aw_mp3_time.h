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

#ifndef __AW_MP3_TIME_H
#define __AW_MP3_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief time units
 */
enum mad_units {

    AW_MP3_TIME_UNITS_HOURS        = -2,
    AW_MP3_TIME_UNITS_MINUTES      = -1,
    AW_MP3_TIME_UNITS_SECONDS      = 0,

    /** \brief metric units */
    AW_MP3_TIME_UNITS_DECISECONDS  = 10,
    AW_MP3_TIME_UNITS_CENTISECONDS = 100,
    AW_MP3_TIME_UNITS_MILLISECONDS = 1000,

    /** \brief audio sample units */
    AW_MP3_TIME_UNITS_8000_HZ      =  8000,
    AW_MP3_TIME_UNITS_11025_HZ     = 11025,
    AW_MP3_TIME_UNITS_12000_HZ     = 12000,

    AW_MP3_TIME_UNITS_16000_HZ     = 16000,
    AW_MP3_TIME_UNITS_22050_HZ     = 22050,
    AW_MP3_TIME_UNITS_24000_HZ     = 24000,
    AW_MP3_TIME_UNITS_32000_HZ     = 32000,
    AW_MP3_TIME_UNITS_44100_HZ     = 44100,
    AW_MP3_TIME_UNITS_48000_HZ     = 48000,

    /** \brief video frame/field units */
    AW_MP3_TIME_UNITS_24_FPS       = 24,
    AW_MP3_TIME_UNITS_25_FPS       = 25,
    AW_MP3_TIME_UNITS_30_FPS       = 30,
    AW_MP3_TIME_UNITS_48_FPS       = 48,
    AW_MP3_TIME_UNITS_50_FPS       = 50,
    AW_MP3_TIME_UNITS_60_FPS       = 60,

    /** \brief CD audio frames */
    AW_MP3_TIME_UNITS_75_FPS       = 75,

    /** \brief video drop-frame units */
    AW_MP3_TIME_UNITS_23_976_FPS   = -24,
    AW_MP3_TIME_UNITS_24_975_FPS   = -25,
    AW_MP3_TIME_UNITS_29_97_FPS    = -30,
    AW_MP3_TIME_UNITS_47_952_FPS   = -48,
    AW_MP3_TIME_UNITS_49_95_FPS    = -50,
    AW_MP3_TIME_UNITS_59_94_FPS    = -60
};

/**
 * \brief 初始化时间
 */
void aw_mp3_time_init (struct aw_mp3_time *p_time);

/**
 * \brief 复位时间
 */
aw_static_inline
void aw_mp3_time_reset (struct aw_mp3_time *p_time)
{
    aw_mp3_time_init(p_time);
}

/**
 * \brief 设置时间
 */
void aw_mp3_time_set (struct aw_mp3_time *p_time,
                      int                 seconds,
                      uint32_t            numer,
                      uint32_t            denom);

/**
 * \brief 增加时间
 */
void aw_mp3_time_add (struct aw_mp3_time *p_time,
                      struct aw_mp3_time *p_incr);

#ifdef __cplusplus
}
#endif

#endif /* __AW_MP3_TIME_H */

/* end of file */
