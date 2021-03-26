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
 * - 1.00 16-11-18  win, first implementation
 * \endinternal
 */

#include "soc/awsa_soc_internal.h"

AW_MUTEX_DECL_STATIC(__g_soc_pcm_mutex);   /**< \brief pcm lock */
AW_MUTEX_DECL(__g_soc_client_mutex);       /**< \brief client lock */

static struct aw_list_head __g_soc_card_list;
static struct aw_list_head __g_soc_plfm_list;
static struct aw_list_head __g_soc_codec_list;

extern struct aw_list_head __g_soc_dai_list;

/**
 * \brief This is a timeout to do a DAPM power down after a stream is closed().
 *        It can be used to eliminate pops between different playback streams,
 *        e.g. between two audio tracks.
 */
static int __g_soc_pm_down_time = 5000;

#if 0

/** \brief common rates */
static uint32_t __g_soc_rates[] = {
    5512,  8000,  11025, 16000,
    22050, 32000, 44100, 48000,
    64000, 88200, 96000, 176400,
    192000
};

#endif

/**
 * \brief create a pcm stream
 */
static aw_err_t __soc_pcm_init (struct awsa_soc_pcm *p_soc_pcm,
                                int                  num);

#if 0

/**
 * \brief determine rate_min/rate_max fields
 */
static void __soc_pcm_hw_rates_limit (struct awsa_core_pcm_runtime *p_runtime)
{
    int i;

    for (i = 0; i < (int)sizeof(__g_soc_rates); i++) {
        if (p_runtime->hw.rates & (1 << i)) {
            p_runtime->hw.rate_min = __g_soc_rates[i];
            break;
        }
    }
    for (i = (int)sizeof(__g_soc_rates) - 1; i >= 0; i--) {
        if (p_runtime->hw.rates & (1 << i)) {
            p_runtime->hw.rate_max = __g_soc_rates[i];
            break;
        }
    }
}

#endif

/**
 * \brief __soc_dai_digital_mute - configure DAI system or master clock.
 * \param[in] p_dai : DAI
 * \param[in] mute  : mute enable
 *
 * \retval AW_OK 成功
 */
static aw_err_t __soc_dai_digital_mute (struct awsa_soc_dai *p_dai, int mute)
{
    if (p_dai->p_driver && p_dai->p_driver->p_opts->pfn_digital_mute) {
        return p_dai->p_driver->p_opts->pfn_digital_mute(p_dai, mute);
    } else {
        return -AW_EINVAL;
    }
}

static aw_err_t __soc_pcm_open (struct awsa_core_pcm_substream *p_substream)
{
    struct awsa_soc_pcm          *p_soc_pcm   = p_substream->p_private_data;
    struct awsa_soc_platform     *p_platform  = p_soc_pcm->p_plfm;
    struct awsa_soc_dai          *p_cpu_dai   = p_soc_pcm->p_cpu_dai;
    struct awsa_soc_dai          *p_codec_dai = p_soc_pcm->p_codec_dai;
    aw_err_t                      ret         = AW_OK;

    AW_MUTEX_LOCK(__g_soc_pcm_mutex, AW_SEM_WAIT_FOREVER);

    /* startup the audio subsystem */
    if (p_cpu_dai->p_driver->p_opts->pfn_startup != NULL) {
        ret = p_cpu_dai->p_driver->p_opts->pfn_startup(p_cpu_dai);
        if (ret < 0) {
            AWSA_SOC_DEBUG_ERROR(
                "can't open cpu %s", p_cpu_dai->p_name);
            goto __out;
        }
    }

    if (p_platform->p_driver->pfn_open != NULL) {
        ret = p_platform->p_driver->pfn_open(p_platform);
        if (ret < 0) {
            AWSA_SOC_DEBUG_ERROR(
                "can't open platform %s", p_platform->p_name);
            goto __platform_err;
        }
    }

    if (p_codec_dai->p_driver->p_opts->pfn_startup) {
        ret = p_codec_dai->p_driver->p_opts->pfn_startup(p_codec_dai);
        if (ret < 0) {
            AWSA_SOC_DEBUG_ERROR(
                "can't open codec %s", p_codec_dai->p_name);
            goto __codec_dai_err;
        }
    }

    if (p_soc_pcm->p_dai_link->p_opts               &&
        p_soc_pcm->p_dai_link->p_opts->pfn_startup) {
        ret = p_soc_pcm->p_dai_link->p_opts->pfn_startup(p_soc_pcm);
        if (ret != AW_OK) {
            AWSA_SOC_DEBUG_ERROR(
                "%s startup failed", p_soc_pcm->p_dai_link->p_name);
            goto __machine_err;
        }
    }

#if 0

    /* Check that the codec and cpu DAIs are compatible */
    if (p_substream->stream == AWSA_PCM_STREAM_PLAYBACK) {

        p_runtime->hw.rate_min     = max(p_codec_dai_drv->playback.rate_min,
                                         p_cpu_dai_drv->playback.rate_min);
        p_runtime->hw.rate_max     = min(p_codec_dai_drv->playback.rate_max,
                                         p_cpu_dai_drv->playback.rate_max);
        p_runtime->hw.channels_min = max(p_codec_dai_drv->playback.channels_min,
                                         p_cpu_dai_drv->playback.channels_min);
        p_runtime->hw.channels_max = min(p_codec_dai_drv->playback.channels_max,
                                         p_cpu_dai_drv->playback.channels_max);

        p_runtime->hw.formats      = p_codec_dai_drv->playback.formats &
                                     p_cpu_dai_drv->playback.formats;
        p_runtime->hw.rates        = p_codec_dai_drv->playback.rates &
                                     p_cpu_dai_drv->playback.rates;

        if (p_codec_dai_drv->playback.rates & (AWSA_PCM_RATE_KNOT         |
                                               AWSA_PCM_RATE_CONTINUOUS)) {
            p_runtime->hw.rates |= p_cpu_dai_drv->playback.rates;
        }

        if (p_cpu_dai_drv->playback.rates & (AWSA_PCM_RATE_KNOT         |
                                             AWSA_PCM_RATE_CONTINUOUS)) {
            p_runtime->hw.rates |= p_codec_dai_drv->playback.rates;
        }
    } else {

        p_runtime->hw.rate_min     = max(p_codec_dai_drv->capture.rate_min,
                                         p_cpu_dai_drv->capture.rate_min);
        p_runtime->hw.rate_max     = min(p_codec_dai_drv->capture.rate_max,
                                         p_cpu_dai_drv->capture.rate_max);
        p_runtime->hw.channels_min = max(p_codec_dai_drv->capture.channels_min,
                                         p_cpu_dai_drv->capture.channels_min);
        p_runtime->hw.channels_max = min(p_codec_dai_drv->capture.channels_max,
                                         p_cpu_dai_drv->capture.channels_max);

        p_runtime->hw.formats      = p_codec_dai_drv->capture.formats &
                                     p_cpu_dai_drv->capture.formats;
        p_runtime->hw.rates        = p_codec_dai_drv->capture.rates &
                                     p_cpu_dai_drv->capture.rates;

        if (p_codec_dai_drv->capture.rates & (AWSA_PCM_RATE_KNOT         |
                                              AWSA_PCM_RATE_CONTINUOUS)) {
            p_runtime->hw.rates |= p_cpu_dai_drv->capture.rates;
        }

        if (p_cpu_dai_drv->capture.rates & (AWSA_PCM_RATE_KNOT |
                                            AWSA_PCM_RATE_CONTINUOUS)) {
            p_runtime->hw.rates |= p_codec_dai_drv->capture.rates;
        }
    }

    __soc_pcm_hw_rates_limit(p_runtime);
    if (!p_runtime->hw.rates) {
        AWSA_SOC_DEBUG_ERROR("%s <-> %s No matching rates",
                             p_codec_dai->p_name,
                             p_cpu_dai->p_name);
        goto __config_err;
    }

    if (!p_runtime->hw.formats) {
        AWSA_SOC_DEBUG_ERROR("%s <-> %s No matching formats",
                             p_codec_dai->p_name,
                             p_cpu_dai->p_name);
        goto __config_err;
    }

    if (!p_runtime->hw.channels_min || !p_runtime->hw.channels_max) {
        AWSA_SOC_DEBUG_ERROR("%s <-> %s No matching channels",
                             p_codec_dai->p_name,
                             p_cpu_dai->p_name);
        goto __config_err;
    }

#endif

#if 0

    /* Symmetry only applies if we've already got an active stream. */
    if (p_cpu_dai->active || p_codec_dai->active) {
        ret = __soc_pcm_symmetry_apply(p_substream);
        if (ret != AW_OK) {
            goto __config_err;
        }
    }
#endif

    AWSA_SOC_DEBUG_VERBOSE("%s <-> %s info:",
                            p_codec_dai->p_name,
                            p_cpu_dai->p_name);

#if 0
    AWSA_SOC_DEBUG_VERBOSE("rate mask 0x%x",
                            p_runtime->hw.rates);

    AWSA_SOC_DEBUG_VERBOSE("min ch %d max ch %d",
                            p_runtime->hw.channels_min,
                            p_runtime->hw.channels_max);

    AWSA_SOC_DEBUG_VERBOSE("min rate %d max rate %d",
                            p_runtime->hw.rate_min,
                            p_runtime->hw.rate_max);
#endif

    if (p_substream->stream == AWSA_PCM_STREAM_PLAYBACK) {
        p_cpu_dai->playback_active++;
        p_codec_dai->playback_active++;
    } else {
        p_cpu_dai->capture_active++;
        p_codec_dai->capture_active++;
    }

    p_cpu_dai->active++;
    p_codec_dai->active++;
    p_soc_pcm->p_codec->active++;

    p_soc_pcm->p_substream = p_substream;
    p_platform->p_soc_pcm  = p_soc_pcm;
    AW_MUTEX_UNLOCK(__g_soc_pcm_mutex);
    return AW_OK;

#if 0
__config_err:
    if (p_soc_pcm->p_dai_link->p_opts               != NULL  &&
        p_soc_pcm->p_dai_link->p_opts->pfn_shutdown != NULL) {
        p_soc_pcm->p_dai_link->p_opts->pfn_shutdown(p_soc_pcm,
                                                    p_substream->stream);
    }
#endif

__machine_err:
    if (p_codec_dai->p_driver->p_opts->pfn_shutdown) {
        p_codec_dai->p_driver->p_opts->pfn_shutdown(p_codec_dai,
                                                    p_substream->stream);
    }

__codec_dai_err:
    if (p_platform->p_driver->pfn_close) {
        p_platform->p_driver->pfn_close(p_platform);
    }

__platform_err:
    if (p_cpu_dai->p_driver->p_opts->pfn_shutdown) {
        p_cpu_dai->p_driver->p_opts->pfn_shutdown(p_cpu_dai,
                                                  p_substream->stream);
    }

__out:
    AW_MUTEX_UNLOCK(__g_soc_pcm_mutex);
    return ret;
}

static aw_err_t __soc_pcm_close (struct awsa_core_pcm_substream *p_substream)
{
    struct awsa_soc_pcm         *p_soc_pcm   = p_substream->p_private_data;
    struct awsa_soc_platform    *p_platform  = p_soc_pcm->p_plfm;
    struct awsa_soc_dai         *p_cpu_dai   = p_soc_pcm->p_cpu_dai;
    struct awsa_soc_dai         *p_codec_dai = p_soc_pcm->p_codec_dai;
    struct awsa_soc_codec       *p_codec     = p_soc_pcm->p_codec;

    AW_MUTEX_LOCK(__g_soc_pcm_mutex, AW_SEM_WAIT_FOREVER);

    if (p_substream->stream == AWSA_PCM_STREAM_PLAYBACK) {
        p_cpu_dai->playback_active--;
        p_codec_dai->playback_active--;
    } else {
        p_cpu_dai->capture_active--;
        p_codec_dai->capture_active--;
    }

    p_cpu_dai->active--;
    p_codec_dai->active--;
    p_codec->active--;

    /*
     * Muting the DAC suppresses artifacts caused during digital
     * shutdown, for example from stopping clocks.
     */
    if (p_substream->stream == AWSA_PCM_STREAM_PLAYBACK) {
        __soc_dai_digital_mute(p_codec_dai, 1);
    }

    if (p_cpu_dai->p_driver->p_opts->pfn_shutdown) {
        p_cpu_dai->p_driver->p_opts->pfn_shutdown(p_cpu_dai,
                                                  p_substream->stream);
    }

    if (p_codec_dai->p_driver->p_opts->pfn_shutdown) {
        p_codec_dai->p_driver->p_opts->pfn_shutdown(p_codec_dai,
                                                    p_substream->stream);
    }

    if (p_soc_pcm->p_dai_link->p_opts               != NULL  &&
        p_soc_pcm->p_dai_link->p_opts->pfn_shutdown != NULL) {
        p_soc_pcm->p_dai_link->p_opts->pfn_shutdown(p_soc_pcm,
                                                    p_substream->stream);
    }

    if (p_platform->p_driver->pfn_close) {
        p_platform->p_driver->pfn_close(p_platform);
    }

    if (p_substream->stream == AWSA_PCM_STREAM_PLAYBACK) {
        /* start delayed pop wq here for playback streams */
        p_codec_dai->pop_wait = 1;
        aw_delayed_work_start(&p_soc_pcm->delayed_work,
                              p_soc_pcm->pmdown_time);
    } else {
        /* capture streams can be powered down now */
        awsa_soc_codec_stream_event(
            p_codec,
            p_codec_dai->p_driver->capture.p_stream_name,
            AWSA_SOC_DAPM_STREAM_STOP);
    }

    AW_MUTEX_UNLOCK(__g_soc_pcm_mutex);
    return AW_OK;
}

/**
 * \brief ioctl
 */
aw_err_t __soc_pcm_ioctl (struct awsa_core_pcm_substream *p_substream,
                          int                             cmd,
                          void                           *p_arg)
{
    return AW_OK;
}

/**
 * \brief Called by AWSA when the hardware params are set by application. This
 *        function can also be called multiple times and can allocate buffers
 *        (using awsa_core_pcm_lib_* ). It's non-atomic.
 */
static
aw_err_t __soc_pcm_config (struct awsa_core_pcm_substream *p_substream,
                           struct awsa_pcm_config         *p_config)
{
    struct awsa_soc_pcm       *p_soc_pcm   = p_substream->p_private_data;
    struct awsa_soc_platform  *p_platform  = p_soc_pcm->p_plfm;
    struct awsa_soc_dai       *p_cpu_dai   = p_soc_pcm->p_cpu_dai;
    struct awsa_soc_dai       *p_codec_dai = p_soc_pcm->p_codec_dai;
    aw_err_t                   ret         = AW_OK;

    AW_MUTEX_LOCK(__g_soc_pcm_mutex, AW_SEM_WAIT_FOREVER);

    if (p_soc_pcm->p_dai_link->p_opts             != NULL  &&
        p_soc_pcm->p_dai_link->p_opts->pfn_config != NULL) {
        ret = p_soc_pcm->p_dai_link->p_opts->pfn_config(p_soc_pcm,
                                                        p_substream->stream,
                                                        p_config);
        if (ret != AW_OK) {
            AWSA_SOC_DEBUG_ERROR("machine config failed");
            goto __out;
        }
    }

    if (p_codec_dai->p_driver->p_opts->pfn_config) {
        ret = p_codec_dai->p_driver->p_opts->pfn_config(p_codec_dai,
                                                        p_substream->stream,
                                                        p_config);
        if (ret != AW_OK) {
            AWSA_SOC_DEBUG_ERROR("can't config codec %s",
                                  p_codec_dai->p_name);
            goto __out;
        }
    }

    if (p_cpu_dai->p_driver->p_opts->pfn_config) {
        ret = p_cpu_dai->p_driver->p_opts->pfn_config(p_cpu_dai,
                                                      p_substream->stream,
                                                      p_config);
        if (ret != AW_OK) {
            AW_DBGF(("awbl soc: interface %s hw params failed\n",
                     p_cpu_dai->p_name));
            goto __out;
        }
    }

    if (p_platform->p_driver->pfn_config) {
        ret = p_platform->p_driver->pfn_config(p_platform,
                                               p_substream->stream,
                                               p_config);
        if (ret != AW_OK) {
            AW_DBGF(("awbl soc: platform %s hw params failed\n",
                     p_platform->p_name));
            goto __out;
        }
    }

    p_soc_pcm->rate = p_config->rate;

__out:
    AW_MUTEX_UNLOCK(__g_soc_pcm_mutex);
    return ret;
}

/**
 * \brief dma memory allocate
 */
static
aw_err_t __soc_pcm_dma_alloc (struct awsa_core_pcm_substream  *p_substream,
                              void                           **p_dma_mem,
                              awsa_pcm_uframes_t               period_size,
                              uint32_t                         period_count)
{
    struct awsa_soc_pcm       *p_soc_pcm   = p_substream->p_private_data;
    struct awsa_soc_platform  *p_platform  = p_soc_pcm->p_plfm;
    aw_err_t                   err;

    awsa_soc_assert(p_platform->p_driver->pfn_dma_alloc != NULL);
    AW_MUTEX_LOCK(__g_soc_pcm_mutex, AW_SEM_WAIT_FOREVER);
    err = p_platform->p_driver->pfn_dma_alloc(p_platform, p_dma_mem);
    AW_MUTEX_UNLOCK(__g_soc_pcm_mutex);
    return err;
}

/**
 * \brief dma memory free
 */
static
aw_err_t __soc_pcm_dma_free (struct awsa_core_pcm_substream  *p_substream,
                              void                           *p_dma_mem)
{
    struct awsa_soc_pcm       *p_soc_pcm   = p_substream->p_private_data;
    struct awsa_soc_platform  *p_platform  = p_soc_pcm->p_plfm;
    aw_err_t                   err;

    awsa_soc_assert(p_platform->p_driver->pfn_dma_alloc != NULL);
    AW_MUTEX_LOCK(__g_soc_pcm_mutex, AW_SEM_WAIT_FOREVER);
    err = p_platform->p_driver->pfn_dma_free(p_platform, p_dma_mem);
    AW_MUTEX_UNLOCK(__g_soc_pcm_mutex);
    return err;
}

/**
 * \brief Called by ALSA when the PCM substream is prepared, can set format, sample
 *        rate, etc.  This function is non atomic and can be called multiple times,
 *        it can refer to the runtime info.
 */
static aw_err_t __soc_pcm_prepare (struct awsa_core_pcm_substream *p_substream)
{
    struct awsa_soc_pcm      *p_soc_pcm   = p_substream->p_private_data;
    struct awsa_soc_platform *p_platform  = p_soc_pcm->p_plfm;
    struct awsa_soc_codec    *p_codec     = p_soc_pcm->p_codec;
    struct awsa_soc_dai      *p_cpu_dai   = p_soc_pcm->p_cpu_dai;
    struct awsa_soc_dai      *p_codec_dai = p_soc_pcm->p_codec_dai;
    aw_err_t                  ret         = AW_OK;

    AW_MUTEX_LOCK(__g_soc_pcm_mutex, AW_SEM_WAIT_FOREVER);

    if (p_soc_pcm->p_dai_link->p_opts              != NULL  &&
        p_soc_pcm->p_dai_link->p_opts->pfn_prepare != NULL) {
        ret = p_soc_pcm->p_dai_link->p_opts->pfn_prepare(p_soc_pcm);
        if (ret != AW_OK) {
            AWSA_CORE_DEBUG_ERROR(("awsa soc: machine prepare error\n"));
            goto __out;
        }
    }

    if (p_platform->p_driver->pfn_prepare) {
        ret = p_platform->p_driver->pfn_prepare(p_platform);
        if (ret != AW_OK) {
            AWSA_CORE_DEBUG_ERROR(("awsa soc: platform prepare error\n"));
            goto __out;
        }
    }

    if (p_codec_dai->p_driver->p_opts->pfn_prepare) {
        ret = p_codec_dai->p_driver->p_opts->pfn_prepare(p_codec_dai);
        if (ret != AW_OK) {
            AWSA_CORE_DEBUG_ERROR(("awsa soc: codec DAI prepare error\n"));
            goto __out;
        }
    }

    if (p_cpu_dai->p_driver->p_opts->pfn_prepare) {
        ret = p_cpu_dai->p_driver->p_opts->pfn_prepare(p_cpu_dai);
        if (ret != AW_OK) {
            AWSA_CORE_DEBUG_ERROR(("awsa soc: cpu DAI prepare error\n"));
            goto __out;
        }
    }

    /* cancel any delayed stream shutdown that is pending */
    if (p_substream->stream == AWSA_PCM_STREAM_PLAYBACK &&
        p_codec_dai->pop_wait) {
        p_codec_dai->pop_wait = 0;
        aw_delayed_work_stop(&p_soc_pcm->delayed_work);
    }

    if (p_substream->stream == AWSA_PCM_STREAM_PLAYBACK) {
        awsa_soc_codec_stream_event(
            p_codec,
            p_codec_dai->p_driver->playback.p_stream_name,
            AWSA_SOC_DAPM_STREAM_START);
    } else {
        awsa_soc_codec_stream_event(
            p_codec,
            p_codec_dai->p_driver->capture.p_stream_name,
            AWSA_SOC_DAPM_STREAM_START);
    }

    __soc_dai_digital_mute(p_codec_dai, 0);

__out:
    AW_MUTEX_UNLOCK(__g_soc_pcm_mutex);
    return ret;
}

#if 0
static aw_err_t __soc_pcm_copy (struct awsa_core_pcm_substream *p_substream,
                                int                             channel,
                                awsa_pcm_uframes_t              pos,
                                void                           *p_buf,
                                awsa_pcm_uframes_t              count)
{
    struct awsa_soc_pcm       *p_soc_pcm   = p_substream->p_private_data;
    struct awsa_soc_platform  *p_platform  = p_soc_pcm->p_plfm;
    aw_err_t                   err         = -AW_ENOTSUP;

    if (p_platform->p_driver->pfn_copy != NULL) {
        err = p_platform->p_driver->pfn_copy(p_platform,
                                             channel,
                                             pos,
                                             p_buf,
                                             count);
    }
    return err;
}
#endif

static aw_err_t __soc_pcm_trigger (struct awsa_core_pcm_substream *p_substream,
                                   int                             cmd)
{
    struct awsa_soc_pcm      *p_soc_pcm   = p_substream->p_private_data;
    struct awsa_soc_platform *p_platform  = p_soc_pcm->p_plfm;
    struct awsa_soc_dai      *p_cpu_dai   = p_soc_pcm->p_cpu_dai;
    struct awsa_soc_dai      *p_codec_dai = p_soc_pcm->p_codec_dai;
    aw_err_t                  ret;

    if (p_codec_dai->p_driver->p_opts->pfn_trigger) {
        ret = p_codec_dai->p_driver->p_opts->pfn_trigger(p_codec_dai, cmd);
        if (ret != AW_OK) {
            return ret;
        }
    }

    if (p_platform->p_driver->pfn_trigger) {
        ret = p_platform->p_driver->pfn_trigger(p_platform, cmd);
        if (ret != AW_OK) {
            return ret;
        }
    }

    if (p_cpu_dai->p_driver->p_opts->pfn_trigger) {
        ret = p_cpu_dai->p_driver->p_opts->pfn_trigger(p_cpu_dai, cmd);
        if (ret != AW_OK) {
            return ret;
        }
    }

    return AW_OK;
}

/**
 * \brief soc level wrapper for pointer callback
 *        If cpu_dai, codec_dai, platform driver has the delay callback, than
 *        the runtime->delay will be updated accordingly.
 */
static
awsa_pcm_uframes_t __soc_pcm_pointer (
    struct awsa_core_pcm_substream *p_substream)
{
    struct awsa_soc_pcm          *p_soc_pcm   = p_substream->p_private_data;
    struct awsa_soc_platform     *p_platform  = p_soc_pcm->p_plfm;
    struct awsa_soc_dai          *p_cpu_dai   = p_soc_pcm->p_cpu_dai;
    struct awsa_soc_dai          *p_codec_dai = p_soc_pcm->p_codec_dai;
    struct awsa_core_pcm_runtime *p_runtime   = &p_substream->rt;
    awsa_pcm_uframes_t            offset      = 0;
    awsa_pcm_sframes_t            delay       = 0;

    if (p_platform->p_driver->pfn_pointer != NULL) {
        offset = p_platform->p_driver->pfn_pointer(p_platform);
    }

    if (p_cpu_dai->p_driver->p_opts->pfn_delay != NULL) {
        delay += p_cpu_dai->p_driver->p_opts->pfn_delay(p_cpu_dai);
    }

    if (p_codec_dai->p_driver->p_opts->pfn_delay != NULL) {
        delay += p_codec_dai->p_driver->p_opts->pfn_delay(p_codec_dai);
    }

    if (p_platform->p_driver->pfn_delay != NULL) {
        delay += p_platform->p_driver->pfn_delay(p_platform, p_codec_dai);
    }

    p_runtime->delay = delay;
    return offset;
}

/**
 * \brief soc silence
 */
static
aw_err_t __soc_pcm_silence (struct awsa_core_pcm_substream *p_substream,
                            int                             channel,
                            awsa_pcm_uframes_t              pos,
                            awsa_pcm_uframes_t              count)
{
    struct awsa_soc_pcm       *p_soc_pcm   = p_substream->p_private_data;
    struct awsa_soc_platform  *p_platform  = p_soc_pcm->p_plfm;
    aw_err_t                   err         = -AW_ENOTSUP;

    if (p_platform->p_driver->pfn_silence != NULL) {
        err = p_platform->p_driver->pfn_silence(p_platform,
                                                channel,
                                                pos,
                                                count);
    }
    return err;
}

/**
 * \brief Power down the audio subsystem pmdown_time msecs after
 *        close is called.
 */
static void __soc_delayed_work_close (void *p_arg)
{
    struct awsa_soc_pcm   *p_soc_pcm   = p_arg;
    struct awsa_soc_codec *p_codec     = p_soc_pcm->p_codec;
    struct awsa_soc_dai   *p_codec_dai = p_soc_pcm->p_codec_dai;

    AW_MUTEX_LOCK(__g_soc_pcm_mutex, AW_SEM_WAIT_FOREVER);

    AWSA_SOC_DEBUG_VERBOSE(
        "%s status: %s waiting: %s",
        p_codec_dai->p_driver->playback.p_stream_name,
        p_codec_dai->playback_active ? "active" : "inactive",
        p_codec_dai->pop_wait        ? "yes"    : "no");

    /* are we waiting on this codec DAI stream */
    if (p_codec_dai->pop_wait == AW_TRUE) {
        p_codec_dai->pop_wait = AW_FALSE;
        awsa_soc_codec_stream_event(
            p_codec,
            p_codec_dai->p_driver->playback.p_stream_name,
            AWSA_SOC_DAPM_STREAM_STOP);
    }

    AW_MUTEX_UNLOCK(__g_soc_pcm_mutex);
}

/** \brief soc pcm operations */
static struct awsa_core_pcm_servopts __g_soc_pcm_opts = {
    __soc_pcm_open,
    __soc_pcm_close,
    __soc_pcm_ioctl,
    __soc_pcm_config,
    __soc_pcm_dma_alloc,
    __soc_pcm_dma_free,
    __soc_pcm_prepare,
    NULL,
    __soc_pcm_trigger,
    __soc_pcm_pointer,
    __soc_pcm_silence,
    NULL,
};

/**
 * \brief dai link
 */
static aw_err_t __soc_dai_link_bind (struct awsa_soc_card *p_card, int num)
{
    const
    struct awsa_soc_dai_link  *p_dai_link  = &p_card->p_driver->p_dai_link[num];
    struct awsa_soc_pcm       *p_soc_pcm   = &p_card->p_soc_pcm[num];
    struct awsa_soc_codec     *p_codec     = NULL;
    struct awsa_soc_platform  *p_platform  = NULL;
    struct awsa_soc_dai       *p_codec_dai = NULL, *p_cpu_dai = NULL;

    if (p_soc_pcm->complete) {
        return AW_OK;
    }

    AWSA_SOC_DEBUG_INFO("binding %s at idx %d",
                        p_dai_link->p_name,
                        num);

    /* do we already have the CPU DAI for this link ? */
    if (p_soc_pcm->p_cpu_dai != NULL) {
        goto __find_codec;
    }

    /* no, then find CPU DAI from registered DAIs*/
    aw_list_for_each_entry(p_cpu_dai,
                           &__g_soc_dai_list,
                           struct awsa_soc_dai,
                           list) {

        if (!strcmp(p_cpu_dai->p_name, p_dai_link->p_cpu_dai_name)) {

            p_soc_pcm->p_cpu_dai = p_cpu_dai;
            goto __find_codec;
        }
    }

    AWSA_SOC_DEBUG_WARN("cpu dai %s not registered",
                        p_dai_link->p_cpu_dai_name);

__find_codec:

    /* do we already have the CODEC for this link ? */
    if (p_soc_pcm->p_codec != NULL) {
        goto __find_platform;
    }

    /* no, then find CODEC from registered CODECs*/
    aw_list_for_each_entry(p_codec,
                           &__g_soc_codec_list,
                           struct awsa_soc_codec,
                           list) {

        if (!strcmp(p_codec->p_name, p_dai_link->p_codec_name)) {
            p_soc_pcm->p_codec = p_codec;

            /*
             * codec found, so find codec dai
             * from registered dais from this codec
             */
            aw_list_for_each_entry(p_codec_dai,
                                   &__g_soc_dai_list,
                                   struct awsa_soc_dai,
                                   list) {
                if (!strcmp(p_codec_dai->p_name,
                            p_dai_link->p_codec_dai_name)) {
                    p_soc_pcm->p_codec_dai = p_codec_dai;
                    goto __find_platform;
                }
            }

            AWSA_SOC_DEBUG_WARN("codec dai %s not registered",
                                p_dai_link->p_codec_dai_name);

            goto __find_platform;
        }
    }

    AWSA_SOC_DEBUG_WARN("codec %s not registered", p_dai_link->p_codec_name);

__find_platform:

    /* do we already have the CODEC DAI for this link ? */
    if (p_soc_pcm->p_plfm != NULL) {
        goto __out;
    }

    /* no, then find CPU DAI from registered DAIs */
    aw_list_for_each_entry(p_platform,
                           &__g_soc_plfm_list,
                           struct awsa_soc_platform,
                           list) {

        if (!strcmp(p_platform->p_name, p_dai_link->p_plfm_name)) {
            p_soc_pcm->p_plfm = p_platform;
            goto __out;
        }
    }

    AWSA_SOC_DEBUG_WARN("platform %s not registered", p_dai_link->p_plfm_name);
    return AW_OK;

__out:

    /*
     * mark rtd as complete if we found all 4 of our client devices
     */
    if ((p_soc_pcm->p_codec     != NULL)  &&
        (p_soc_pcm->p_codec_dai != NULL)  &&
        (p_soc_pcm->p_plfm      != NULL)  &&
        (p_soc_pcm->p_cpu_dai   != NULL)) {
        p_soc_pcm->complete   = 1;
        p_soc_pcm->p_dai_link = &p_card->p_driver->p_dai_link[num];
        p_card->num_pcm++;
    }

    return AW_OK;
}

/**
 * \brief remove codec resource
 */
static void __soc_codec_remove (struct awsa_soc_codec *p_codec)
{
    aw_err_t err;

    if (p_codec->p_driver->pfn_remove) {
        err = p_codec->p_driver->pfn_remove(p_codec);
        if (err != AW_OK) {
            AWSA_SOC_DEBUG_ERROR("failed to remove %s: %d",
                                 p_codec->p_name, err);
        }
    }

    /* Make sure all dapm widgets are freed */
    awsa_soc_dapm_free(&p_codec->dapm);
    p_codec->probed = 0;
    //aw_list_del(&p_codec->card_list);
}

/**
 * \brief remove dai link
 */
static void __soc_dai_link_remove (struct awsa_soc_card *p_card, int num)
{
    struct awsa_soc_pcm      *p_soc_pcm   = &p_card->p_soc_pcm[num];
    struct awsa_soc_codec    *p_codec     = p_soc_pcm->p_codec;
    struct awsa_soc_platform *p_platform  = p_soc_pcm->p_plfm;
    struct awsa_soc_dai      *p_codec_dai = p_soc_pcm->p_codec_dai;
    struct awsa_soc_dai      *p_cpu_dai   = p_soc_pcm->p_cpu_dai;

    aw_err_t ret;

    /* unregister the rtd device */
    if (p_soc_pcm->dev_registered) {
        p_soc_pcm->dev_registered = AW_FALSE;
    }

    /* remove the CODEC DAI */
    if (p_codec_dai && p_codec_dai->probed) {
        if (p_codec_dai->p_driver->pfn_remove) {
            ret = p_codec_dai->p_driver->pfn_remove(p_codec_dai);
            if (ret != AW_OK) {
                AWSA_SOC_DEBUG_WARN("failed to remove %s",
                                    p_codec_dai->p_name);
            }
        }
        p_codec_dai->probed = 0;
        //aw_list_del(&p_codec_dai->card_list);
    }

    /* remove the platform */
    if (p_platform && p_platform->probed) {
        if (p_platform->p_driver->pfn_remove) {
            ret = p_platform->p_driver->pfn_remove(p_platform);
            if (ret != AW_OK) {
                AWSA_SOC_DEBUG_WARN("failed to remove %s", p_platform->p_name);
            }
        }
        p_platform->probed = 0;
        //aw_list_del(&p_platform->card_list);
    }

    /* remove the CODEC */
    if (p_codec && p_codec->probed) {
        __soc_codec_remove(p_codec);
    }

    /* remove the cpu_dai */
    if (p_cpu_dai && p_cpu_dai->probed) {
        if (p_cpu_dai->p_driver->pfn_remove) {
            ret = p_cpu_dai->p_driver->pfn_remove(p_cpu_dai);
            if (ret != AW_OK) {
                AWSA_SOC_DEBUG_WARN("failed to remove %s",
                                    p_cpu_dai->p_name);
            }
        }
        p_cpu_dai->probed = 0;
        //aw_list_del(&p_cpu_dai->card_list);
    }
}

/**
 * \brief codec probe
 */
static aw_err_t __soc_codec_probe (struct awsa_soc_card  *p_card,
                                   struct awsa_soc_codec *p_codec)
{
    aw_err_t ret         = AW_OK;

    p_codec->p_card      = p_card;
    p_codec->dapm.p_card = p_card;

    if (p_codec->p_driver->pfn_probe) {
        ret = p_codec->p_driver->pfn_probe(p_codec);
        if (ret != AW_OK) {
            AWSA_SOC_DEBUG_ERROR("failed to probe codec %s: %d",
                                 p_codec->p_name, ret);
            return ret;
        }
    }

    p_codec->probed = 1;
    //aw_list_add(&p_codec->card_list, &p_card->codec_dev_list);
    aw_list_add(&p_codec->dapm.list, &p_card->dapm_list);
    return ret;
}

/**
 * \brief component initilize
 */
static aw_err_t __soc_post_component_init (struct awsa_soc_card  *p_card,
                                           struct awsa_soc_codec *p_codec,
                                           int                    num,
                                           aw_bool_t              dai_less)
{
    const
    struct awsa_soc_dai_link *p_dai_link = NULL;
    struct awsa_soc_aux_dev  *p_aux_dev  = NULL;
    struct awsa_soc_pcm      *p_soc_pcm;
    const char               *p_name;
    aw_err_t                  ret        = AW_OK;

    if (!dai_less) {
        p_dai_link        = &p_card->p_driver->p_dai_link[num];
        p_soc_pcm         = &p_card->p_soc_pcm[num];
        p_name            = p_dai_link->p_name;
        p_soc_pcm->p_card = p_card;

        /* do machine specific initialization */
        if (p_dai_link->pfn_init != NULL) {
            ret = p_dai_link->pfn_init(p_soc_pcm);
            if (ret != AW_OK) {
                AWSA_SOC_DEBUG_ERROR("failed to init %s: %d",
                                     p_name,
                                     ret);
                return ret;
            }
        }

        /* register the rtd device */
        p_soc_pcm->p_codec        = p_codec;
        p_soc_pcm->dev_registered = AW_TRUE;
    } else {
        p_aux_dev  = &p_card->p_aux_dev[num];
        p_name     = p_aux_dev->p_codec_name;

        if (p_aux_dev->pfn_init != NULL) {
            ret = p_aux_dev->pfn_init(&p_codec->dapm);
            if (ret != AW_OK) {
                AWSA_SOC_DEBUG_ERROR("failed to init %s: %d",
                                     p_name,
                                     ret);
                return ret;
            }
        }
    }

    /* Make sure all DAPM widgets are instantiated */
    awsa_soc_dapm_new(&p_codec->dapm);
    awsa_soc_dapm_sync(&p_codec->dapm);
    return ret;
}

/**
 * \brief dai link probe
 */
static aw_err_t __soc_dai_link_probe (struct awsa_soc_card *p_card, int num)
{
    const
    struct awsa_soc_dai_link  *p_dai_link  = &p_card->p_driver->p_dai_link[num];
    struct awsa_soc_pcm       *p_soc_pcm   = &p_card->p_soc_pcm[num];
    struct awsa_soc_codec     *p_codec     = p_soc_pcm->p_codec;
    struct awsa_soc_platform  *p_platform  = p_soc_pcm->p_plfm;
    struct awsa_soc_dai       *p_codec_dai = p_soc_pcm->p_codec_dai;
    struct awsa_soc_dai       *p_cpu_dai   = p_soc_pcm->p_cpu_dai;
    aw_err_t                   ret;

    AWSA_SOC_DEBUG_INFO("probe %s dai link %d", p_card->p_name, num);

    /* config components */
    p_codec_dai->plfm_codec.p_codec = p_codec;
    p_cpu_dai->plfm_codec.p_plfm    = p_platform;
    p_codec_dai->p_card             = p_card;
    p_cpu_dai->p_card               = p_card;

    /* set default power off timeout */
    p_soc_pcm->pmdown_time = __g_soc_pm_down_time;

    /* probe the cpu_dai */
    if (!p_cpu_dai->probed) {
        if (p_cpu_dai->p_driver->pfn_probe != NULL) {
            ret = p_cpu_dai->p_driver->pfn_probe(p_cpu_dai);
            if (ret != AW_OK) {
                AWSA_SOC_DEBUG_ERROR("failed to probe CPU DAI %s",
                                     p_cpu_dai->p_name);
                return ret;
            }
        }
        p_cpu_dai->probed = 1;

        /* mark cpu_dai as probed and add to card cpu_dai list */
        //aw_list_add(&p_cpu_dai->card_list, &p_card->dai_dev_list);
    }

    /* probe the CODEC */
    if (!p_codec->probed) {
        ret = __soc_codec_probe(p_card, p_codec);
        if (ret != AW_OK) {
            AWSA_SOC_DEBUG_ERROR("failed to probe codec %s",
                                 p_codec->p_name);
            return ret;
        }
    }

    /* probe the platform */
    if (!p_platform->probed) {
        if (p_platform->p_driver->pfn_probe != NULL) {
            ret = p_platform->p_driver->pfn_probe(p_platform);
            if (ret != AW_OK) {
                AWSA_SOC_DEBUG_ERROR(
                    "failed to probe platform %s", p_platform->p_name);
                return ret;
            }
        }

        /* mark platform as probed and add to card platform list */
        p_platform->probed = 1;
        //aw_list_add(&p_platform->card_list, &p_card->plfm_dev_list);
    }

    /* probe the CODEC DAI */
    if (!p_codec_dai->probed) {
        if (p_codec_dai->p_driver->pfn_probe) {
            ret = p_codec_dai->p_driver->pfn_probe(p_codec_dai);
            if (ret != AW_OK) {
                AWSA_SOC_DEBUG_ERROR(
                    "failed to probe CODEC DAI %s",
                    p_codec_dai->p_name);
                return ret;
            }
        }

        /* mark cpu_dai as probed and add to card cpu_dai list */
        p_codec_dai->probed = 1;
        //aw_list_add(&p_codec_dai->card_list, &p_card->dai_dev_list);
    }

    /* DAPM dai link stream work */
    aw_delayed_work_init(&p_soc_pcm->delayed_work,
                         __soc_delayed_work_close,
                        (void *)p_soc_pcm);

    ret = __soc_post_component_init(p_card, p_codec, num, 0);
    if (ret != AW_OK) {
        return ret;
    }

    /* create the pcm */
    ret = __soc_pcm_init(p_soc_pcm, num);
    if (ret != AW_OK) {
        AWSA_SOC_DEBUG_ERROR("can't init pcm %s", p_dai_link->p_stream_name);
        return ret;
    }

    return AW_OK;
}

/**
 * \brief auxiliary device probe
 */
static int __soc_aux_dev_probe (struct awsa_soc_card *p_card, int num)
{
    struct awsa_soc_aux_dev *p_aux_dev = &p_card->p_aux_dev[num];
    struct awsa_soc_codec   *p_codec;
    aw_err_t                 ret       = -AW_ENODEV;

    /* find CODEC from registered CODECs*/
    aw_list_for_each_entry(p_codec,
                           &__g_soc_codec_list,
                           struct awsa_soc_codec,
                           list) {
        if (!strcmp(p_codec->p_name, p_aux_dev->p_codec_name)) {
            if (p_codec->probed) {
                AWSA_SOC_DEBUG_ERROR("codec already probed");
                ret = -AW_EBUSY;
                goto __out;
            }
            goto __found;
        }
    }

    /* codec not found */
    AWSA_SOC_DEBUG_ERROR("codec %s not found", p_aux_dev->p_codec_name);
    goto __out;

__found:
    ret = __soc_codec_probe(p_card, p_codec);
    if (ret != AW_OK) {
        return ret;
    }

    ret = __soc_post_component_init(p_card, p_codec, num, 1);

__out:
    return ret;
}

/**
 * \brief auxiliary device remove
 */
static void __soc_aux_dev_remove (struct awsa_soc_card *p_card, int num)
{
    struct awsa_soc_aux_dev *p_aux_dev = &p_card->p_aux_dev[num];
    struct awsa_soc_codec   *p_codec   = NULL;

    /* find CODEC from registered CODECs*/
    aw_list_for_each_entry(p_codec,
                           &__g_soc_codec_list,
                           struct awsa_soc_codec,
                           list) {
        if (!strcmp(p_codec->p_name, p_aux_dev->p_codec_name)) {
            if (p_codec->probed) {
                AWSA_SOC_DEBUG_ERROR("codec already probed");
                return;
            }
            break;
        }
    }

    if (p_codec != NULL && p_codec->probed) {
        __soc_codec_remove(p_codec);
    }
}

/**
 * \brief initilize sound card
 */
static void __soc_instantiate_card (struct awsa_soc_card *p_card)
{
    aw_err_t               ret;
    int                    i;

    AW_MUTEX_LOCK(p_card->mutex, AW_SEM_WAIT_FOREVER);

    if (p_card->instantiated) {
        AW_MUTEX_UNLOCK(p_card->mutex);
        return;
    }

    /* bind DAIs */
    for (i = 0; i < p_card->p_driver->num_links; i++) {
        __soc_dai_link_bind(p_card, i);
    }

    /* bind completed ? */
    if (p_card->num_pcm != p_card->p_driver->num_links) {
        AW_MUTEX_UNLOCK(p_card->mutex);
        return;
    }

    /* card bind complete so register a sound card */
    ret = awsa_core_card_init(&p_card->core_card, p_card->id, p_card);
    if (ret != AW_OK) {
        AWSA_SOC_DEBUG_ERROR(
            "can't create sound card for card %s",
            p_card->p_name);
        AW_MUTEX_UNLOCK(p_card->mutex);
        return;
    }

    ret = awsa_core_ctl_init(&p_card->ctl, &p_card->core_card);
    if (ret != AW_OK) {
        AWSA_SOC_DEBUG_ERROR("unable to register ctl device");
        return;
    }

    /* initialise the sound card only once */
    if (p_card->p_driver->pfn_probe != NULL) {
        ret = p_card->p_driver->pfn_probe(p_card);
        if (ret != AW_OK) {
            goto __card_probe_error;
        }
    }

    for (i = 0; i < p_card->p_driver->num_links; i++) {
        ret = __soc_dai_link_probe(p_card, i);
        if (ret != AW_OK) {
            AWSA_SOC_DEBUG_ERROR(
                "failed to instantiate card %s: %d",
                p_card->p_name, ret);
            goto __probe_dai_err;
        }
    }

    for (i = 0; i < p_card->num_aux_dev; i++) {
        ret = __soc_aux_dev_probe(p_card, i);
        if (ret != AW_OK) {
            AWSA_SOC_DEBUG_ERROR(
                "failed to add auxiliary devices %s: %d",
                p_card->p_name, ret);
            goto __probe_aux_dev_err;
        }
    }

    ret = awsa_core_card_register(&p_card->core_card);
    if (ret != AW_OK) {
        AWSA_SOC_DEBUG_ERROR(
            "failed to register soundcard for %s",
            p_card->p_name);
        goto __probe_aux_dev_err;
    }

    p_card->instantiated = 1;
    AW_MUTEX_UNLOCK(p_card->mutex);
    return;

__probe_aux_dev_err:
    for (i = 0; i < p_card->num_aux_dev; i++) {
        __soc_aux_dev_remove(p_card, i);
    }

__probe_dai_err:
    for (i = 0; i < p_card->p_driver->num_links; i++) {
        __soc_dai_link_remove(p_card, i);
    }

__card_probe_error:
    if (p_card->p_driver->pfn_remove) {
        p_card->p_driver->pfn_remove(p_card);
    }

    AW_MUTEX_UNLOCK(p_card->mutex);
}

/**
 * \brief Attempt to initialise any uninitialised cards.
 * \attention Must be called with __g_soc_client_mutex.
 */
void __soc_instantiate_cards (void)
{
    struct awsa_soc_card *p_card;

    aw_list_for_each_entry(p_card,
                           &__g_soc_card_list,
                           struct awsa_soc_card,
                           list) {
        __soc_instantiate_card(p_card);
    }
}

/**
 * \brief create a new pcm
 */
static aw_err_t __soc_pcm_init (struct awsa_soc_pcm *p_soc_pcm, int num)
{
    struct awsa_soc_codec    *p_codec     = p_soc_pcm->p_codec;
    struct awsa_soc_dai      *p_codec_dai = p_soc_pcm->p_codec_dai;
    struct awsa_soc_dai      *p_cpu_dai   = p_soc_pcm->p_cpu_dai;
    struct awsa_core_pcm     *p_pcm       = &p_soc_pcm->core_pcm;

    aw_err_t                  ret         = AW_OK;
    int                       playback    = 0;
    int                       capture     = 0;

    if (p_codec_dai->p_driver->playback.channels_min) {
        playback = 1;
    }
    if (p_codec_dai->p_driver->capture.channels_min) {
        capture = 1;
    }

    AWSA_SOC_DEBUG_INFO("registered pcm #%d", num);
    ret = awsa_core_pcm_init(p_pcm,
                             num,
                             &p_soc_pcm->core_substream[0],
                             playback,
                             &p_soc_pcm->core_substream[1],
                             capture,
                             p_soc_pcm,
                             &p_soc_pcm->p_card->core_card);
    if (ret != AW_OK) {
        AWSA_SOC_DEBUG_ERROR("can't create pcm for codec %s",
                              p_codec->p_name);
        return ret;
    }

    //__g_soc_pcm_opts.pfn_pointer = p_platform->p_driver->p_opts->pfn_pointer;
    //__g_soc_pcm_opts.pfn_ioctl   = p_platform->p_driver->p_opts->pfn_ioctl;
    //__g_soc_pcm_opts.pfn_copy    = p_platform->p_driver->p_opts->pfn_copy;
    //__g_soc_pcm_opts.pfn_silence = p_platform->p_driver->p_opts->pfn_silence;
    //__g_soc_pcm_opts.pfn_ack     = p_platform->p_driver->p_opts->pfn_ack;

    if (playback != 0) {
        awsa_core_pcm_servopts_set(p_pcm,
                                   AWSA_PCM_STREAM_PLAYBACK,
                                   &__g_soc_pcm_opts);
    }

    if (capture != 0) {
        awsa_core_pcm_servopts_set(p_pcm,
                                   AWSA_PCM_STREAM_CAPTURE,
                                   &__g_soc_pcm_opts);
    }

    AWSA_SOC_DEBUG_ERROR("%s <-> %s mapping ok",
                         p_codec_dai->p_name,
                         p_cpu_dai->p_name);
    return ret;
}

/**
 * \brief update codec register bits
 */
static
aw_err_t __soc_codec_bits_update_locked (struct awsa_soc_codec *p_codec,
                                         uint16_t               reg,
                                         uint32_t               mask,
                                         uint32_t               value)
{
    aw_err_t err;

    AW_MUTEX_LOCK(p_codec->mutex, AW_SEM_WAIT_FOREVER);
    err = awsa_soc_codec_bits_update(p_codec, reg, mask, value);
    AW_MUTEX_UNLOCK(p_codec->mutex);

    return err;
}

/**
 * \brief platform register
 */
aw_err_t awsa_soc_platform_register (
    struct awsa_soc_platform         *p_plfm,
    const char                       *p_name,
    struct awsa_soc_platform_driver  *p_plfm_drv,
    struct awsa_soc_dai              *p_dais,
    const char                       *dais_name[],
    const struct awsa_soc_dai_driver *p_dais_drv,
    int                               num_dais,
    void                             *p_priave_data)
{
    aw_err_t ret;

    if (p_plfm == NULL || p_name == NULL || p_plfm_drv == NULL) {
        return -AW_EINVAL;
    }

    AWSA_SOC_DEBUG_INFO("platform register %s", p_name);

    p_plfm->p_name         = p_name;
    p_plfm->p_driver       = p_plfm_drv;
    p_plfm->p_dai          = p_dais;
    p_plfm->num_dai        = num_dais;
    p_plfm->p_private_data = p_priave_data;

    AW_MUTEX_LOCK(__g_soc_client_mutex, AW_SEM_WAIT_FOREVER);
    aw_list_add(&p_plfm->list, &__g_soc_plfm_list);
    __soc_instantiate_cards();
    AW_MUTEX_UNLOCK(__g_soc_client_mutex);

    /* register any DAIs */
    if (num_dais != 0) {
        ret = awsa_soc_dais_register(p_dais, dais_name, p_dais_drv, num_dais);
        if (ret != AW_OK) {
            return ret;
        }
    }

    AWSA_SOC_DEBUG_INFO("platform registered '%s'", p_plfm->p_name);
    return AW_OK;
}

/**
 * \brief platform unregister
 */
void awsa_soc_platform_unregister (struct awsa_soc_platform *p_plfm)
{
    struct awsa_soc_platform *p_tmp = NULL;

    if (p_plfm == NULL) {
        return;
    }

    aw_list_for_each_entry(p_tmp,
                           &__g_soc_plfm_list,
                           struct awsa_soc_platform,
                           list) {
        if (p_tmp == p_plfm) {
            goto __found;
        }
    }
    return;

__found:
    if (p_plfm->num_dai != 0) {
        awsa_soc_dais_unregister(p_plfm->p_dai, p_plfm->num_dai);
    }

    AW_MUTEX_LOCK(__g_soc_client_mutex, AW_SEM_WAIT_FOREVER);
    aw_list_del(&p_plfm->list);
    AW_MUTEX_UNLOCK(__g_soc_client_mutex);

    AWSA_SOC_DEBUG_INFO("unregistered platform '%s'", p_plfm->p_name);
}

/**
 * \brief codec register
 */
aw_err_t awsa_soc_codec_register (
    struct awsa_soc_codec              *p_codec,
    const char                         *p_name,
    const struct awsa_soc_codec_driver *p_codec_drv,
    struct awsa_soc_dai                *p_dais,
    const char                         *dais_name[],
    const struct awsa_soc_dai_driver   *p_dais_drv,
    int                                 num_dais,
    void                               *p_private_data)
{
    aw_err_t  ret;

    AWSA_SOC_DEBUG_ERROR("codec register %s", p_name);

    p_codec->p_name          = p_name;
    p_codec->dapm.bias_level = AWSA_SOC_BIAS_OFF;
    p_codec->dapm.p_codec    = p_codec;
    p_codec->p_driver        = p_codec_drv;
    p_codec->p_dai           = p_dais;
    p_codec->num_dai         = num_dais;
    p_codec->p_private_data  = p_private_data;
    AW_MUTEX_INIT(p_codec->mutex, AW_SEM_Q_PRIORITY);

    /* register any DAIs */
    if (num_dais != 0) {
        ret = awsa_soc_dais_register(p_dais, dais_name, p_dais_drv, num_dais);
        if (ret != AW_OK) {
            return ret;
        }
    }

    AW_MUTEX_LOCK(__g_soc_client_mutex, AW_SEM_WAIT_FOREVER);
    aw_list_add(&p_codec->list, &__g_soc_codec_list);
    __soc_instantiate_cards();
    AW_MUTEX_UNLOCK(__g_soc_client_mutex);

    AWSA_SOC_DEBUG_ERROR("registered codec '%s'", p_codec->p_name);
    return AW_OK;
}

/**
 * \brief codec unregister
 */
void awsa_soc_codec_unregister (struct awsa_soc_codec *p_codec)
{
    struct awsa_soc_codec *p_tmp;
    aw_list_for_each_entry(p_tmp,
                           &__g_soc_codec_list,
                           struct awsa_soc_codec,
                           list) {
        if (p_tmp == p_codec) {
            goto __found;
        }
    }
    return;

__found:
    if (p_codec->num_dai != 0) {
        awsa_soc_dais_unregister(p_codec->p_dai, p_codec->num_dai);
    }

    AW_MUTEX_LOCK(__g_soc_client_mutex, AW_SEM_WAIT_FOREVER);
    aw_list_del(&p_codec->list);
    AW_MUTEX_UNLOCK(__g_soc_client_mutex);

    AWSA_SOC_DEBUG_ERROR("unregistered codec '%s'", p_codec->p_name);
}

/**
 * \brief add ctl device
 */
aw_err_t awsa_soc_codec_ctl_elem_add (
    struct awsa_soc_codec                  *p_codec,
    struct awsa_core_ctl_elem              *p_elem,
    const struct awsa_core_ctl_elem_driver *p_driver,
    int                                     num_elem)
{
    struct awsa_soc_card *p_card = p_codec->p_card;
    aw_err_t              ret;
    int                   i;

    for (i = 0; i < num_elem; i++) {
        ret = awsa_core_ctl_elem_add(&p_elem[i],
                                     &p_driver[i],
                                     p_codec,
                                     &p_card->ctl);
        if (ret != AW_OK) {
            AWSA_SOC_DEBUG_ERROR("%s: Failed to add %s: %d",
                                 p_codec->p_name,
                                 p_driver[i].p_name,
                                 ret);
            return ret;
        }
    }

    return AW_OK;
}

aw_err_t awsa_soc_codec_widgets_add (
    struct awsa_soc_codec                    *p_codec,
    struct awsa_soc_dapm_widget              *p_widget,
    const struct awsa_soc_dapm_widget_driver *p_driver,
    int                                       num,
    struct awsa_core_ctl_elem                *p_elem)
{
    return awsa_soc_dapm_widgets_add(&p_codec->dapm,
                                     p_widget,
                                     p_driver,
                                     num,
                                     p_elem);
}

/**
 * \brief soc sound register
 */
aw_err_t awsa_soc_card_register (
    struct awsa_soc_card              *p_card,
    int                                id,
    const char                        *p_name,
    struct awsa_soc_pcm               *p_soc_pcm,
    int                                num_pcms,
    struct awsa_soc_aux_dev           *p_aux_devs,
    int                                num_aux_devs,
    const struct awsa_soc_card_driver *p_card_drv,
    void                              *p_private_data)
{
    if (p_card == NULL || p_name == NULL || p_card_drv == NULL) {
        return -AW_EINVAL;
    }

    if (num_pcms < p_card_drv->num_links) {
        AWSA_SOC_DEBUG_ERROR(
            "'%s' pcm number != dai link number", p_card->p_name);
        return -AW_EINVAL;
    }

    p_card->p_name         = p_name;
    p_card->p_driver       = p_card_drv;

    p_card->p_soc_pcm      = p_soc_pcm;
    p_card->num_pcm        = 0;

    p_card->p_aux_dev      = p_aux_devs;
    p_card->num_aux_dev    = num_aux_devs;

    p_card->id             = id;
    p_card->p_private_data = p_private_data;

    /* Bodge while we unpick instantiation */
#if 0
    AW_INIT_LIST_HEAD(&p_card->dai_dev_list);
    AW_INIT_LIST_HEAD(&p_card->codec_dev_list);
    AW_INIT_LIST_HEAD(&p_card->plfm_dev_list);
#endif
    AW_INIT_LIST_HEAD(&p_card->widgets);
    AW_INIT_LIST_HEAD(&p_card->paths);
    AW_INIT_LIST_HEAD(&p_card->dapm_list);
    AW_INIT_LIST_HEAD(&p_card->list);

    p_card->instantiated = 0;
    AW_MUTEX_INIT(p_card->mutex, AW_SEM_Q_PRIORITY);

    AW_MUTEX_LOCK(__g_soc_client_mutex, AW_SEM_WAIT_FOREVER);
    aw_list_add(&p_card->list, &__g_soc_card_list);
    __soc_instantiate_cards();
    AW_MUTEX_UNLOCK(__g_soc_client_mutex);

    AWSA_SOC_DEBUG_INFO("registered card '%s'", p_card->p_name);
    return AW_OK;
}

/**
 * \brief soc sound unregister
 */
aw_err_t awsa_soc_card_unregister (struct awsa_soc_card *p_card)
{
    if (p_card == NULL) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(__g_soc_client_mutex, AW_SEM_WAIT_FOREVER);
    aw_list_del(&p_card->list);
    AW_MUTEX_UNLOCK(__g_soc_client_mutex);

    AW_MUTEX_LOCK(p_card->mutex, AW_SEM_WAIT_FOREVER);
    p_card->instantiated = 0;
    AW_MUTEX_UNLOCK(p_card->mutex);
    return AW_OK;
}

/**
 * \brief codec io read
 */
aw_err_t awsa_soc_codec_read (struct awsa_soc_codec *p_codec,
                              uint32_t               reg,
                              uint32_t              *p_value)
{
    aw_err_t ret;

    ret = p_codec->p_driver->pfn_read(p_codec, reg, p_value);
    if (ret != AW_OK) {
        AWSA_SOC_DEBUG_ERROR("codec read failure");
        return ret;
    }
    AWSA_SOC_DEBUG_VERBOSE("codec read %x => %x", reg, *p_value);
    return ret;
}

/**
 * \brief codec io write
 */
aw_err_t awsa_soc_codec_write (struct awsa_soc_codec *p_codec,
                               uint32_t               reg,
                               uint32_t               val)
{
    aw_err_t ret;

    AWSA_SOC_DEBUG_VERBOSE("codec write %x => %x", reg, val);
    ret = p_codec->p_driver->pfn_write(p_codec, reg, val);
    if (ret != AW_OK) {
        AWSA_SOC_DEBUG_ERROR("codec write failure");
    }

    return ret;
}

/**
 * \brief codec register bit update
 */
aw_err_t awsa_soc_codec_bits_update (struct awsa_soc_codec *p_codec,
                                     uint16_t               reg,
                                     uint32_t               mask,
                                     uint32_t               value)
{
    aw_err_t ret;
    uint32_t old, new;

    ret = awsa_soc_codec_read(p_codec, reg, &old);
    if (ret != AW_OK) {
        return ret;
    }

    new = (old & ~mask) | value;
    if (old != new) {
        ret = awsa_soc_codec_write(p_codec, reg, new);
    }
    return ret;
}

/**
 * \brief send a stream event to the dapm
 */
aw_err_t awsa_soc_codec_stream_event (struct awsa_soc_codec *p_codec,
                                      const char            *p_stream,
                                      int                    event)
{
    if (p_codec == NULL || p_stream == NULL) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_codec->mutex, AW_SEM_WAIT_FOREVER);
    awsa_soc_dapm_stream_event(&p_codec->dapm, p_stream, event);
    AW_MUTEX_UNLOCK(p_codec->mutex);
    return AW_OK;
}

/**
 * \brief update the pcm status for the next period
 */
void awsa_soc_pcm_period_elapsed (struct awsa_soc_pcm *p_soc_pcm)
{
    awsa_core_pcm_period_elapsed(p_soc_pcm->p_substream);
}

/**
 * \brief enum double information get
 */
aw_err_t awsa_soc_enum_double_info (
    struct awsa_core_ctl_elem      *p_ctl_elem,
    struct awsa_core_ctl_elem_info *p_info)
{
    struct awsa_soc_enum *p_enum = (struct awsa_soc_enum *)
                                    p_ctl_elem->p_driver->private_value;

    p_info->type  = AWSA_CORE_CTL_ELEM_TYPE_ENUMERATED;
    p_info->count = p_enum->shift_l == p_enum->shift_r ? 1 : 2;

    p_info->info.enumerated.items = p_enum->max;

    if (p_info->info.enumerated.item > p_enum->max - 1) {
        p_info->info.enumerated.item = p_enum->max - 1;
    }

    return AW_OK;
}

/**
 * \brief external enumerated single mixer info callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_info     : elemeent inforation
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_enum_info_ext (
    struct awsa_core_ctl_elem      *p_ctl_elem,
    struct awsa_core_ctl_elem_info *p_info)
{
    struct awsa_soc_enum *p_e = (struct awsa_soc_enum *)
                                 p_ctl_elem->p_driver->private_value;

    p_info->type                  = AWSA_CORE_CTL_ELEM_TYPE_ENUMERATED;
    p_info->count                 = 1;
    p_info->info.enumerated.items = p_e->max;

    if (p_info->info.enumerated.item > p_e->max - 1) {
        p_info->info.enumerated.item = p_e->max - 1;
    }

    return AW_OK;
}

/**
 * \brief enumerated double mixer get callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_value    : ctl element value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_enum_double_get (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value)
{
    struct awsa_soc_codec *p_codec = p_ctl_elem->p_private_data;
    struct awsa_soc_enum  *p_e     = (struct awsa_soc_enum *)
                                     p_ctl_elem->p_driver->private_value;
    uint32_t val, bitmask;

    for (bitmask = 1; bitmask < p_e->max; bitmask <<= 1) {
        ;
    }

    awsa_soc_codec_read(p_codec, p_e->reg, &val);
    p_value->value.enumerated.p_item[0] = (val >> p_e->shift_l) & (bitmask - 1);
    if (p_e->shift_l != p_e->shift_r) {
        p_value->value.enumerated.p_item[1] = (val >> p_e->shift_r) &
                                              (bitmask - 1);
    }

    return AW_OK;
}

/**
 * \brief enumerated double mixer put callback
 */
aw_err_t awsa_soc_enum_double_set (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value)
{
    struct awsa_soc_codec *p_codec = p_ctl_elem->p_private_data;
    struct awsa_soc_enum  *p_e     = (struct awsa_soc_enum *)
                                     p_ctl_elem->p_driver->private_value;
    uint32_t val;
    uint32_t mask, bitmask;

    for (bitmask = 1; bitmask < p_e->max; bitmask <<= 1) {
        ;
    }

    if (p_value->value.enumerated.p_item[0] > p_e->max - 1) {
        return -AW_EINVAL;
    }

    val  = p_value->value.enumerated.p_item[0] << p_e->shift_l;
    mask = (bitmask - 1) << p_e->shift_l;
    if (p_e->shift_l != p_e->shift_r) {
        if (p_value->value.enumerated.p_item[1] > p_e->max - 1) {
            return -AW_EINVAL;
        }
        val  |= p_value->value.enumerated.p_item[1] << p_e->shift_r;
        mask |= (bitmask - 1) << p_e->shift_r;
    }

    return __soc_codec_bits_update_locked(p_codec, p_e->reg, mask, val);
}

/**
 * \brief enumerated double mixer get callback
 */
aw_err_t awsa_soc_enum_double_value_get (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value)
{
    struct awsa_soc_codec *p_codec = p_ctl_elem->p_private_data;
    struct awsa_soc_enum  *p_e     = (struct awsa_soc_enum *)
                                     p_ctl_elem->p_driver->private_value;
    uint32_t reg_val, val, mux;

    awsa_soc_codec_read(p_codec, p_e->reg, &reg_val);
    val = (reg_val >> p_e->shift_l) & p_e->mask;
    for (mux = 0; mux < p_e->max; mux++) {
        if (val == p_e->p_values[mux]) {
            break;
        }
    }

    p_value->value.enumerated.p_item[0] = mux;
    if (p_e->shift_l != p_e->shift_r) {
        val = (reg_val >> p_e->shift_r) & p_e->mask;
        for (mux = 0; mux < p_e->max; mux++) {
            if (val == p_e->p_values[mux]) {
                break;
            }
        }
        p_value->value.enumerated.p_item[1] = mux;
    }

    return AW_OK;
}

/**
 * \brief enumerated double mixer put callback
 */
aw_err_t awsa_soc_enum_double_value_set (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value)
{
    struct awsa_soc_codec *p_codec = p_ctl_elem->p_private_data;
    struct awsa_soc_enum  *p_e     = (struct awsa_soc_enum *)
                                     p_ctl_elem->p_driver->private_value;
    uint32_t val;
    uint32_t mask;

    if (p_value->value.enumerated.p_item[0] > p_e->max - 1) {
        return -AW_EINVAL;
    }
    val  = p_e->p_values[p_value->value.enumerated.p_item[0]] << p_e->shift_l;
    mask = p_e->mask << p_e->shift_l;
    if (p_e->shift_l != p_e->shift_r) {
        if (p_value->value.enumerated.p_item[1] > p_e->max - 1) {
            return -AW_EINVAL;
        }
        val  |= p_e->p_values[p_value->value.enumerated.p_item[1]] <<
                p_e->shift_r;
        mask |= p_e->mask << p_e->shift_r;
    }

    return __soc_codec_bits_update_locked(p_codec, p_e->reg, mask, val);
}

/**
 * \brief single mixer info callback
 */
aw_err_t awsa_soc_volsw_info (
    struct awsa_core_ctl_elem      *p_ctl_elem,
    struct awsa_core_ctl_elem_info *p_info)
{
    struct awsa_soc_mixer_ctl *p_mc =
        (struct awsa_soc_mixer_ctl *)p_ctl_elem->p_driver->private_value;

    int      platform_max;
    uint32_t shift  = p_mc->shift_l;
    uint32_t rshift = p_mc->shift_r;

    if (!p_mc->plfm_max) {
        p_mc->plfm_max = p_mc->max;
    }
    platform_max = p_mc->plfm_max;

    if (platform_max == 1 && !strstr((char *)p_ctl_elem->p_driver->p_name,
                                     " Volume")) {
        p_info->type = AWSA_CORE_CTL_ELEM_TYPE_BOOLEAN;
    } else {
        p_info->type = AWSA_CORE_CTL_ELEM_TYPE_INTEGER;
    }

    p_info->count            = shift == rshift ? 1 : 2;
    p_info->info.integer.min = 0;
    p_info->info.integer.max = platform_max;
    return AW_OK;
}

/**
 * \brief external single mixer info callback
 */
aw_err_t awsa_soc_volsw_info_ext (
    struct awsa_core_ctl_elem      *p_ctl_elem,
    struct awsa_core_ctl_elem_info *p_info)
{
    int max = p_ctl_elem->p_driver->private_value;

    if (max == 1 && !strstr(p_ctl_elem->p_driver->p_name, " Volume")) {
        p_info->type = AWSA_CORE_CTL_ELEM_TYPE_BOOLEAN;
    } else {
        p_info->type = AWSA_CORE_CTL_ELEM_TYPE_INTEGER;
    }

    p_info->count            = 1;
    p_info->info.integer.min = 0;
    p_info->info.integer.max = max;
    return AW_OK;
}

/**
 * \brief single mixer value get callback
 */
aw_err_t awsa_soc_volsw_get (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value)
{
    struct awsa_soc_mixer_ctl *p_mc    = (struct awsa_soc_mixer_ctl *)
                                         p_ctl_elem->p_driver->private_value;
    struct awsa_soc_codec     *p_codec = p_ctl_elem->p_private_data;

    uint32_t reg    = p_mc->reg_l;
    uint32_t val;
    uint32_t shift  = p_mc->shift_l;
    uint32_t rshift = p_mc->shift_r;
    int      max    = p_mc->max;
    uint32_t mask   = (1 << awsa_soc_fls(max)) - 1;
    uint32_t invert = p_mc->invert;

    awsa_soc_codec_read(p_codec, reg, &val);
    p_value->value.integer.p_value[0] = (val >> shift) & mask;
    if (shift != rshift) {
        awsa_soc_codec_read(p_codec, reg, &val);
        p_value->value.integer.p_value[1] = (val >> rshift) & mask;
    }

    if (invert) {
        p_value->value.integer.p_value[0] =
            max - p_value->value.integer.p_value[0];
        if (shift != rshift) {
            p_value->value.integer.p_value[1] =
                max - p_value->value.integer.p_value[1];
        }
    }

    return AW_OK;
}

/**
 * \brief single mixer value set callback
 */
aw_err_t awsa_soc_volsw_set (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value)
{
    struct awsa_soc_mixer_ctl *p_mc    = (struct awsa_soc_mixer_ctl *)
                                          p_ctl_elem->p_driver->private_value;
    struct awsa_soc_codec     *p_codec = p_ctl_elem->p_private_data;

    uint32_t reg    = p_mc->reg_l;
    uint32_t shift  = p_mc->shift_l;
    uint32_t rshift = p_mc->shift_r;
    int      max    = p_mc->max;
    uint32_t mask   = (1 << awsa_soc_fls(max)) - 1;
    uint32_t invert = p_mc->invert;

    uint32_t val, val2, val_mask;

    val = (p_value->value.integer.p_value[0] & mask);
    if (invert) {
        val = max - val;
    }
    val_mask = mask << shift;
    val = val << shift;
    if (shift != rshift) {
        val2 = (p_value->value.integer.p_value[1] & mask);
        if (invert) {
            val2 = max - val2;
        }
        val_mask |= mask << rshift;
        val      |= val2 << rshift;
    }

    return __soc_codec_bits_update_locked(p_codec, reg, val_mask, val);
}

/**
 * \brief double mixer info callback
 */
aw_err_t awsa_soc_volsw_info_2r (
    struct awsa_core_ctl_elem      *p_ctl_elem,
    struct awsa_core_ctl_elem_info *p_info)
{
    struct awsa_soc_mixer_ctl *p_mc = (struct awsa_soc_mixer_ctl *)
                                       p_ctl_elem->p_driver->private_value;
    int plfm_max;

    if (!p_mc->plfm_max) {
        p_mc->plfm_max = p_mc->max;
    }
    plfm_max = p_mc->plfm_max;

    if (plfm_max == 1 && !strstr(p_ctl_elem->p_driver->p_name, " Volume")) {
        p_info->type = AWSA_CORE_CTL_ELEM_TYPE_BOOLEAN;
    } else {
        p_info->type = AWSA_CORE_CTL_ELEM_TYPE_INTEGER;
    }

    p_info->count            = 2;
    p_info->info.integer.min = 0;
    p_info->info.integer.max = plfm_max;
    return AW_OK;
}

/**
 * \brief double mixer get callback
 */
aw_err_t awsa_soc_volsw_get_2r (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value)
{
    struct awsa_soc_mixer_ctl *p_mc    = (struct awsa_soc_mixer_ctl *)
                                          p_ctl_elem->p_driver->private_value;
    struct awsa_soc_codec     *p_codec = p_ctl_elem->p_private_data;

    uint32_t reg    = p_mc->reg_l;
    uint32_t reg2   = p_mc->reg_r;
    uint32_t shift  = p_mc->shift_l;
    int      max    = p_mc->max;
    uint32_t mask   = (1 << awsa_soc_fls(max)) - 1;
    uint32_t invert = p_mc->invert;
    uint32_t val, valr;

    awsa_soc_codec_read(p_codec, reg,  &val);
    awsa_soc_codec_read(p_codec, reg2, &valr);

    p_value->value.integer.p_value[0] = (val >> shift)  & mask;
    p_value->value.integer.p_value[1] = (valr >> shift) & mask;

    if (invert) {
        p_value->value.integer.p_value[0] =
            max - p_value->value.integer.p_value[0];
        p_value->value.integer.p_value[1] =
            max - p_value->value.integer.p_value[1];
    }

    return AW_OK;
}

/**
 * \brief double mixer set callback
 */
aw_err_t awsa_soc_volsw_set_2r (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value)
{
    struct awsa_soc_mixer_ctl *p_mc    = (struct awsa_soc_mixer_ctl *)
                                          p_ctl_elem->p_driver->private_value;
    struct awsa_soc_codec     *p_codec = p_ctl_elem->p_private_data;

    uint32_t reg    = p_mc->reg_l;
    uint32_t reg2   = p_mc->reg_r;
    uint32_t shift  = p_mc->shift_l;
    int      max    = p_mc->max;
    uint32_t mask   = (1 << awsa_soc_fls(max)) - 1;
    uint32_t invert = p_mc->invert;

    aw_err_t err;
    uint32_t val, val2, val_mask;

    val_mask = mask << shift;
    val      = (p_value->value.integer.p_value[0] & mask);
    val2     = (p_value->value.integer.p_value[1] & mask);

    if (invert) {
        val  = max - val;
        val2 = max - val2;
    }

    val  = val  << shift;
    val2 = val2 << shift;

    err = __soc_codec_bits_update_locked(p_codec, reg, val_mask, val);
    if (err != AW_OK) {
        return err;
    }

    return __soc_codec_bits_update_locked(p_codec, reg2, val_mask, val2);
}

/**
 * \brief signed mixer info callback
 */
aw_err_t awsa_soc_volsw_info_s8 (
    struct awsa_core_ctl_elem      *p_ctl_elem,
    struct awsa_core_ctl_elem_info *p_info)
{
    struct awsa_soc_mixer_ctl *p_mc = (struct awsa_soc_mixer_ctl *)
                                       p_ctl_elem->p_driver->private_value;

    int plfm_max;
    int min = p_mc->min;

    if (!p_mc->plfm_max) {
        p_mc->plfm_max = p_mc->max;
    }
    plfm_max = p_mc->plfm_max;

    p_info->type             = AWSA_CORE_CTL_ELEM_TYPE_INTEGER;
    p_info->count            = 2;
    p_info->info.integer.min = 0;
    p_info->info.integer.max = plfm_max - min;
    return AW_OK;
}

/**
 * \brief signed mixer get callback
 */
aw_err_t awsa_soc_volsw_get_s8 (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value)
{
    struct awsa_soc_mixer_ctl *p_mc    = (struct awsa_soc_mixer_ctl *)
                                          p_ctl_elem->p_driver->private_value;
    struct awsa_soc_codec     *p_codec = p_ctl_elem->p_private_data;

    uint32_t reg = p_mc->reg_l;
    int      min = p_mc->min;
    uint32_t val;

    awsa_soc_codec_read(p_codec, reg, &val);

    p_value->value.integer.p_value[0] = ((int8_t)(val & 0xff)) - min;
    p_value->value.integer.p_value[1] = ((int8_t)((val >> 8) & 0xff)) - min;
    return AW_OK;
}

/**
 * \brief signed mixer value set callback
 */
aw_err_t awsa_soc_volsw_set_s8 (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value)
{
    struct awsa_soc_mixer_ctl *p_mc    = (struct awsa_soc_mixer_ctl *)
                                          p_ctl_elem->p_driver->private_value;
    struct awsa_soc_codec     *p_codec = p_ctl_elem->p_private_data;

    uint32_t reg = p_mc->reg_l;
    int      min = p_mc->min;
    uint32_t val;

    val  = (p_value->value.integer.p_value[0]+min) & 0xff;
    val |= ((p_value->value.integer.p_value[1]+min) & 0xff) << 8;

    return __soc_codec_bits_update_locked(p_codec, reg, 0xffff, val);
}

/**
 * \brief double with tlv and variable data size mixer info callback
 */
aw_err_t awsa_soc_volsw_info_2r_sx (
    struct awsa_core_ctl_elem      *p_ctl_elem,
    struct awsa_core_ctl_elem_info *p_info)
{
    struct awsa_soc_mixer_ctl *p_mc = (struct awsa_soc_mixer_ctl *)
                                       p_ctl_elem->p_driver->private_value;

    int max = p_mc->max;
    int min = p_mc->min;

    p_info->type             = AWSA_CORE_CTL_ELEM_TYPE_INTEGER;
    p_info->count            = 2;
    p_info->info.integer.min = 0;
    p_info->info.integer.max = max - min;
    return AW_OK;
}

/**
 * \brief double with tlv and variable data size
 */
aw_err_t awsa_soc_volsw_get_2r_sx (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value)
{
    struct awsa_soc_mixer_ctl *p_mc    = (struct awsa_soc_mixer_ctl *)
                                          p_ctl_elem->p_driver->private_value;
    struct awsa_soc_codec     *p_codec = p_ctl_elem->p_private_data;

    uint32_t mask = (1 << p_mc->shift_l) - 1;
    int      min  = p_mc->min;
    uint32_t val;
    uint32_t valr;

    awsa_soc_codec_read(p_codec, p_mc->reg_l, &val);
    val  &= mask;
    awsa_soc_codec_read(p_codec, p_mc->reg_r, &valr);
    valr &= mask;

    p_value->value.integer.p_value[0] = ((val  & 0xff) - min) & mask;
    p_value->value.integer.p_value[1] = ((valr & 0xff) - min) & mask;
    return AW_OK;
}

/**
 * \brief double with tlv and variable data size mixer set callback
 */
aw_err_t awsa_soc_volsw_set_2r_sx (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value)
{
    struct awsa_soc_mixer_ctl *p_mc    = (struct awsa_soc_mixer_ctl *)
                                          p_ctl_elem->p_driver->private_value;
    struct awsa_soc_codec     *p_codec = p_ctl_elem->p_private_data;

    uint32_t mask = (1 << p_mc->shift_l) - 1;
    int      min  = p_mc->min;
    aw_err_t ret;
    uint32_t val, valr, oval, ovalr;

    val   = ((p_value->value.integer.p_value[0] + min) & 0xff);
    val  &= mask;
    valr  = ((p_value->value.integer.p_value[1] + min) & 0xff);
    valr &= mask;

    awsa_soc_codec_read(p_codec, p_mc->reg_l, &oval);
    oval  &= mask;
    awsa_soc_codec_read(p_codec, p_mc->reg_r, &ovalr);
    ovalr &= mask;

    ret = AW_OK;
    if (oval != val) {
        ret = awsa_soc_codec_write(p_codec, p_mc->reg_l, val);
        if (ret < 0) {
            return ret;
        }
    }

    if (ovalr != valr) {
        ret = awsa_soc_codec_write(p_codec, p_mc->reg_r, valr);
    }

    return ret;
}

/**
 * \brief awsa soc initialization
 */
void awsa_soc_init (void)
{
    AW_MUTEX_INIT(__g_soc_pcm_mutex,    AW_SEM_Q_PRIORITY);
    AW_MUTEX_INIT(__g_soc_client_mutex, AW_SEM_Q_PRIORITY);

    AW_INIT_LIST_HEAD(&__g_soc_card_list);
    AW_INIT_LIST_HEAD(&__g_soc_dai_list);
    AW_INIT_LIST_HEAD(&__g_soc_plfm_list);
    AW_INIT_LIST_HEAD(&__g_soc_codec_list);

    __g_soc_pm_down_time = 5000;
}

/* end of file */
