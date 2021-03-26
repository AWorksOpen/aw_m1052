/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-11-10  win, first implementation
 * \endinternal
 */

#include "core/awsa_core_internal.h"

extern const
struct awsa_pcm_format_data __g_core_pcm_formats[AWSA_PCM_FORMAT_LAST+1];

/**
 * \brief samples to bytes
 */
static inline
ssize_t __core_pcm_samples_to_bytes (struct awsa_core_pcm_runtime *p_runtime,
                                     ssize_t                       size)
{
    return size * p_runtime->sample_bits / 8;
}

/**
 * \brief frames convert to bytes
 */
static inline
ssize_t __core_pcm_frames_to_bytes (struct awsa_core_pcm_runtime *p_runtime,
                                    awsa_pcm_sframes_t            size)
{
    return size * p_runtime->frame_bits / 8;
}

/**
 * \brief pcm get system time
 */
static inline
void __core_pcm_gettime (struct awsa_core_pcm_runtime *p_runtime,
                         struct aw_timespec           *p_tv)
{
    aw_timespec_get(p_tv);
}

/**
 * \brief result is: 0 ... (boundary - 1)
 */
static inline
awsa_pcm_uframes_t __core_pcm_playback_avail (
    struct awsa_core_pcm_runtime *p_runtime)
{
    awsa_pcm_sframes_t avail = p_runtime->hw_ptr       +
                               p_runtime->buffer_size  -
                               p_runtime->appl_ptr;
    if (avail < 0) {
        avail += p_runtime->boundary;
    } else if ((awsa_pcm_uframes_t)avail >= p_runtime->boundary) {
        avail -= p_runtime->boundary;
    }

    return avail;
}

/**
 * \brief result is: 0 ... (boundary - 1)
 */
static inline
awsa_pcm_uframes_t __core_pcm_capture_avail (
    struct awsa_core_pcm_runtime *p_runtime)
{
    awsa_pcm_sframes_t avail = p_runtime->hw_ptr - p_runtime->appl_ptr;
    if (avail < 0) {
        avail += p_runtime->boundary;
    }

    return avail;
}

/**
 * \brief playback avail
 */
static inline
awsa_pcm_sframes_t __core_pcm_playback_hw_avail (
    struct awsa_core_pcm_runtime *p_runtime)
{
    return p_runtime->buffer_size - __core_pcm_playback_avail(p_runtime);
}

#if 0

/**
 * \brief capture avail
 */
static inline
awsa_pcm_sframes_t __core_pcm_capture_hw_avail (
    struct awsa_core_pcm_runtime *p_runtime)
{
    return p_runtime->buffer_size - __core_pcm_capture_avail(p_runtime);
}

#endif

/**
 * \brief check whether any data exists on the playback buffer
 */
static inline aw_bool_t
__core_pcm_playback_data (struct awsa_core_pcm_substream *p_substream)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;

    if (p_runtime->config.stop_threshold >= p_runtime->boundary) {
        return AW_TRUE;
    }

    return (__core_pcm_playback_avail(p_runtime) < p_runtime->buffer_size);
}

/**
 * \brief check whether the playback buffer is empty
 */
static inline
int __core_pcm_playback_empty (struct awsa_core_pcm_substream *p_substream)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;
    return __core_pcm_playback_avail(p_runtime) >= p_runtime->buffer_size;
}

#if 0

/**
 * \brief check whether the capture buffer is empty
 */
static inline
int __core_pcm_capture_empty (struct awsa_core_pcm_substream *p_substream)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;
    return __core_pcm_capture_avail(p_runtime) == 0;
}

#endif

/**
 * \brief try to stop all running streams in the substream group
 */
static
aw_err_t __core_pcm_lib_xrun (struct awsa_core_pcm_substream *p_substream)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;

    __core_pcm_gettime(p_runtime,
                      (struct aw_timespec *)&p_runtime->tstamp);
    return awsa_core_pcm_lib_stop(p_substream, AWSA_CORE_PCM_STATE_XRUN);
}

/**
 * \brief Change the RUNNING stream(s) to XRUN state.
 */
static
aw_err_t __core_pcm_xrun (struct awsa_core_pcm_substream *p_substream)
{
    struct awsa_core_card        *p_card    = p_substream->p_pcm->p_card;
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;

    int result;

    AW_MUTEX_LOCK(p_card->power_lock, AW_SEM_WAIT_FOREVER);
    aw_spinlock_isr_take(&p_substream->lock);

    switch (p_runtime->state) {

    case AWSA_CORE_PCM_STATE_XRUN:
        result = 0; /* already there */
        break;

    case AWSA_CORE_PCM_STATE_RUNNING:
        result = awsa_core_pcm_lib_stop(p_substream, AWSA_CORE_PCM_STATE_XRUN);
        break;

    default:
        result = -AW_EBADF;
    }

    aw_spinlock_isr_give(&p_substream->lock);
    AW_MUTEX_UNLOCK(p_card->power_lock);
    return result;
}

#if 0
/**
 * \brief set trigger timestamp
 */
static
void __core_pcm_trigger_tstamp (struct awsa_core_pcm_substream *p_substream)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;

    __core_pcm_gettime(p_runtime, &p_runtime->trigger_tstamp);
}
#endif

/**
 * \brief set the silence data on the buffer
 */
static
aw_err_t __core_pcm_format_silence_set (awsa_pcm_format_t   format,
                                        void               *p_data,
                                        awsa_pcm_uframes_t  samples)
{
    int      width;
    uint8_t *p_dst, *p_pat;

    if (format < 0 || format > AWSA_PCM_FORMAT_LAST) {
        return -AW_EINVAL;
    }

    if (samples == 0) {
        return 0;
    }

    width = __g_core_pcm_formats[format].phys;    /* physical width */
    p_pat = (uint8_t *)__g_core_pcm_formats[format].silence;
    if (width == 0) {
        return -AW_EINVAL;
    }

    /* signed or 1 byte data */
    if (__g_core_pcm_formats[format].signd == 1 || width <= 8) {
        uint32_t bytes = samples * width / 8;
        memset(p_data, *p_pat, bytes);
        return AW_OK;
    }

    /* non-zero samples, fill using a loop */
    width /= 8;
    p_dst  = p_data;

#if 0
    while (samples--) {
        memcpy(dst, pat, width);
        dst += width;
    }
#else

    /* a bit optimization for constant width */
    switch (width) {

    case 2:
        while (samples--) {
            memcpy(p_dst, p_pat, 2);
            p_dst += 2;
        }
        break;

    case 3:
        while (samples--) {
            memcpy(p_dst, p_pat, 3);
            p_dst += 3;
        }
        break;

    case 4:
        while (samples--) {
            memcpy(p_dst, p_pat, 4);
            p_dst += 4;
        }
        break;

    case 8:
        while (samples--) {
            memcpy(p_dst, p_pat, 8);
            p_dst += 8;
        }
        break;
    }
#endif

    return AW_OK;
}

/**
 * \brief fill ring buffer with silence
 */
static void __core_pcm_lib_playback_silence (
    struct awsa_core_pcm_substream *p_substream,
    awsa_pcm_uframes_t              new_hw_ptr)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;
    awsa_pcm_uframes_t            frames, ofs, transfer;

    if (p_runtime->silence_size < p_runtime->boundary) {
        awsa_pcm_sframes_t noise_dist, n;

        if (p_runtime->silence_start != p_runtime->appl_ptr) {
            n = p_runtime->appl_ptr - p_runtime->silence_start;
            if (n < 0) {
                n += p_runtime->boundary;
            }

            if ((awsa_pcm_uframes_t)n < p_runtime->silence_filled) {
                p_runtime->silence_filled -= n;
            } else {
                p_runtime->silence_filled = 0;
            }

            p_runtime->silence_start = p_runtime->appl_ptr;
        }

        if (p_runtime->silence_filled >= p_runtime->buffer_size) {
            return;
        }

        noise_dist = __core_pcm_playback_hw_avail(p_runtime) +
                     p_runtime->silence_filled;
        if (noise_dist >= p_runtime->config.silence_threshold) {
            return;
        }

        frames = p_runtime->config.silence_threshold - noise_dist;
        if (frames > p_runtime->silence_size) {
            frames = p_runtime->silence_size;
        }
    } else {
        if (new_hw_ptr == ULONG_MAX) {  /* initialization */
            awsa_pcm_sframes_t avail = __core_pcm_playback_hw_avail(p_runtime);
            if (avail > p_runtime->buffer_size) {
                avail = p_runtime->buffer_size;
            }
            p_runtime->silence_filled = avail > 0 ? avail : 0;
            p_runtime->silence_start  = (p_runtime->hw_ptr          +
                                         p_runtime->silence_filled) %
                                         p_runtime->boundary;
        } else {
            ofs = p_runtime->hw_ptr;
            frames = new_hw_ptr - ofs;
            if ((awsa_pcm_sframes_t)frames < 0) {
                frames += p_runtime->boundary;
            }
            p_runtime->silence_filled -= frames;
            if ((awsa_pcm_sframes_t)p_runtime->silence_filled < 0) {
                p_runtime->silence_filled = 0;
                p_runtime->silence_start = new_hw_ptr;
            } else {
                p_runtime->silence_start = ofs;
            }
        }
        frames = p_runtime->buffer_size - p_runtime->silence_filled;
    }

    if (frames > p_runtime->buffer_size) {
        return;
    }

    if (frames == 0) {
        return;
    }

    ofs = p_runtime->silence_start % p_runtime->buffer_size;
    while (frames > 0) {
        transfer = ofs + frames > p_runtime->buffer_size ?
                   p_runtime->buffer_size - ofs : frames;

#if 0
        if (p_substream->p_opts->pfn_silence) {
            int err;
            err = p_substream->p_opts->pfn_silence(p_substream,
                                                   -1,
                                                   ofs,
                                                   transfer);
            awsa_core_assert(err == 0);
        } else {
            char *hwbuf = p_runtime->p_dma_area +
                          __core_pcm_frames_to_bytes(p_runtime, ofs);
            __core_pcm_misc_format_set_silence(p_runtime->format,
                                               hwbuf,
                                               transfer * p_runtime->channels);
        }
#else
        {
            uint32_t c;
            uint32_t channels = p_runtime->config.channels;

            struct awsa_pcm_config *p_config = &p_runtime->config;

            uint8_t  bits = awsa_core_pcm_format_phys_get(p_config->format);

            if (p_substream->p_opts->pfn_silence != NULL) {
                for (c = 0; c < channels; ++c) {
                    aw_err_t err;

                    err = p_substream->p_opts->pfn_silence(p_substream,
                                                           c,
                                                           ofs,
                                                           transfer);
                    awsa_core_assert(err == 0);
                }
            } else {
                size_t dma_csize = p_config->period_size * bits / 8 *
                                   p_config->period_count;

                for (c = 0; c < channels; ++c) {
                    uint8_t *p_hwbuf =
                                  p_runtime->p_dma_area +
                                  (c * dma_csize)       +
                                  __core_pcm_samples_to_bytes(p_runtime, ofs);
                    __core_pcm_format_silence_set(p_runtime->config.format,
                                                  p_hwbuf,
                                                  transfer);
                }
            }
        }
#endif

        p_runtime->silence_filled += transfer;
        frames -= transfer;
        ofs = 0;
    }
}

/**
 * \brief update substream state
 */
static
aw_err_t __core_pcm_update_state (struct awsa_core_pcm_substream *p_substream,
                                  struct awsa_core_pcm_runtime   *p_runtime)
{
    awsa_pcm_uframes_t avail;

    if (p_substream->stream == AWSA_PCM_STREAM_PLAYBACK) {
        avail = __core_pcm_playback_avail(p_runtime);
    } else {
        avail = __core_pcm_capture_avail(p_runtime);
    }

    if (avail > p_runtime->avail_max) {
        p_runtime->avail_max = avail;
    }

    if (p_runtime->state == AWSA_CORE_PCM_STATE_DRAINING) {
        if (avail >= p_runtime->buffer_size) {
            awsa_core_pcm_lib_drain_done(p_substream);
            return -AW_EPIPE;
        }
    } else {
        if (avail >= p_runtime->config.stop_threshold) {
            __core_pcm_lib_xrun(p_substream);
            return -AW_EPIPE;
        }
    }

    if (p_runtime->twake) {
        if (avail >= p_runtime->twake) {
            AW_SEMB_GIVE(p_runtime->tsleep);
        }
    } else if (avail >= p_runtime->config.avail_min) {
        AW_SEMB_GIVE(p_runtime->sleep);
    }

    return AW_OK;
}

aw_err_t awsa_core_pcm_lib_update (
    struct awsa_core_pcm_substream *p_substream,
    aw_bool_t                       in_interrupt)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;

    awsa_pcm_uframes_t pos;
    awsa_pcm_uframes_t old_hw_ptr, new_hw_ptr, hw_base;
    awsa_pcm_sframes_t hdelta, delta;
    uint32_t           jdelta;

    old_hw_ptr = p_runtime->hw_ptr;
    pos = p_substream->p_opts->pfn_pointer(p_substream);
    if (pos == AWSA_CORE_PCM_POS_XRUN) {
        __core_pcm_lib_xrun(p_substream);
        return -AW_EPIPE;
    }

    if (pos >= p_runtime->buffer_size) {
        pos = 0;
    }
    pos -= pos % p_runtime->min_align;

    hw_base = p_runtime->hw_ptr_base;
    new_hw_ptr = hw_base + pos;
    if (in_interrupt) {

        /*
         * we know that one period was processed
         * delta = "expected next hw_ptr" for in_interrupt != 0
         */
        delta = p_runtime->hw_ptr_interrupt + p_runtime->config.period_size;
        if (delta > new_hw_ptr) {

            /* check for double acknowledged interrupts */
            hdelta = aw_sys_tick_get() - p_runtime->hw_ptr_tick;
            if (hdelta > p_runtime->hw_ptr_buffer_tick / 2) {
                hw_base += p_runtime->buffer_size;
                if (hw_base >= p_runtime->boundary) {
                    hw_base = 0;
                }
                new_hw_ptr = hw_base + pos;
                goto __delta;
            }
        }
    }

    /*
     * new_hw_ptr might be lower than old_hw_ptr in case when
     * pointer crosses the end of the ring buffer
     */
    if (new_hw_ptr < old_hw_ptr) {
        hw_base += p_runtime->buffer_size;
        if (hw_base >= p_runtime->boundary) {
            hw_base = 0;
        }
        new_hw_ptr = hw_base + pos;
    }

__delta:
    delta = new_hw_ptr - old_hw_ptr;
    if (delta < 0) {
        delta += p_runtime->boundary;
    }

    if (p_runtime->no_period_wakeup) {

        /*
         * Without regular period interrupts, we have to check
         * the elapsed time to detect xruns.
         */
        jdelta = aw_sys_tick_get() - p_runtime->hw_ptr_tick;
        if (jdelta < p_runtime->hw_ptr_buffer_tick / 2) {
            goto no_delta_check;
        }

        hdelta = jdelta - delta * aw_sys_clkrate_get() / p_runtime->config.rate;
        while (hdelta > p_runtime->hw_ptr_buffer_tick / 2 + 1) {
            delta   += p_runtime->buffer_size;
            hw_base += p_runtime->buffer_size;
            if (hw_base >= p_runtime->boundary) {
                hw_base = 0;
            }
            new_hw_ptr  = hw_base + pos;
            hdelta     -= p_runtime->hw_ptr_buffer_tick;
        }
        goto no_delta_check;
    }

    /* something must be really wrong */
    if (delta >= p_runtime->buffer_size + p_runtime->config.period_size) {
        AWSA_CORE_DEBUG_ERROR("pcm unexpected hw_pointer value");
        AWSA_CORE_DEBUG_VERBOSE(
            "in_interrupt=%d, pos=%ld, new_hw_ptr=%ld, old_hw_ptr=%ld",
            in_interrupt, pos, new_hw_ptr, old_hw_ptr);
        return AW_OK;
    }

    hdelta = delta;
    if (hdelta < p_runtime->delay) {
        goto no_jiffies_check;
    }

    hdelta -= p_runtime->delay;
    jdelta  = aw_sys_tick_get() - p_runtime->hw_ptr_tick;

    if (((hdelta * aw_sys_clkrate_get()) / p_runtime->config.rate) >
          jdelta + aw_sys_clkrate_get()/100) {

        delta = jdelta /
            (((p_runtime->config.period_size * aw_sys_clkrate_get()) /
               p_runtime->config.rate) + aw_sys_clkrate_get() / 100);

        /* move new_hw_ptr according jiffies not pos variable */
        new_hw_ptr = old_hw_ptr;
        hw_base = delta;

        /* use loop to avoid checks for delta overflows */
        /* the delta value is small or zero in most cases */
        while (delta > 0) {
            new_hw_ptr += p_runtime->config.period_size;
            if (new_hw_ptr >= p_runtime->boundary) {
                new_hw_ptr -= p_runtime->boundary;
            }
            delta--;
        }

        /* align hw_base to buffer_size */
        AWSA_CORE_DEBUG_ERROR("hw_ptr skipping! %s"
                              "(pos=%ld, delta=%ld, period=%ld, "
                              "jdelta=%lu/%lu/%lu, hw_ptr=%ld/%ld)\n",
                              in_interrupt ? "[Q] " : "",
                              (uint32_t)pos,
                              (uint32_t)hdelta,
                              (uint32_t)p_runtime->config.period_size,
                              jdelta,
                              ((hdelta * aw_sys_clkrate_get()) /
                               p_runtime->config.rate),
                               hw_base,
                               (uint32_t)old_hw_ptr,
                               (uint32_t)new_hw_ptr);

        /* reset values to proper state */
        delta   = 0;
        hw_base = new_hw_ptr - (new_hw_ptr % p_runtime->buffer_size);
    }

no_jiffies_check:
    if (delta > p_runtime->config.period_size +
                p_runtime->config.period_size / 2) {
        AWSA_CORE_DEBUG_ERROR("pcm lost interrupts");
        AWSA_CORE_DEBUG_VERBOSE(
            "in_interrupt=%d, pos=%ld, new_hw_ptr=%ld, old_hw_ptr=%ld",
            in_interrupt, pos, new_hw_ptr, old_hw_ptr);
    }

no_delta_check:
    if (p_runtime->hw_ptr == new_hw_ptr) {
        return 0;
    }

    if (p_substream->stream == AWSA_PCM_STREAM_PLAYBACK &&
        p_runtime->silence_size > 0) {
        __core_pcm_lib_playback_silence(p_substream, new_hw_ptr);
    }

    if (in_interrupt) {
        delta = new_hw_ptr - p_runtime->hw_ptr_interrupt;
        if (delta < 0) {
            delta += p_runtime->boundary;
        }
        delta -= (awsa_pcm_uframes_t)delta % p_runtime->config.period_size;
        p_runtime->hw_ptr_interrupt += delta;
        if (p_runtime->hw_ptr_interrupt >= p_runtime->boundary) {
            p_runtime->hw_ptr_interrupt -= p_runtime->boundary;
        }
    }
    p_runtime->hw_ptr_base  = hw_base;
    p_runtime->hw_ptr       = new_hw_ptr;
    p_runtime->hw_ptr_tick  = aw_sys_tick_get();
    __core_pcm_gettime(p_runtime,
                      (struct aw_timespec *)&p_runtime->tstamp);

    return __core_pcm_update_state(p_substream, p_runtime);
}

/**
 * \brief call it with irq disabled
 */
static
aw_err_t __core_pcm_update_hw_ptr (struct awsa_core_pcm_substream *p_substream)
{
    return awsa_core_pcm_lib_update(p_substream, 0);
}

/*****************************************************************************/

/**
 * \brief action operate interface
 */
struct __core_pcm_action_opts {

    /** \brief step1, pre-do */
    aw_err_t (*pfn_action_pre ) (struct awsa_core_pcm_substream *p_substream,
                                 int                             state);

    /** \brief step2 */
    aw_err_t (*pfn_action_do  ) (struct awsa_core_pcm_substream *p_substream,
                                 int                             state);

    /** \brief step3 - cleanup step2 operation */
    void     (*pfn_action_undo) (struct awsa_core_pcm_substream *p_substream,
                                 int                             state);

    /** \brief step3 post-do */
    void     (*pfn_action_post) (struct awsa_core_pcm_substream *p_substream,
                                 int                             state);
};

/**
 * \brief action execution process
 * \note call with stream lock
 */
static
aw_err_t __core_pcm_lib_action (struct __core_pcm_action_opts  *p_opts,
                                struct awsa_core_pcm_substream *p_substream,
                                int                             state)
{
    aw_err_t ret;

    //aw_spinlock_isr_take(&p_substream->lock);

    ret = p_opts->pfn_action_pre(p_substream, state);
    if (ret != AW_OK) {
        //aw_spinlock_isr_give(&p_substream->lock);
        return ret;
    }

    ret = p_opts->pfn_action_do(p_substream, state);
    if (ret == AW_OK) {
        p_opts->pfn_action_post(p_substream, state);
    } else if (p_opts->pfn_action_undo != NULL) {
        p_opts->pfn_action_undo(p_substream, state);
    }

    //aw_spinlock_isr_give(&p_substream->lock);
    return ret;
}

/**
 * \brief action execution process
 * \note call without stream lock
 */
static aw_err_t __core_pcm_lib_action_nolock (
    struct __core_pcm_action_opts  *p_opts,
    struct awsa_core_pcm_substream *p_substream,
    int                             state)
{
    aw_err_t ret;

    ret = p_opts->pfn_action_pre(p_substream, state);
    if (ret != AW_OK) {
        return ret;
    }

    ret = p_opts->pfn_action_do(p_substream, state);
    if (ret == AW_OK) {
        p_opts->pfn_action_post(p_substream, state);
    } else if (p_opts->pfn_action_undo != NULL) {
        p_opts->pfn_action_undo(p_substream, state);
    }

    return ret;
}

/*****************************************************************************/

/**
 * \brief start action callback - prepare
 */
static aw_err_t __core_pcm_lib_start_action_pre (
    struct awsa_core_pcm_substream *p_substream,
    int                             state)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;

    if (p_runtime->state != AWSA_CORE_PCM_STATE_PREPARED) {
        return -AW_EBADF;
    }

    if (p_substream->stream == AWSA_PCM_STREAM_PLAYBACK &&
        !__core_pcm_playback_data(p_substream)) {
        return -AW_EPIPE;
    }

    return AW_OK;
}

/**
 * \brief start action callback - do
 */
static aw_err_t __core_pcm_lib_start_action_do (
    struct awsa_core_pcm_substream *p_substream,
    int                             state)
{
    return p_substream->p_opts->pfn_trigger(p_substream,
                                            AWSA_CORE_PCM_TRIGGER_START);
}

/**
 * \brief start action callback - undo
 */
static void __core_pcm_lib_start_action_undo (
    struct awsa_core_pcm_substream *p_substream,
    int                             state)
{
    p_substream->p_opts->pfn_trigger(p_substream,
                                     AWSA_CORE_PCM_TRIGGER_STOP);
}

/**
 * \brief start action callback - post
 */
static void __core_pcm_lib_start_action_post (
    struct awsa_core_pcm_substream *p_substream,
    int                             state)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;

#if 0
    __core_pcm_trigger_tstamp(p_substream);
#endif
    p_runtime->hw_ptr_tick        = aw_sys_tick_get();
    p_runtime->hw_ptr_buffer_tick = (p_runtime->buffer_size *
                                     aw_sys_clkrate_get())  /
                                     p_runtime->config.rate;
    p_runtime->state = (awsa_core_pcm_state_t)state;
    if (p_substream->stream == AWSA_PCM_STREAM_PLAYBACK &&
        p_runtime->silence_size > 0) {
        __core_pcm_lib_playback_silence(p_substream, ULONG_MAX);
    }
}

/**
 * \brief Start a pcm substream for playback or capture
 */
aw_err_t awsa_core_pcm_lib_start (struct awsa_core_pcm_substream *p_substream)
{
    /**
     * \brief action start
     */
    static
    struct __core_pcm_action_opts __g_pcm_start_action = {
        __core_pcm_lib_start_action_pre,
        __core_pcm_lib_start_action_do,
        __core_pcm_lib_start_action_undo,
        __core_pcm_lib_start_action_post
    };

    return __core_pcm_lib_action(&__g_pcm_start_action,
                                 p_substream,
                                 AWSA_CORE_PCM_STATE_RUNNING);
}

/*****************************************************************************/

/** \brief stop action callback - prepare */
static aw_err_t __core_pcm_lib_stop_action_pre (
    struct awsa_core_pcm_substream *p_substream,
    int                             state)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;

    if (p_runtime->state == AWSA_CORE_PCM_STATE_OPEN) {
        return -AW_EBADF;
    }

    return AW_OK;
}

/** \brief stop action callback - do */
static aw_err_t __core_pcm_lib_stop_action_do (
    struct awsa_core_pcm_substream *p_substream,
    int                             state)
{
    aw_err_t ret = AW_OK;

    if (awsa_core_pcm_is_running(p_substream)) {
        ret = p_substream->p_opts->pfn_trigger(p_substream,
                                               AWSA_CORE_PCM_TRIGGER_STOP);
    }

    return ret; /* unconditonally stop all substreams */
}

/** \brief stop action callback - post */
static void __core_pcm_lib_stop_action_post (
    struct awsa_core_pcm_substream *p_substream,
    int                             state)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;

    if (p_runtime->state != state) {
#if 0
        __core_pcm_trigger_tstamp(p_substream);
#endif

        /* timer notify */
        // need do something

        p_runtime->state = (awsa_core_pcm_state_t)state;
    }
    AW_SEMB_GIVE(p_runtime->sleep);
    AW_SEMB_GIVE(p_runtime->tsleep);
}

static struct __core_pcm_action_opts __g_pcm_stop_action = {
    __core_pcm_lib_stop_action_pre,
    __core_pcm_lib_stop_action_do,
    NULL,
    __core_pcm_lib_stop_action_post
};

/**
 * \brief try to stop all running streams in the substream group
 * \details The state of each stream is then changed to the given
 *          state unconditionally.
 */
aw_err_t awsa_core_pcm_lib_stop (struct awsa_core_pcm_substream *p_substream,
                                 int                             state)
{
    return __core_pcm_lib_action(&__g_pcm_stop_action, p_substream, state);
}

/*****************************************************************************/

/** \brief reset action callback - prepare */
static aw_err_t __core_pcm_lib_reset_action_pre (
    struct awsa_core_pcm_substream *p_substream,
    int                             state)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;

    switch (p_runtime->state) {

    case AWSA_CORE_PCM_STATE_RUNNING:
    case AWSA_CORE_PCM_STATE_PREPARED:
    case AWSA_CORE_PCM_STATE_PAUSED:
    case AWSA_CORE_PCM_STATE_SUSPENDED:
        return AW_OK;

    default:
        return -AW_EBADF;
    }
}

/** \brief reset action callback - do */
static aw_err_t __core_pcm_lib_reset_action_do (
    struct awsa_core_pcm_substream *p_substream,
    int                             state)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;
    aw_err_t                      err;

    err = p_substream->p_opts->pfn_ioctl(p_substream,
                                         AWSA_CORE_PCM_IOCTL_RESET,
                                         NULL);
    if (err != AW_OK) {
        return err;
    }

    p_runtime->hw_ptr           = 0;
    p_runtime->hw_ptr_base      = 0;
    p_runtime->hw_ptr_interrupt = p_runtime->hw_ptr -
                                  p_runtime->hw_ptr %
                                  p_runtime->config.period_size;
    p_runtime->silence_start    = p_runtime->hw_ptr;
    p_runtime->silence_filled   = 0;
    return AW_OK;
}

/** \brief reset action callback - post */
static void __core_pcm_lib_reset_action_post (
    struct awsa_core_pcm_substream *p_substream,
    int                             state)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;

    p_runtime->appl_ptr = p_runtime->hw_ptr;
    if (p_substream->stream == AWSA_PCM_STREAM_PLAYBACK &&
        p_runtime->silence_size > 0) {
        __core_pcm_lib_playback_silence(p_substream, ULONG_MAX);
    }
}

/** \brief pcm substream reset */
aw_err_t awsa_core_pcm_lib_reset (struct awsa_core_pcm_substream *p_substream)
{
    static
    struct __core_pcm_action_opts pcm_reset_action = {
        __core_pcm_lib_reset_action_pre,
        __core_pcm_lib_reset_action_do,
        NULL,
        __core_pcm_lib_reset_action_post
    };

    return __core_pcm_lib_action(&pcm_reset_action, p_substream, 0);
}

/*****************************************************************************/

/** \brief prepare action callback - prepare */
static aw_err_t __core_pcm_lib_prepare_action_pre (
    struct awsa_core_pcm_substream *p_substream,
    int                             state)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;

    if (p_runtime->state == AWSA_CORE_PCM_STATE_OPEN ||
        p_runtime->state == AWSA_CORE_PCM_STATE_DISCONNECTED) {
        return -AW_EBADF;
    }

    if (awsa_core_pcm_is_running(p_substream)) {
        return -AW_EBUSY;
    }

    return AW_OK;
}

/** \brief prepare action callback - do */
static aw_err_t __core_pcm_lib_prepare_action_do (
    struct awsa_core_pcm_substream *p_substream,
    int                             state)
{
    aw_err_t err;

    err = p_substream->p_opts->pfn_prepare(p_substream);
    if (err < 0) {
        return err;
    }

    return __core_pcm_lib_reset_action_do(p_substream, 0);
}

/** \brief prepare action callback - post */
static void __core_pcm_lib_prepare_action_post (
    struct awsa_core_pcm_substream *p_substream,
    int                             state)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;

    p_runtime->appl_ptr = p_runtime->hw_ptr;
    p_runtime->state    = AWSA_CORE_PCM_STATE_PREPARED;
}

/**
 * \brief prepare the PCM substream to be triggerable
 */
aw_err_t awsa_core_pcm_lib_prepare (struct awsa_core_pcm_substream *p_substream)
{
    static
    struct __core_pcm_action_opts pcm_prepare_action = {
        __core_pcm_lib_prepare_action_pre,
        __core_pcm_lib_prepare_action_do,
        NULL,
        __core_pcm_lib_prepare_action_post
    };

    aw_err_t               ret;
    struct awsa_core_card *p_card = p_substream->p_pcm->p_card;

    AW_MUTEX_LOCK(p_card->power_lock, AW_SEM_WAIT_FOREVER);
    ret = __core_pcm_lib_action_nolock(&pcm_prepare_action,
                                       p_substream,
                                       0);
    AW_MUTEX_UNLOCK(p_card->power_lock);
    return ret;
}

/*****************************************************************************/

/** \brief pause action callback - prepare */
static aw_err_t __core_pcm_lib_pause_action_pre (
    struct awsa_core_pcm_substream *p_substream,
    int                             push)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;

    if (push) {
        if (p_runtime->state != AWSA_CORE_PCM_STATE_RUNNING) {
            return -AW_EBADF;
        }
    } else if (p_runtime->state != AWSA_CORE_PCM_STATE_PAUSED) {
        return -AW_EBADF;
    }

    return AW_OK;
}

/** \brief pause action callback - do */
static aw_err_t __core_pcm_lib_pause_action_do (
    struct awsa_core_pcm_substream *p_substream,
    int                             push)
{

    /*
     * some drivers might use hw_ptr to recover from the pause -
     * update the hw_ptr now
     */
    if (push) {
        __core_pcm_update_hw_ptr(p_substream);
    }

    /*
     * The tick check in __core_pcm_update_hw_ptr*() is done by
     * a delta between the current tick, this gives a large enough
     * delta, effectively to skip the check once.
     */
    p_substream->rt.hw_ptr_tick = aw_sys_tick_get()    -
                                  aw_sys_clkrate_get() * 1000;
    return p_substream->p_opts->pfn_trigger(
                                    p_substream,
                                    push ? AWSA_CORE_PCM_TRIGGER_PAUSE_PUSH :
                                           AWSA_CORE_PCM_TRIGGER_PAUSE_RELEASE);
}

/** \brief pause action callback - undo */
static void __core_pcm_lib_pause_action_undo (
    struct awsa_core_pcm_substream *p_substream,
    int                             push)
{
    p_substream->p_opts->pfn_trigger(
                             p_substream,
                             push ? AWSA_CORE_PCM_TRIGGER_PAUSE_RELEASE :
                                    AWSA_CORE_PCM_TRIGGER_PAUSE_PUSH);
}

/** \brief pause action callback - post */
static void __core_pcm_lib_pause_action_post (
    struct awsa_core_pcm_substream *p_substream,
    int                             push)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;

#if 0
    __core_pcm_trigger_tstamp(p_substream);
#endif

    if (push) {
        p_runtime->state = AWSA_CORE_PCM_STATE_PAUSED;
        // notify timer pause
        AW_SEMB_GIVE(p_runtime->sleep);
        AW_SEMB_GIVE(p_runtime->tsleep);
    } else {
        p_runtime->state = AWSA_CORE_PCM_STATE_RUNNING;
        // notify timer pause
    }
}

/**
 * \brief Push/release the pause for all linked streams.
 */
aw_err_t awsa_core_pcm_lib_pause (struct awsa_core_pcm_substream *p_substream,
                                  int                             push)
{
    static
    struct __core_pcm_action_opts pcm_action_pause = {
        __core_pcm_lib_pause_action_pre,
        __core_pcm_lib_pause_action_do,
        __core_pcm_lib_pause_action_undo,
        __core_pcm_lib_pause_action_post
    };

    return __core_pcm_lib_action(&pcm_action_pause, p_substream, push);
}

/*****************************************************************************/

/** \brief drain action callback - prepare */
static aw_err_t __core_pcm_lib_drain_init_action_pre (
    struct awsa_core_pcm_substream *p_substream,
    int                             state)
{
    return AW_OK;
}

/** \brief drain action callback - do */
static aw_err_t __core_pcm_lib_drain_init_action_do (
    struct awsa_core_pcm_substream *p_substream,
    int                             state)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;

    if (p_substream->stream == AWSA_PCM_STREAM_PLAYBACK) {

        switch (p_runtime->state) {

        case AWSA_CORE_PCM_STATE_PREPARED:

            /* start playback stream if possible */
            if (!__core_pcm_playback_empty(p_substream)) {
                __core_pcm_lib_start_action_do(p_substream,
                                               AWSA_CORE_PCM_STATE_DRAINING);
                __core_pcm_lib_start_action_post(p_substream,
                                                 AWSA_CORE_PCM_STATE_DRAINING);
            }
            break;

        case AWSA_CORE_PCM_STATE_RUNNING:
            p_runtime->state = AWSA_CORE_PCM_STATE_DRAINING;
            break;

        default:
            break;
        }

    } else {
        /* stop running stream */
        if (p_runtime->state == AWSA_CORE_PCM_STATE_RUNNING) {
            int new_state = __core_pcm_capture_avail(p_runtime) > 0 ?
                                                AWSA_CORE_PCM_STATE_DRAINING :
                                                AWSA_CORE_PCM_STATE_SETUP;
            __core_pcm_lib_stop_action_do(p_substream, new_state);
            __core_pcm_lib_stop_action_post(p_substream, new_state);
        }
    }

    return AW_OK;
}

/** \brief drain action callback - post */
static void __core_pcm_lib_drain_init_action_post (
    struct awsa_core_pcm_substream *p_substream,
    int                             state)
{

}

static struct __core_pcm_action_opts __g_pcm_action_drain_init = {
    __core_pcm_lib_drain_init_action_pre,
    __core_pcm_lib_drain_init_action_do,
    NULL,
    __core_pcm_lib_drain_init_action_post
};

/**
 * \brief Drain the stream(s).
 * \details When the substream is linked, sync until the draining
 *          of all playback streams is finished.
 *          After this call, all streams are supposed to be either
 *          SETUP or DRAINING (capture only) state.
 */
aw_err_t awsa_core_pcm_lib_drain (struct awsa_core_pcm_substream *p_substream)
{
    struct awsa_core_card          *p_card;
    struct awsa_core_pcm_runtime   *p_runtime;

    aw_err_t ret = AW_OK, tout;
    int nonblock = 0;

    p_card    = p_substream->p_pcm->p_card;
    p_runtime = &p_substream->rt;

    if (p_runtime->state == AWSA_CORE_PCM_STATE_OPEN) {
        return -AW_EBADF;
    }

    AW_MUTEX_LOCK(p_card->power_lock, AW_SEM_WAIT_FOREVER);
    //aw_spinlock_isr_take(&p_substream->lock);

    /* resume pause */
    if (p_runtime->state == AWSA_CORE_PCM_STATE_PAUSED) {
        awsa_core_pcm_lib_pause(p_substream, 0);
    }

    /* pre-start/stop - all running streams are changed to DRAINING state */
    ret = __core_pcm_lib_action(&__g_pcm_action_drain_init, p_substream, 0);
    if (ret != AW_OK) {
        goto __unlock;
    }

    /* in non-blocking, we don't wait in ioctl but let caller poll */
    if (nonblock) {
        ret = -AW_EAGAIN;
        goto __unlock;
    }


    if (p_substream->stream != AWSA_PCM_STREAM_PLAYBACK) {
        goto __unlock;
    }

    if (p_runtime->state != AWSA_CORE_PCM_STATE_DRAINING) {
        goto __unlock;
    }

    //aw_spinlock_isr_give(&p_substream->lock);
    AW_MUTEX_UNLOCK(p_card->power_lock);
    tout = AW_SEMB_TAKE(p_runtime->sleep,
                        aw_ms_to_ticks(10 * aw_sys_clkrate_get()));
    AW_MUTEX_LOCK(p_card->power_lock, AW_SEM_WAIT_FOREVER);
    //aw_spinlock_isr_take(&p_substream->lock);
    if (tout != AW_OK) {
        if (p_runtime->state == AWSA_CORE_PCM_STATE_SUSPENDED) {
            ret = -AW_ESPIPE;
        } else {
            AWSA_CORE_DEBUG_ERROR(("pcm playback drain error"));
            awsa_core_pcm_lib_stop(p_substream, AWSA_CORE_PCM_STATE_SETUP);
            ret = -AW_EIO;
        }
    }

__unlock:
    //aw_spinlock_isr_give(&p_substream->lock);
    AW_MUTEX_UNLOCK(p_card->power_lock);
    return ret;
}

/**
 * \brief stop the DMA only when the given stream is playback
 */
aw_err_t awsa_core_pcm_lib_drain_done (
    struct awsa_core_pcm_substream *p_substream)
{
    return __core_pcm_lib_action(&__g_pcm_stop_action,
                                 p_substream,
                                 AWSA_CORE_PCM_STATE_SETUP);
}

/*****************************************************************************/

/**
 * \brief sanity-check for read/write methods
 */
static
aw_err_t __core_pcm_sanity_check (struct awsa_core_pcm_substream *p_substream)
{
    if (p_substream == NULL || p_substream->rt.p_dma_area == NULL) {
        return -AW_ENXIO;
    }

    if (p_substream->rt.state == AWSA_CORE_PCM_STATE_OPEN) {
        return -AW_EBADF;
    }

    return AW_OK;
}

/**
 * \brief transfer
 */
typedef aw_err_t (*pfn_transfer) (struct awsa_core_pcm_substream *p_substream,
                                  uint32_t                        hwoff,
                                  uint32_t                        data,
                                  awsa_pcm_uframes_t              off,
                                  awsa_pcm_uframes_t              size);

/**
 * \brief write data
 */
static aw_err_t __core_pcm_lib_write_transfer (
    struct awsa_core_pcm_substream *p_substream,
    uint32_t                        hwoff,
    uint32_t                        data,
    uint32_t                        off,
    awsa_pcm_uframes_t              frames)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;
    aw_err_t                      ret       = AW_OK;

    uint8_t *p_buf = (uint8_t *)data +
                      __core_pcm_frames_to_bytes(p_runtime, off);

    if (p_substream->p_opts->pfn_copy != NULL) {
        ret = p_substream->p_opts->pfn_copy(p_substream,
                                            -1,
                                            hwoff,
                                            p_buf,
                                            frames);
    } else {
        uint8_t *p_hwbuf = p_runtime->p_dma_area +
                           __core_pcm_frames_to_bytes(p_runtime, hwoff);
        memcpy(p_hwbuf, p_buf, __core_pcm_frames_to_bytes(p_runtime, frames));
    }

    return ret;
}

/**
 * \brief Wait until avail_min data becomes available
 */
static aw_err_t __core_pcm_wait_for_avail (
    struct awsa_core_pcm_substream *p_substream,
    awsa_pcm_uframes_t             *p_avail)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;

    int is_playback = p_substream->stream == AWSA_PCM_STREAM_PLAYBACK;
    aw_err_t err    = AW_OK;

    awsa_pcm_uframes_t avail = 0;
    aw_err_t           tout;

    AW_FOREVER {

        aw_spinlock_isr_give(&p_substream->lock);
        tout = AW_SEMB_TAKE(p_runtime->tsleep, aw_ms_to_ticks(10000));
        aw_spinlock_isr_take(&p_substream->lock);

        switch (p_runtime->state) {

        case AWSA_CORE_PCM_STATE_SUSPENDED:
            err = -AW_ESPIPE;
            goto _endloop;

        case AWSA_CORE_PCM_STATE_XRUN:
            err = -AW_EPIPE;
            goto _endloop;

        case AWSA_CORE_PCM_STATE_DRAINING:
            if (is_playback) {
                err = -AW_EPIPE;
            } else {
                avail = 0; /* indicate draining */
            }
            goto _endloop;

        case AWSA_CORE_PCM_STATE_OPEN:
        case AWSA_CORE_PCM_STATE_SETUP:
        case AWSA_CORE_PCM_STATE_DISCONNECTED:
            err = -AW_EBADF;
            goto _endloop;

        default:
            break;
        }

        if (tout != AW_OK) {
            AWSA_CORE_DEBUG_ERROR(
                "pcm %s write error", is_playback ? "playback" :
                                                    "capture");
            err = -AW_EIO;
            break;
        }

        if (is_playback) {
            avail = __core_pcm_playback_avail(p_runtime);
        } else {
            avail = __core_pcm_capture_avail(p_runtime);
        }

        if (avail >= p_runtime->twake) {
            break;
        }
    }

_endloop:
    *p_avail = avail;
    return err;
}

/**
 * \brief write1
 */
static awsa_pcm_sframes_t __core_pcm_lib_write1 (
    struct awsa_core_pcm_substream *p_substream,
    uint32_t                        data,
    awsa_pcm_uframes_t              size,
    int                             nonblock,
    pfn_transfer                    transfer)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;
    awsa_pcm_uframes_t            xfer      = 0;
    awsa_pcm_uframes_t            offset    = 0;
    aw_err_t                      err       = AW_OK;

    if (size == 0) {
        return 0;
    }

    aw_spinlock_isr_take(&p_substream->lock);
    switch (p_runtime->state) {

    case AWSA_CORE_PCM_STATE_PREPARED:
    case AWSA_CORE_PCM_STATE_RUNNING:
    case AWSA_CORE_PCM_STATE_PAUSED:
        break;

    case AWSA_CORE_PCM_STATE_XRUN:
        err = -AW_EPIPE;
        goto _end_unlock;

    case AWSA_CORE_PCM_STATE_SUSPENDED:
        err = -AW_ESPIPE;
        goto _end_unlock;

    default:
        err = -AW_EBADF;
        goto _end_unlock;
    }

    p_runtime->twake = p_runtime->config.avail_min ?
                       p_runtime->config.avail_min : 1;
    while (size > 0) {
        awsa_pcm_uframes_t frames, appl_ptr, appl_ofs;
        awsa_pcm_uframes_t avail;
        awsa_pcm_uframes_t cont;

        if (p_runtime->state == AWSA_CORE_PCM_STATE_RUNNING) {
            __core_pcm_update_hw_ptr(p_substream);
        }

        avail = __core_pcm_playback_avail(p_runtime);
        if (avail == 0) {
            if (nonblock) {
                err = -AW_EAGAIN;
                goto _end_unlock;
            }
            p_runtime->twake = min(size,
                                   p_runtime->config.avail_min ? : 1);
            err = __core_pcm_wait_for_avail(p_substream, &avail);
            if (err != AW_OK) {
                goto _end_unlock;
            }
        }
        frames = size > avail ? avail : size;
        cont = p_runtime->buffer_size -
               p_runtime->appl_ptr    %
               p_runtime->buffer_size;
        if (frames > cont) {
            frames = cont;
        }

        if (frames == 0) {
            p_runtime->twake = 0;
            aw_spinlock_isr_give(&p_substream->lock);
            return -AW_EINVAL;
        }

        appl_ptr = p_runtime->appl_ptr;
        appl_ofs = appl_ptr % p_runtime->buffer_size;
        aw_spinlock_isr_give(&p_substream->lock);
        err = transfer(p_substream, appl_ofs, data, offset, frames);
        aw_spinlock_isr_take(&p_substream->lock);
        if (err != AW_OK) {
            goto _end_unlock;
        }

        switch (p_runtime->state) {

        case AWSA_CORE_PCM_STATE_XRUN:
            err = -AW_EPIPE;
            goto _end_unlock;

        case AWSA_CORE_PCM_STATE_SUSPENDED:
            err = -AW_ESPIPE;
            goto _end_unlock;

        default:
            break;
        }

        appl_ptr += frames;
        if (appl_ptr >= p_runtime->boundary) {
            appl_ptr -= p_runtime->boundary;
        }

        p_runtime->appl_ptr = appl_ptr;
        if (p_substream->p_opts->pfn_ack != NULL) {
            p_substream->p_opts->pfn_ack(p_substream);
        }

        offset += frames;
        size   -= frames;
        xfer   += frames;
        if (p_runtime->state == AWSA_CORE_PCM_STATE_PREPARED &&
            __core_pcm_playback_hw_avail(p_runtime) >=
            (awsa_pcm_sframes_t)p_runtime->config.start_threshold) {
            err = awsa_core_pcm_lib_start(p_substream);
            if (err != AW_OK) {
                goto _end_unlock;
            }
        }
    }

_end_unlock:
    p_runtime->twake = 0;
    if (xfer > 0 && err >= 0) {
        __core_pcm_update_state(p_substream, p_runtime);
    }
    aw_spinlock_isr_give(&p_substream->lock);

    return xfer > 0 ? (awsa_pcm_sframes_t)xfer : err;
}

/**
 * \brief read transfer
 */
static aw_err_t __core_pcm_lib_read_transfer (
    struct awsa_core_pcm_substream *p_substream,
    uint32_t                        hwoff,
    uint32_t                        data,
    uint32_t                        off,
    awsa_pcm_uframes_t              frames)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;
    aw_err_t                      ret       = AW_OK;

    uint8_t *p_buf = (uint8_t *)data +
                      __core_pcm_frames_to_bytes(p_runtime, off);

    if (p_substream->p_opts->pfn_copy != NULL) {
        ret = p_substream->p_opts->pfn_copy(p_substream,
                                            -1,
                                            hwoff,
                                            p_buf,
                                            frames);
    } else {
        uint8_t *p_hwbuf = p_runtime->p_dma_area +
                           __core_pcm_frames_to_bytes(p_runtime, hwoff);
        memcpy(p_buf, p_hwbuf, __core_pcm_frames_to_bytes(p_runtime, frames));
    }

    return ret;
}

static awsa_pcm_sframes_t __core_pcm_lib_read1 (
    struct awsa_core_pcm_substream *p_substream,
    uint32_t                        data,
    awsa_pcm_uframes_t              size,
    int                             nonblock,
    pfn_transfer                    transfer)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;
    awsa_pcm_uframes_t            xfer      = 0;
    awsa_pcm_uframes_t            offset    = 0;
    int                           err       = 0;

    if (size == 0) {
        return 0;
    }

    aw_spinlock_isr_take(&p_substream->lock);
    switch (p_runtime->state) {

    case AWSA_CORE_PCM_STATE_PREPARED:
        if (size >= p_runtime->config.start_threshold) {
            err = awsa_core_pcm_lib_start(p_substream);
            if (err != AW_OK) {
                goto __end_unlock;
            }
        }
        break;

    case AWSA_CORE_PCM_STATE_DRAINING:
    case AWSA_CORE_PCM_STATE_RUNNING:
    case AWSA_CORE_PCM_STATE_PAUSED:
        break;

    case AWSA_CORE_PCM_STATE_XRUN:
        err = -AW_EPIPE;
        goto __end_unlock;

    case AWSA_CORE_PCM_STATE_SUSPENDED:
        err = -AW_ESPIPE;
        goto __end_unlock;

    default:
        err = -AW_EBADF;
        goto __end_unlock;
    }

    p_runtime->twake = p_runtime->config.avail_min ?
                       p_runtime->config.avail_min : 1;
    while (size > 0) {
        awsa_pcm_uframes_t frames, appl_ptr, appl_ofs;
        awsa_pcm_uframes_t avail;
        awsa_pcm_uframes_t cont;

        if (p_runtime->state == AWSA_CORE_PCM_STATE_RUNNING) {
            __core_pcm_update_hw_ptr(p_substream);
        }
        avail = __core_pcm_capture_avail(p_runtime);
        if (avail == 0) {
            if (p_runtime->state == AWSA_CORE_PCM_STATE_DRAINING) {
                awsa_core_pcm_lib_stop(p_substream, AWSA_CORE_PCM_STATE_SETUP);
                goto __end_unlock;
            }
            if (nonblock) {
                err = -AW_EAGAIN;
                goto __end_unlock;
            }
            p_runtime->twake = min(size,
                                   p_runtime->config.avail_min ? : 1);
            err = __core_pcm_wait_for_avail(p_substream, &avail);
            if (err != AW_OK) {
                goto __end_unlock;
            }

            if (!avail) {
                continue; /* draining */
            }
        }
        frames = size > avail ? avail : size;
        cont = p_runtime->buffer_size - p_runtime->appl_ptr %
                                        p_runtime->buffer_size;
        if (frames > cont) {
            frames = cont;
        }

        if (frames == 0) {
            p_runtime->twake = 0;
            aw_spinlock_isr_give(&p_substream->lock);
            return -AW_EINVAL;
        }

        appl_ptr = p_runtime->appl_ptr;
        appl_ofs = appl_ptr % p_runtime->buffer_size;
        aw_spinlock_isr_give(&p_substream->lock);
        err = transfer(p_substream, appl_ofs, data, offset, frames);
        aw_spinlock_isr_take(&p_substream->lock);
        if (err != AW_OK) {
            goto __end_unlock;
        }

        switch (p_runtime->state) {

        case AWSA_CORE_PCM_STATE_XRUN:
            err = -AW_EPIPE;
            goto __end_unlock;

        case AWSA_CORE_PCM_STATE_SUSPENDED:
            err = -AW_ESPIPE;
            goto __end_unlock;

        default:
            break;
        }

        appl_ptr += frames;
        if (appl_ptr >= p_runtime->boundary) {
            appl_ptr -= p_runtime->boundary;
        }
        p_runtime->appl_ptr = appl_ptr;
        if (p_substream->p_opts->pfn_ack) {
            p_substream->p_opts->pfn_ack(p_substream);
        }

        offset += frames;
        size   -= frames;
        xfer   += frames;
    }

__end_unlock:
    p_runtime->twake = 0;
    if (xfer > 0 && err >= 0) {
        __core_pcm_update_state(p_substream, p_runtime);
    }
    aw_spinlock_isr_give(&p_substream->lock);

    return xfer > 0 ? (awsa_pcm_sframes_t)xfer : err;
}

/**
 * \brief read pcm substream
 */
awsa_pcm_sframes_t awsa_core_pcm_lib_read (
    struct awsa_core_pcm_substream *p_substream,
    void                           *p_buf,
    awsa_pcm_uframes_t              size)
{
    aw_err_t err;

    err = __core_pcm_sanity_check(p_substream);
    if (err != AW_OK) {
        return 0;
    }

    return __core_pcm_lib_read1(p_substream,
                                (uint32_t)p_buf,
                                size,
                                0,
                                __core_pcm_lib_read_transfer);
}

/**
 * \brief write pcm substream
 */
awsa_pcm_sframes_t awsa_core_pcm_lib_write (
    struct awsa_core_pcm_substream *p_substream,
    const void                     *p_buf,
    awsa_pcm_uframes_t              size)
{
    aw_err_t err;

    err = __core_pcm_sanity_check(p_substream);
    if (err != AW_OK) {
        return 0;
    }

    return __core_pcm_lib_write1(p_substream,
                                 (uint32_t)p_buf,
                                 size,
                                 0,
                                 __core_pcm_lib_write_transfer);
}

/**
 * \brief update pcm substream
 */
aw_err_t awsa_core_pcm_lib_drop (struct awsa_core_pcm_substream *p_substream)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;
    aw_err_t                      ret       = AW_OK;

    if (p_substream == NULL) {
        return -AW_ENXIO;
    }

    if (p_runtime->state == AWSA_CORE_PCM_STATE_OPEN         ||
        p_runtime->state == AWSA_CORE_PCM_STATE_DISCONNECTED ||
        p_runtime->state == AWSA_CORE_PCM_STATE_SUSPENDED) {
        return -AW_EBADF;
    }

    //aw_spinlock_isr_take(&p_substream->lock);

    /* resume pause */
    if (p_runtime->state == AWSA_CORE_PCM_STATE_PAUSED) {
        ret = awsa_core_pcm_lib_pause(p_substream, 0);
        if (ret != AW_OK) {
            return ret;
        }
    }

    ret = awsa_core_pcm_lib_stop(p_substream, AWSA_CORE_PCM_STATE_SETUP);
    if (ret != AW_OK) {
        return ret;
    }

    p_runtime->appl_ptr = p_runtime->hw_ptr;
    //aw_spinlock_isr_give(&p_substream->lock);
    return ret;
}

/**
 * \brief delay pcm substream
 */
aw_err_t awsa_core_pcm_lib_delay (struct awsa_core_pcm_substream *p_substream,
                                  awsa_pcm_sframes_t             *p_res)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;
    aw_err_t                      ret       = AW_OK;
    awsa_pcm_sframes_t            n         = 0;

    aw_spinlock_isr_take(&p_substream->lock);

    switch (p_runtime->state) {

    case AWSA_CORE_PCM_STATE_DRAINING:

        if (p_substream->stream == AWSA_PCM_STREAM_CAPTURE) {
            goto __badfd;
        }
        break;

    case AWSA_CORE_PCM_STATE_RUNNING:
        if ((ret = __core_pcm_update_hw_ptr(p_substream)) < 0) {
            break;
        }
        /* no break */

    case AWSA_CORE_PCM_STATE_PREPARED:
    case AWSA_CORE_PCM_STATE_SUSPENDED:
        ret = AW_OK;
        if (p_substream->stream == AWSA_PCM_STREAM_PLAYBACK) {
            n = __core_pcm_playback_hw_avail(p_runtime);
        } else {
            n = __core_pcm_capture_avail(p_runtime);
        }
        n += p_runtime->delay;
        break;

    case AWSA_CORE_PCM_STATE_XRUN:
        ret = -AW_EPIPE;
        break;

    default:
__badfd:
        ret = -AW_EBADF;
        break;
    }

    aw_spinlock_isr_give(&p_substream->lock);
    if (ret == AW_OK) {
        *p_res = n;
    }

    return ret;
}

/**
 * \brief update avail frames
 */
aw_err_t awsa_core_pcm_lib_avail_update (
    struct awsa_core_pcm_substream *p_substream)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;
    awsa_pcm_uframes_t            avail;
    aw_err_t                      ret;

    if (p_substream->stream == AWSA_PCM_STREAM_PLAYBACK) {
        avail = __core_pcm_playback_avail(p_runtime);
    } else {
        avail = __core_pcm_capture_avail(p_runtime);
    }

    switch (p_runtime->state) {

    case AWSA_CORE_PCM_STATE_RUNNING:
        if (avail >= p_runtime->config.stop_threshold) {

            ret = __core_pcm_xrun(p_substream);
            if (ret != AW_OK) {
                return ret;
            }

            /*
             * everything is ok, state == AWSA_CORE_PCM_STATE_XRUN
             * at the moment
             */
            return -AW_EPIPE;
        }
        break;

    case AWSA_CORE_PCM_STATE_XRUN:
        return -AW_EPIPE;

    default:
        break;
    }

    return avail;
}

/**
 * \brief substream release
 */
static
aw_err_t __core_pcm_substream_release (
    struct awsa_core_pcm_substream *p_substream)
{
    aw_err_t ret;

    p_substream->ref_count--;
    if (p_substream->ref_count > 0) {
        AWSA_CORE_DEBUG_INFO("soundcard ref_count != 0");
        return -AW_EAGAIN;
    }

    ret = awsa_core_pcm_lib_drop(p_substream);
    if (ret != AW_OK) {
        return ret;
    }

    ret = p_substream->p_opts->pfn_close(p_substream);
    if (ret != AW_OK) {
        return ret;
    }

    if (p_substream->p_opts->pfn_dma_free != NULL) {
        ret = p_substream->p_opts->pfn_dma_free(
                  p_substream, (void *)p_substream->rt.p_dma_area);
        if (ret != AW_OK) {
            return ret;
        }
        p_substream->rt.p_dma_area = NULL;
    }
    p_substream->p_stream->opened_cnt--;

    return AW_OK;
}

/**
 * \brief Release pcm stream
 */
aw_err_t awsa_core_pcm_lib_release (
    struct awsa_core_pcm_substream *p_substream)
{
    struct awsa_core_pcm *p_pcm;
    aw_err_t              ret;

    if (p_substream == NULL) {
        return -AW_ENXIO;
    }

    p_pcm = p_substream->p_pcm;
    AW_MUTEX_LOCK(p_pcm->open_mutex, AW_SEM_WAIT_FOREVER);
    ret = __core_pcm_substream_release(p_substream);
    AW_MUTEX_UNLOCK(p_pcm->open_mutex);
    return ret;
}

/* end of file */
