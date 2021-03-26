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

/** \brief card list */
extern
struct aw_list_head __g_core_cards_list;

/* pcm device lsit */
struct aw_list_head __g_core_pcm_devices;

/* pcm register lock */
AW_MUTEX_DECL_STATIC(__g_core_pcm_register_mutex);

/* card list lock */
AW_MUTEX_IMPORT(__g_core_cards_lock);

/**
 * \brief pcm formats
 * \details [AWSA_PCM_FORMAT_*]={width, phys, endian, signd, silence}
 */
const
struct awsa_pcm_format_data __g_core_pcm_formats[AWSA_PCM_FORMAT_LAST+1] = {

    [AWSA_PCM_FORMAT_S8]     = {  8,  9, -1, 1, {0x00}                   },
    [AWSA_PCM_FORMAT_U8]     = {  8,  8, -1, 0, {0x80}                   },
    [AWSA_PCM_FORMAT_S16_LE] = { 16, 16,  1, 1, {0x00}                   },
    [AWSA_PCM_FORMAT_S16_BE] = { 16, 16,  0, 1, {0x00}                   },
    [AWSA_PCM_FORMAT_U16_LE] = { 16, 16,  1, 0, {0x00, 0x80}             },
    [AWSA_PCM_FORMAT_U16_BE] = { 16, 16,  0, 0, {0x80, 0x00}             },
    [AWSA_PCM_FORMAT_S24_LE] = { 24, 32,  1, 1, {0x00}                   },
    [AWSA_PCM_FORMAT_S24_BE] = { 24, 32,  0, 1, {0x00}                   },
    [AWSA_PCM_FORMAT_U24_LE] = { 24, 32,  1, 0, {0x00, 0x00, 0x80}       },
    [AWSA_PCM_FORMAT_U24_BE] = { 24, 32,  0, 0, {0x00, 0x80, 0x00, 0x00} },
    [AWSA_PCM_FORMAT_S32_LE] = { 32, 32,  1, 1, {0x00}                   },
    [AWSA_PCM_FORMAT_S32_BE] = { 32, 32,  0, 1, {0x00}                   },
    [AWSA_PCM_FORMAT_U32_LE] = { 32, 32,  1, 0, {0x00, 0x00, 0x00, 0x80} },
    [AWSA_PCM_FORMAT_U32_BE] = { 32, 32,  0, 0, {0x80, 0x00, 0x00, 0x00} },

    [AWSA_PCM_FORMAT_FLOAT_LE]   = { 32, 32, 1, -1, {0x00} },
    [AWSA_PCM_FORMAT_FLOAT_BE]   = { 32, 32, 0, -1, {0x00} },
    [AWSA_PCM_FORMAT_FLOAT64_LE] = { 64, 64, 1, -1, {0x00} },
    [AWSA_PCM_FORMAT_FLOAT64_BE] = { 64, 64, 0, -1, {0x00} },

    [AWSA_PCM_FORMAT_IEC958_SUBFRAME_LE] = { 32, 32, 1, -1, {0x00} },
    [AWSA_PCM_FORMAT_IEC958_SUBFRAME_BE] = { 32, 32, 0, -1, {0x00} },

    [AWSA_PCM_FORMAT_MU_LAW] = { 8, 8, -1, -1, {0x7f} },
    [AWSA_PCM_FORMAT_A_LAW]  = { 8, 8, -1, -1, {0x55} },

    [AWSA_PCM_FORMAT_IMA_ADPCM] = { 4, 4, -1, -1, {0x00} },
    [AWSA_PCM_FORMAT_G723_24]   = { 3, 3, -1, -1, {0x00} },
    [AWSA_PCM_FORMAT_G723_40]   = { 5, 5, -1, -1, {0x00} },

    /* FIXME: the following three formats are not defined properly yet */
    [AWSA_PCM_FORMAT_MPEG]    = { 0, 0, -1, -1, {0x00} },
    [AWSA_PCM_FORMAT_GSM]     = { 0, 0, -1, -1, {0x00} },
    [AWSA_PCM_FORMAT_SPECIAL] = { 0, 0, -1, -1, {0x00} },

    [AWSA_PCM_FORMAT_S24_3LE] = { 24, 24, 1, 1, {0x00}             },
    [AWSA_PCM_FORMAT_S24_3BE] = { 24, 24, 0, 1, {0x00}             },
    [AWSA_PCM_FORMAT_U24_3LE] = { 24, 24, 1, 0, {0x00, 0x00, 0x80} },
    [AWSA_PCM_FORMAT_U24_3BE] = { 24, 24, 0, 0, {0x80, 0x00, 0x00} },
    [AWSA_PCM_FORMAT_S20_3LE] = { 20, 24, 1, 1, {0x00}             },
    [AWSA_PCM_FORMAT_S20_3BE] = { 20, 24, 0, 1, {0x00}             },
    [AWSA_PCM_FORMAT_U20_3LE] = { 20, 24, 1, 0, {0x00, 0x00, 0x08} },
    [AWSA_PCM_FORMAT_U20_3BE] = { 20, 24, 0, 0, {0x08, 0x00, 0x00} },
    [AWSA_PCM_FORMAT_S18_3LE] = { 18, 24, 1, 1, {0x00}             },
    [AWSA_PCM_FORMAT_S18_3BE] = { 18, 24, 0, 1, {0x00}             },
    [AWSA_PCM_FORMAT_U18_3LE] = { 18, 24, 1, 0, {0x00, 0x00, 0x02} },
    [AWSA_PCM_FORMAT_U18_3BE] = { 18, 24, 0, 0, {0x02, 0x00, 0x00} },

    [AWSA_PCM_FORMAT_G723_24_1B] = { 3, 8, -1, -1, {0x00}},
    [AWSA_PCM_FORMAT_G723_40_1B] = { 5, 8, -1, -1, {0x00}},
};

/**
 * \brief transmit data
 */
struct __core_pcm_xmt {
    awsa_pcm_sframes_t  result;
    void               *p_buf;
    awsa_pcm_uframes_t  frames;
};

/**
 * \brief add pcm device
 */
static aw_err_t __core_pcm_add (struct awsa_core_pcm *p_new_pcm)
{
    struct awsa_core_pcm *p_tmp;

    aw_list_for_each_entry(p_tmp,
                           &__g_core_pcm_devices,
                           struct awsa_core_pcm,
                           list) {
        if (p_tmp->p_card == p_new_pcm->p_card &&
            p_tmp->device == p_new_pcm->device) {
            return -AW_EBUSY;
        }

        if (p_tmp->p_card->id > p_new_pcm->p_card->id ||
           (p_tmp->p_card == p_new_pcm->p_card &&
            p_tmp->device  > p_new_pcm->device)) {
            aw_list_add(&p_new_pcm->list, p_tmp->list.prev);
            return AW_OK;
        }
    }

    aw_list_add_tail(&p_new_pcm->list, &__g_core_pcm_devices);
    return AW_OK;
}

/**
 * \brief pcm device register
 */
static aw_err_t __core_pcm_dev_register (struct awsa_core_dev *p_dev)
{
    int                   cidx, err;
    struct awsa_core_pcm *p_pcm;

    if (p_dev == NULL || p_dev->p_dev_data == NULL) {
        return -AW_ENXIO;
    }

    p_pcm = p_dev->p_dev_data;

    AW_MUTEX_LOCK(__g_core_pcm_register_mutex, AW_SEM_WAIT_FOREVER);

    err = __core_pcm_add(p_pcm);
    if (err != AW_OK) {
        AW_MUTEX_UNLOCK(__g_core_pcm_register_mutex);
        return err;
    }

    for (cidx = 0; cidx < 2; cidx++) {
        if (p_pcm->streams[cidx].p_substream == NULL) {
            continue;
        }

        switch (cidx) {

        case AWSA_PCM_STREAM_PLAYBACK:
            AWSA_CORE_DEBUG_VERBOSE("pcmC%dD%dp",
                                     p_pcm->p_card->id,
                                     p_pcm->device);
            break;

        case AWSA_PCM_STREAM_CAPTURE:
            AWSA_CORE_DEBUG_VERBOSE("pcmC%dD%dc",
                                     p_pcm->p_card->id,
                                     p_pcm->device);
            break;

        default:
            break;
        }

    }

    AW_MUTEX_UNLOCK(__g_core_pcm_register_mutex);
    return AW_OK;
}

/**
 * \brief pcm device disconnect
 */
static aw_err_t __core_pcm_dev_disconnect (struct awsa_core_dev *p_dev)
{
    struct awsa_core_pcm           *p_pcm;
    struct awsa_core_pcm_substream *p_substream;
    int                             cidx;

    if (p_dev == NULL || p_dev->p_dev_data == NULL) {
        return -AW_ENXIO;
    }

    p_pcm = p_dev->p_dev_data;

    AW_MUTEX_LOCK(__g_core_pcm_register_mutex, AW_SEM_WAIT_FOREVER);

    if (aw_list_empty(&p_pcm->list)) {
        goto __unlock;
    }

    aw_list_del_init(&p_pcm->list);
    for (cidx = 0; cidx < 2; cidx++) {
        for (p_substream = p_pcm->streams[cidx].p_substream;
             p_substream;
             p_substream = p_substream->p_next) {
             p_substream->rt.state = AWSA_CORE_PCM_STATE_DISCONNECTED;
        }
    }

__unlock:
    AW_MUTEX_UNLOCK(__g_core_pcm_register_mutex);
    return AW_OK;
}

/**
 * \brief pcm devices service operation
 */
const static struct awsa_core_dev_servopts __g_core_pcm_opts = {
    __core_pcm_dev_register,
    NULL,
    __core_pcm_dev_disconnect,
};

/**
 * \brief attach substream
 */
static
aw_err_t __core_pcm_substream_attach (
    struct awsa_core_pcm            *p_pcm,
    int                              stream,
    struct awsa_core_pcm_substream **pp_substream)
{
    struct awsa_core_pcm_stream    *p_stream     = NULL;
    struct awsa_core_pcm_substream *p_substream  = NULL;

    awsa_core_assert(p_pcm        != NULL);
    awsa_core_assert(pp_substream != NULL);

    *pp_substream = NULL;
    p_stream = &p_pcm->streams[stream];
    if (p_stream->p_substream  == NULL || p_stream->cnt == 0) {
        return -AW_ENODEV;
    }

    /* only support half duplex */
    switch (stream) {

    case AWSA_PCM_STREAM_PLAYBACK:

        for (p_substream = p_pcm->streams[AWSA_PCM_STREAM_CAPTURE].p_substream;
             p_substream;
             p_substream = p_substream->p_next) {
            if (p_substream->ref_count > 0) {
                return -AW_EAGAIN;
            }
        }
        break;

    case AWSA_PCM_STREAM_CAPTURE:

        for (p_substream = p_pcm->streams[AWSA_PCM_STREAM_PLAYBACK].p_substream;
             p_substream;
             p_substream = p_substream->p_next) {
            if (p_substream->ref_count > 0) {
                return -AW_EAGAIN;
            }
        }
        break;

    default:
        return -AW_EINVAL;
    }

    for (p_substream = p_stream->p_substream;
         p_substream;
         p_substream = p_substream->p_next) {
        if (!(p_substream->ref_count > 0)) {
            break;
        }
    }

    if (p_substream == NULL) {
        return -AW_EAGAIN;
    }

    AW_SEMB_INIT(p_substream->rt.sleep,  AW_SEM_EMPTY, AW_SEM_Q_FIFO);
    AW_SEMB_INIT(p_substream->rt.tsleep, AW_SEM_EMPTY, AW_SEM_Q_FIFO);

    p_substream->rt.state         = AWSA_CORE_PCM_STATE_OPEN;
    p_substream->p_private_data   = p_pcm->p_private_data;
    p_substream->ref_count        = 1;
    p_stream->opened_cnt++;
    *pp_substream = p_substream;
    return AW_OK;
}

aw_err_t awsa_core_pcm_init (struct awsa_core_pcm           *p_pcm,
                             int                             device,
                             struct awsa_core_pcm_substream *p_playback,
                             int                             playback_count,
                             struct awsa_core_pcm_substream *p_capture,
                             int                             capture_count,
                             void                           *p_private_data,
                             struct awsa_core_card          *p_card)
{
    aw_err_t ret;

    awsa_core_assert(p_pcm  != NULL);
    awsa_core_assert(p_card != NULL);

    p_pcm->p_card = p_card;
    p_pcm->device = device;

    ret = awsa_core_pcm_stream_init(p_pcm,
                                   AWSA_PCM_STREAM_PLAYBACK,
                                   p_playback,
                                   playback_count);
    if (ret != AW_OK) {
        return ret;
    }

    ret = awsa_core_pcm_stream_init(p_pcm,
                                    AWSA_PCM_STREAM_CAPTURE,
                                    p_capture,
                                    capture_count);
    if (ret != AW_OK) {
        return ret;
    }

    AW_MUTEX_INIT(p_pcm->open_mutex, AW_SEM_Q_PRIORITY);

    ret = awsa_core_dev_init(&p_pcm->dev,
                             AWSA_CORE_DEV_TYPE_PCM,
                             p_pcm,
                             &__g_core_pcm_opts,
                             p_card);
    if (ret != AW_OK) {
        return ret;
    }

    p_pcm->p_private_data = p_private_data;
    return AW_OK;
}

aw_err_t awsa_core_pcm_stream_init (
    struct awsa_core_pcm           *p_pcm,
    awsa_pcm_stream_t               stream,
    struct awsa_core_pcm_substream *p_substream,
    int                             substream_count)
{
    struct awsa_core_pcm_stream    *p_stream = &p_pcm->streams[stream];
    struct awsa_core_pcm_substream *p_tmp, *p_prev;
    int                             idx;

    p_stream->stream  = stream;
    p_stream->p_pcm   = p_pcm;
    p_stream->cnt     = substream_count;

    p_prev = NULL;
    p_tmp  = NULL;
    for (idx = 0, p_prev = NULL; idx < substream_count; idx++) {
        p_tmp = &p_substream[idx];
        if (p_tmp == NULL) {
            continue;
        }
        p_tmp->p_pcm            = p_pcm;
        p_tmp->p_stream         = p_stream;
        p_tmp->index            = idx;
        p_tmp->stream           = stream;
        p_tmp->buffer_bytes_max = UINT_MAX;
        if (p_prev == NULL) {
            p_stream->p_substream = p_tmp;
        } else {
            p_prev->p_next        = p_tmp;
        }

        aw_spinlock_isr_init(&p_tmp->lock, 0);
        p_prev = p_tmp;
    }

    return AW_OK;
}

/**
 * \brief set the same service function for all substreams
 */
void awsa_core_pcm_servopts_set (struct awsa_core_pcm          *p_pcm,
                                 int                            dir,
                                 struct awsa_core_pcm_servopts *p_opts)
{
    struct awsa_core_pcm_stream    *p_stream = &p_pcm->streams[dir];
    struct awsa_core_pcm_substream *p_substream;

    for (p_substream  = p_stream->p_substream;
         p_substream != NULL;
         p_substream  = p_substream->p_next) {
        p_substream->p_opts = p_opts;
    }
}

/**
 * \brief update the pcm status for the next period
 */
void awsa_core_pcm_period_elapsed (struct awsa_core_pcm_substream *p_substream)
{
    if (p_substream == NULL) {
        return;
    }

    aw_spinlock_isr_take(&p_substream->lock);
    if (!awsa_core_pcm_is_running(p_substream)             ||
        awsa_core_pcm_lib_update(p_substream, 1) != AW_OK) {
        goto __end;
    }

    /* timer device interrupt process */
    // need add something

__end:
    aw_spinlock_isr_give(&p_substream->lock);
}

/**
 * \brief get pcm data format bits
 */
int awsa_core_pcm_format_width_get (awsa_pcm_format_t format)
{
    int val;

    if (format < 0 || format > AWSA_PCM_FORMAT_LAST) {
        return -AW_EINVAL;
    }

    if ((val = __g_core_pcm_formats[format].width) == 0) {
        return -AW_EINVAL;
    }

    return val;
}

/**
 * \brief get pcm data format physical bits
 */
int awsa_core_pcm_format_phys_get (awsa_pcm_format_t format)
{
    int val;

    if (format < 0 || format > AWSA_PCM_FORMAT_LAST) {
        return -AW_EINVAL;
    }

    if ((val = __g_core_pcm_formats[format].phys) == 0) {
        return -AW_EINVAL;
    }

    return val;
}

/**
 * \brief write frames
 */
static
aw_err_t __core_pcm_frames_write (struct awsa_core_pcm_substream *p_substream,
                                  struct __core_pcm_xmt          *p_xmt)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;
    awsa_pcm_sframes_t            result;

    if (p_runtime->state == AWSA_CORE_PCM_STATE_OPEN) {
        return -AW_EBADF;
    }

    result = awsa_core_pcm_lib_write(p_substream,
                                     p_xmt->p_buf,
                                     p_xmt->frames);
    p_xmt->result = result;
    return result < 0 ? result : 0;
}

/**
 * \brief read frames
 */
static
aw_err_t __core_pcm_frames_read (struct awsa_core_pcm_substream *p_substream,
                                 struct __core_pcm_xmt          *p_xmt)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;
    awsa_pcm_sframes_t            result;

    if (p_runtime->state == AWSA_CORE_PCM_STATE_OPEN) {
        return -AW_EBADF;
    }

    result = awsa_core_pcm_lib_read(p_substream,
                                    p_xmt->p_buf,
                                    p_xmt->frames);
    p_xmt->result = result;
    return result < 0 ? result : 0;
}

/**
 * \brief open one pcm stream
 */
aw_err_t awsa_pcm_open (awsa_pcm_t        *p_pcm,
                        int                card,
                        int                device,
                        awsa_pcm_stream_t  stream,
                        awsa_pcm_config_t *p_config)
{
    struct awsa_core_dev           *p_dev       = NULL;
    struct awsa_core_pcm           *p_sub_pcm   = NULL;
    struct awsa_core_pcm_substream *p_substream = NULL;
    struct awsa_core_card          *p_card      = NULL;

    aw_err_t ret;

    if (p_pcm == NULL) {
        return -AW_EINVAL;
    }

    memset(p_pcm, 0, sizeof(*p_pcm));

    AW_MUTEX_LOCK(__g_core_cards_lock, AW_SEM_WAIT_FOREVER);
    aw_list_for_each_entry(p_card,
                           &__g_core_cards_list,
                           struct awsa_core_card,
                           list) {

        if (p_card->id == card) {
            AW_MUTEX_UNLOCK(__g_core_cards_lock);
            goto __card_find;
        }
    }

    AW_MUTEX_UNLOCK(__g_core_cards_lock);
    return -AW_ENODEV;

__card_find:

    aw_list_for_each_entry(p_dev,
                           &p_card->devices,
                           struct awsa_core_dev,
                           list) {
        p_sub_pcm = (struct awsa_core_pcm *)p_dev->p_dev_data;
        if (p_sub_pcm->device == device                  &&
            p_dev->type       == AWSA_CORE_DEV_TYPE_PCM) {
            goto __device_find;
        }
    }

    return -AW_ENODEV;

__device_find:

    ret = __core_pcm_substream_attach(p_sub_pcm, stream, &p_substream);
    if (ret != AW_OK) {
        return ret;
    }

    awsa_core_assert(p_substream->p_opts->pfn_open != NULL);
    ret = p_substream->p_opts->pfn_open(p_substream);
    if (ret != AW_OK) {
        awsa_pcm_close(p_pcm);
        return ret;
    }

    p_pcm->p_cookie = p_substream;

    if (p_config != NULL) {
        ret = awsa_pcm_config_set(p_pcm, p_config);
    }

    return ret;
}

#if 0

/**
 * \brief open one avaliable pcm stream
 */
aw_err_t awsa_pcm_open_avail (awsa_pcm_t        *p_pcm,
                              int               *p_card,
                              int               *p_device,
                              awsa_pcm_stream_t  stream,
                              awsa_pcm_config_t *p_config)
{
    return -AW_ENOTSUP;
}

#endif

/**
 * \brief close one pcm stream
 */
aw_err_t awsa_pcm_close (struct awsa_pcm *p_pcm)
{
    aw_err_t ret;

    if (p_pcm == NULL || p_pcm->p_cookie == NULL) {
        return -AW_EINVAL;
    }

    /* drain buffer data */
    awsa_core_pcm_lib_drain(p_pcm->p_cookie);

    ret = awsa_core_pcm_lib_release(p_pcm->p_cookie);
    if (ret != AW_OK) {
        return ret;
    }

    return AW_OK;
}

/**
 * \brief hardware parameter set
 */
static
aw_err_t __core_pcm_config_set (struct awsa_core_pcm_substream *p_substream,
                                awsa_pcm_config_t              *p_config)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;
    aw_err_t                      err;
    uint32_t                      bits;
    awsa_pcm_uframes_t            frames;

    if (p_substream == NULL || p_config == NULL) {
        return -AW_ENXIO;
    }

    aw_spinlock_isr_take(&p_substream->lock);
    switch (p_runtime->state) {

    case AWSA_CORE_PCM_STATE_OPEN:
    case AWSA_CORE_PCM_STATE_SETUP:
    case AWSA_CORE_PCM_STATE_PREPARED:
        break;

    default:
        aw_spinlock_isr_give(&p_substream->lock);
        return -AW_EBADF;
    }
    aw_spinlock_isr_give(&p_substream->lock);

    if (p_substream->p_opts->pfn_config != NULL) {
        err = p_substream->p_opts->pfn_config(p_substream, p_config);
        if (err != AW_OK) {
            goto __error;
        }
    }

    if (p_substream->p_opts->pfn_dma_alloc != NULL) {
        if (p_runtime->p_dma_area             != NULL &&
            p_substream->p_opts->pfn_dma_free != NULL) {
            p_substream->p_opts->pfn_dma_free(p_substream,
                                              (void *)p_runtime->p_dma_area);
        }

        err = p_substream->p_opts->pfn_dma_alloc(p_substream,
                                                 (void *)&p_runtime->p_dma_area,
                                                 p_config->period_size,
                                                 p_config->period_count);
        if (err != AW_OK) {
            goto __error;
        }
    }

    bits = awsa_core_pcm_format_phys_get(p_config->format);

    p_runtime->sample_bits  = bits;
    bits                   *= p_config->channels;
    p_runtime->frame_bits   = bits;
    frames                  = 1;

    while (bits % 8 != 0) {
        bits *= 2;
        frames *= 2;
    }
    p_runtime->byte_align = bits / 8;
    p_runtime->min_align  = frames;

    /* Default sw params */
    p_runtime->silence_size         = 0;
    p_runtime->buffer_size          = p_config->period_size *
                                      p_config->period_count;
    p_runtime->boundary             = p_runtime->buffer_size;
    while (p_runtime->boundary * 2 <= LONG_MAX - p_runtime->buffer_size) {
        p_runtime->boundary *= 2;
    }

    p_runtime->config = *p_config;
    p_runtime->state  = AWSA_CORE_PCM_STATE_SETUP;
    return AW_OK;

__error:

    /*
     * hardware might be unuseable from this time,
     * so we force application to retry to set
     * the correct hardware parameter settings
     */
    p_runtime->state = AWSA_CORE_PCM_STATE_OPEN;
    if (p_substream->p_opts->pfn_dma_free != NULL) {
        p_substream->p_opts->pfn_dma_free(p_substream,
                                          (void *)p_runtime->p_dma_area);
    }

    return err;
}

/**
 * \brief pcm stream config setup
 */
aw_err_t awsa_pcm_config_set (awsa_pcm_t        *p_pcm,
                              awsa_pcm_config_t *p_config)
{
    if (p_pcm == NULL || p_config == NULL) {
        return -AW_EINVAL;
    }

    p_pcm->config = *p_config;

    if (p_config->start_threshold == 0) {
        p_pcm->config.start_threshold = p_config->period_count *
                                        p_config->period_size / 2;
    }

    if (p_config->stop_threshold == 0) {
        p_pcm->config.stop_threshold = p_config->period_count *
                                       p_config->period_size * 10;
    }

    p_pcm->config.avail_min = p_config->period_size;
    return __core_pcm_config_set(p_pcm->p_cookie, &p_pcm->config);
}

/**
 * \brief pcm stream config get
 */
aw_err_t awsa_pcm_config_get (awsa_pcm_t        *p_pcm,
                              awsa_pcm_config_t *p_config)
{
    if (p_pcm == NULL || p_config == NULL) {
        return -AW_EINVAL;
    }

    *p_config = p_pcm->config;
    return AW_OK;
}

/**
 * \brief Returns the buffer size (frames) used for pcm read/write.
 */
awsa_pcm_uframes_t awsa_pcm_buffer_size_get (awsa_pcm_t *p_pcm)
{
    return (p_pcm->config.period_size * p_pcm->config.period_count);
}

/**
 * \brief the number of bytes represented by the frames
 */
size_t awsa_pcm_frames_to_bytes (awsa_pcm_t         *p_pcm,
                                 awsa_pcm_uframes_t  frames)
{
    return (frames * (p_pcm->config.channels *
            awsa_core_pcm_format_phys_get(p_pcm->config.format) / 8));
}

/**
 * \brief the number of frame represented by the bytes
 */
awsa_pcm_uframes_t awsa_pcm_bytes_to_frames (awsa_pcm_t *p_pcm,
                                             size_t      bytes)
{
    return (bytes / (p_pcm->config.channels *
            awsa_core_pcm_format_phys_get(p_pcm->config.format) / 8));
}

/**
 * \brief get timestamp
 */
static aw_err_t __core_pcm_htimestamp_get (
    struct awsa_core_pcm_substream *p_substream,
    awsa_pcm_uframes_t             *p_avail,
    aw_timespec_t                  *p_tstamp)
{
    struct awsa_core_pcm_runtime *p_runtime = &p_substream->rt;
    int                           frames;
    awsa_pcm_uframes_t            hw_ptr;

    if (p_tstamp != NULL) {
        *p_tstamp = p_runtime->tstamp;
        if (p_tstamp->tv_sec == 0 && p_tstamp->tv_nsec == 0) {
            return AW_ERROR;
        }
    }

    hw_ptr = p_runtime->hw_ptr;
    if (p_substream->stream == AWSA_PCM_STREAM_PLAYBACK) {
        frames = hw_ptr + p_runtime->buffer_size - p_runtime->appl_ptr;
    } else {
        frames = hw_ptr - p_runtime->appl_ptr;
    }

    if (frames < 0) {
        frames += p_runtime->boundary;
    } else if (frames > (int)p_runtime->boundary) {
        frames -= p_runtime->boundary;
    }

    if (p_avail != NULL) {
        *p_avail = (uint32_t)frames;
    }

    return AW_OK;
}

/**
 * \brief Returns available frames in pcm buffer and corresponding time stamp.
 *        For an input stream, frames available are frames ready for the
 *        application to read.
 *        For an output stream, frames available are the number of empty
 *        frames available for the application to write.
 */
aw_err_t awsa_pcm_htimestamp_get (awsa_pcm_t         *p_pcm,
                                  awsa_pcm_uframes_t *p_avail,
                                  aw_timespec_t      *p_tstamp)
{
    if (p_pcm == NULL) {
        return -AW_EINVAL;
    }

    return __core_pcm_htimestamp_get(p_pcm->p_cookie, p_avail, p_tstamp);;
}

/**
 * \brief Write data to the fifo
 */
ssize_t awsa_pcm_write (awsa_pcm_t *p_pcm,
                        const void *p_buf,
                        size_t      size)
{
    struct __core_pcm_xmt x;
    aw_err_t              ret;

    x.p_buf  = (void *)p_buf;
    x.frames = size / (p_pcm->config.channels *
            awsa_core_pcm_format_phys_get(p_pcm->config.format) / 8);

    ret = __core_pcm_frames_write(p_pcm->p_cookie, &x);
    if (ret != AW_OK) {
        AWSA_CORE_DEBUG_ERROR("pcm cannot write stream data");
        return ret;
    }

    return size;
}

/**
 * \brief Read data from the fifo
 */
ssize_t awsa_pcm_read (awsa_pcm_t *p_pcm,
                       void       *p_buf,
                       size_t      size)
{
    struct __core_pcm_xmt x;
    aw_err_t              ret;

    x.p_buf  = p_buf;
    x.frames = size / (p_pcm->config.channels *
            awsa_core_pcm_format_phys_get(p_pcm->config.format) / 8);

    ret = __core_pcm_frames_read(p_pcm->p_cookie, &x);
    if (ret != AW_OK) {
        AWSA_CORE_DEBUG_ERROR("pcm cannot read stream data");
        return ret;
    }

    return size;
}

/**
 * \brief Prepare before start stream
 */
aw_err_t awsa_pcm_prepare (awsa_pcm_t *p_pcm)
{
    aw_err_t ret;

    ret = awsa_core_pcm_lib_prepare(p_pcm->p_cookie);
    if (ret != AW_OK) {
        AWSA_CORE_DEBUG_ERROR("cannot prepare channel");
    }

    return ret;
}

/**
 * \brief Start a PCM channel that transfer data
 */
aw_err_t awsa_pcm_start (awsa_pcm_t *p_pcm)
{
    aw_err_t ret;

    ret = awsa_core_pcm_lib_start(p_pcm->p_cookie);
    if (ret != AW_OK) {
        AWSA_CORE_DEBUG_ERROR("cannot start channel");
        return ret;
    }

    return ret;
}

/**
 * \brief stop a PCM channel that doesn't transfer data
 */
aw_err_t awsa_pcm_stop (awsa_pcm_t *p_pcm)
{
    aw_err_t ret;

    if (p_pcm == NULL) {
        return -AW_EINVAL;
    }

    ret = awsa_core_pcm_lib_drop(p_pcm->p_cookie);
    if (ret != AW_OK) {
        AWSA_CORE_DEBUG_ERROR("cannot stop channel");
    }

    return ret;
}

/**
 * \brief Stop a PCM preserving pending frames
 */
aw_err_t awsa_pcm_drain (awsa_pcm_t *p_pcm)
{
    aw_err_t ret;

    if (p_pcm == NULL) {
        return -AW_EINVAL;
    }

    ret = awsa_core_pcm_lib_drain(p_pcm->p_cookie);
    if (ret != AW_OK) {
        AWSA_CORE_DEBUG_ERROR("cannot drain channel");
    }

    return ret;
}

/**
 * \brief Resume a PCM stream
 */
aw_err_t awsa_pcm_pause (awsa_pcm_t *p_pcm, aw_bool_t enable)
{
    aw_err_t ret;

    if (p_pcm == NULL) {
        return -AW_EINVAL;
    }

    if (enable) {
        ret = awsa_core_pcm_lib_pause(p_pcm->p_cookie, 1);
        if (ret != AW_OK) {
            AWSA_CORE_DEBUG_ERROR("can't enable pause channel");
        }
    } else {
        ret = awsa_core_pcm_lib_pause(p_pcm->p_cookie, 0);
        if (ret != AW_OK) {
            AWSA_CORE_DEBUG_ERROR("can't disable pause channel");
        }
    }

    return ret;
}

#if 0

/**
 * \brief get pcm stream buffer fill situation
 */
aw_err_t awsa_pcm_delay (awsa_pcm_t         *p_pcm,
                         awsa_pcm_sframes_t *p_delay)
{
    aw_err_t ret;

    if (p_pcm == NULL) {
        return -AW_EINVAL;
    }

    ret = awsa_core_pcm_lib_delay(p_pcm->p_cookie, p_delay);
    if (ret != AW_OK) {
        AWSA_CORE_DEBUG_ERROR("cannot delay channel");
    }

    return ret;
}

#endif

/**
 * \brief Returns available frames in pcm buffer
 */
awsa_pcm_sframes_t awsa_pcm_avail_get (awsa_pcm_t *p_pcm)
{
    aw_err_t ret;

    if (p_pcm == NULL) {
        return -AW_EINVAL;
    }

    ret = awsa_core_pcm_lib_avail_update(p_pcm->p_cookie);
    if (ret != AW_OK) {
        AWSA_CORE_DEBUG_ERROR("cannot get avail frames");
    }

    return ret;
}

/* end of file */
