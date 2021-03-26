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
 * - 1.00 20-10-23  win, first implementation.
 * \endinternal
 */

#ifndef __AW_MP3_DEC_SYNTH_H
#define __AW_MP3_DEC_SYNTH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_mp3_internal.h"

/**
 * \brief PCM数据
 */
struct aw_mp3_pcm {
    uint32_t sample_rate;       /**< \brief sampling frequency (Hz) */
    uint16_t channels;          /**< \brief number of channels */
    uint16_t length;            /**< \brief number of samples per channel */
    int32_t  samples[2][1152];  /**< \brief PCM output samples [ch][sample] */
};

/**
 * \brief PCM同步
 */
struct aw_mp3_dec_synth {

    /**
     * \brief polyphase filterbank outputs [ch][eo][peo][s][v]
     */
    int32_t           filter[2][2][2][16][8];

    uint32_t          phase;  /**< \brief current processing phase */

    struct aw_mp3_pcm pcm;    /**< \brief PCM output */
};

/**
 * \brief single channel PCM selector
 */
enum aw_mp3_pcm_channel {
    AW_MP3_PCM_CHANNEL_SINGLE = 0,

    AW_MP3_PCM_CHANNEL_DUAL_1 = AW_MP3_PCM_CHANNEL_SINGLE,
    AW_MP3_PCM_CHANNEL_DUAL_2 = 1,
};

/* stereo PCM selector */
enum {
  MAD_PCM_CHANNEL_STEREO_LEFT  = 0,
  MAD_PCM_CHANNEL_STEREO_RIGHT = 1
};

/**
 * \brief synth初始化
 */
void aw_mp3_dec_synth_init (struct aw_mp3_dec_synth *p_synth);

/**
 * \brief synth mute
 */
void aw_mp3_dec_synth_mute (struct aw_mp3_dec_synth *p_synth);

/**
 * \brief synth frame
 */
void aw_mp3_dec_synth_frame (struct aw_mp3_dec_synth       *p_synth,
                             const struct aw_mp3_dec_frame *p_frame);

#ifdef __cplusplus
}
#endif

#endif /* __AW_MP3_DEC_SYNTH_H */

/* end of file */
