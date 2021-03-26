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

#ifndef __AWSA_CORE_PCM_H
#define __AWSA_CORE_PCM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "core/awsa_core_internal.h"

struct awsa_core_pcm_substream;

#if 0
struct awsa_core_pcm_hardware;
#endif

/**
 * \name pcm ioctl
 * @{
 */
#define AWSA_CORE_PCM_IOCTL_FALSE         ((void *)0)
#define AWSA_CORE_PCM_IOCTL_TRUE          ((void *)1)

#define AWSA_CORE_PCM_IOCTL_RESET          0
#define AWSA_CORE_PCM_IOCTL_INFO           1
#define AWSA_CORE_PCM_IOCTL_CHANNEL_INFO   2
#define AWSA_CORE_PCM_IOCTL_GSTATE         3
#define AWSA_CORE_PCM_IOCTL_FIFO_SIZE      4

/** @} */

/**
 * \name pcm trigger state
 * @{
 */
#define AWSA_CORE_PCM_TRIGGER_STOP          0
#define AWSA_CORE_PCM_TRIGGER_START         1
#define AWSA_CORE_PCM_TRIGGER_PAUSE_PUSH    3
#define AWSA_CORE_PCM_TRIGGER_PAUSE_RELEASE 4
#define AWSA_CORE_PCM_TRIGGER_SUSPEND       5
#define AWSA_CORE_PCM_TRIGGER_RESUME        6

/** @} */

/**
 * \brief pcm format bit set
 * \code
 * uint16_t formats;
 * foramts = AWSA_CORE_PCM_FMTBIT(AWSA_PCM_FORMAT_S8)    |
 *           AWSA_CORE_PCM_FMTBIT(AWSA_PCM_FORMAT_S16_LE);
 * \endcode
 */
#define AWSA_CORE_PCM_FMTBIT(format)      (1ull << (format))

/** \brief overrun */
#define AWSA_CORE_PCM_POS_XRUN            ((awsa_pcm_uframes_t)-1)

/**
 * \brief pcm stream or hardware state
 */
typedef enum {

    /** \brief stream is open */
    AWSA_CORE_PCM_STATE_OPEN          = 0,

    /** \brief stream has a setup */
    AWSA_CORE_PCM_STATE_SETUP,

    /** \brief stream is ready to start */
    AWSA_CORE_PCM_STATE_PREPARED,

    /** \brief stream is running */
    AWSA_CORE_PCM_STATE_RUNNING,

    /** \brief Overrun or Underrun */
    AWSA_CORE_PCM_STATE_XRUN,

    /** \brief stream is draining */
    AWSA_CORE_PCM_STATE_DRAINING,

    /** \brief stream is paused */
    AWSA_CORE_PCM_STATE_PAUSED,

    /** \brief stream is suspended */
    AWSA_CORE_PCM_STATE_SUSPENDED,

    /** \brief stream is disconnected */
    AWSA_CORE_PCM_STATE_DISCONNECTED,

    /** \brief the last */
    AWSA_CORE_PCM_STATE_LAST          = AWSA_CORE_PCM_STATE_DISCONNECTED
} awsa_core_pcm_state_t;

/**
 * \brief pcm device service function
 */
struct awsa_core_pcm_servopts {

    /** \brief open a pcm device */
    aw_err_t (*pfn_open     ) (struct awsa_core_pcm_substream *p_substream);

    /** \brief close a pcm device */
    aw_err_t (*pfn_close    ) (struct awsa_core_pcm_substream *p_substream);

    /** \brief pcm device io control */
    aw_err_t (*pfn_ioctl    ) (struct awsa_core_pcm_substream *p_substream,
                               int                             cmd,
                               void                           *p_arg);

    /** \brief pcm config */
    aw_err_t (*pfn_config) (struct awsa_core_pcm_substream *p_substream,
                            awsa_pcm_config_t              *p_params);

    /**
     * \brief dma memory alloc
     * \attention dma memory is a special kind of memory, dynamic allocation
     *            is more suitable than static definition
     */
    aw_err_t (*pfn_dma_alloc) (struct awsa_core_pcm_substream  *p_substream,
                               void                           **p_dma_mem,
                               awsa_pcm_uframes_t               period_size,
                               uint32_t                         period_count);

    /** \brief dma memory free */
    aw_err_t (*pfn_dma_free) (struct awsa_core_pcm_substream  *p_substream,
                              void                            *p_dma_mem);

    /** \brief pcm device prepare function */
    aw_err_t (*pfn_prepare  ) (struct awsa_core_pcm_substream *p_substream);

    /** \brief pcm device copy function */
    aw_err_t (*pfn_copy) (struct awsa_core_pcm_substream *p_substream,
                          int                             channel,
                          awsa_pcm_uframes_t              pos,
                          void                           *p_buf,
                          awsa_pcm_uframes_t              count);

    /** \brief pcm device trigger operation */
    aw_err_t (*pfn_trigger  ) (struct awsa_core_pcm_substream *p_substream,
                               int                             cmd);

    /** \brief get pcm trigger point */
    awsa_pcm_uframes_t
             (*pfn_pointer  ) (struct awsa_core_pcm_substream *p_substream);

    /** \brief pcm device stop transfer data */
    aw_err_t (*pfn_silence  ) (struct awsa_core_pcm_substream *p_substream,
                               int                             channel,
                               awsa_pcm_uframes_t              pos,
                               awsa_pcm_uframes_t              count);

    /** \brief ackledgement */
    aw_err_t (*pfn_ack      ) (struct awsa_core_pcm_substream *p_substream);
};

#if 0

/**
 *  \brief Hardware Parameters
 */
struct awsa_core_pcm_hardware {
    uint64_t           formats;            /**< \brief AWSA_CORE_PCM_FMTBIT_* */
    uint32_t           rates;              /**< \brief AWSA_CORE_PCM_RATE_* */
    uint32_t           rate_min;           /**< \brief min rate */
    uint32_t           rate_max;           /**< \brief max rate */
    uint32_t           channels_min;       /**< \brief min channels */
    uint32_t           channels_max;       /**< \brief max channels */
    size_t             buffer_bytes_min;   /**< \brief min buffer size */
    size_t             buffer_bytes_max;   /**< \brief max buffer size */
    awsa_pcm_uframes_t period_size_min;    /**< \brief min period size */
    awsa_pcm_uframes_t period_size_max;    /**< \brief max period size */
    uint32_t           period_count_min;   /**< \brief min # of periods */
    uint32_t           period_count_max;   /**< \brief max # of periods */
};

#endif

/**
 * \brief pcm format data
 */
struct awsa_pcm_format_data {
    uint8_t width;        /**< \brief bit width */
    uint8_t phys;         /**< \brief physical bit width */

    /** \brief 0 = big-endian, 1 = little-endian, -1 = others */
    int8_t  endian;

    int8_t  signd;        /**< \brief 0 = unsigned, 1 = signed, -1 = others */
    uint8_t silence[8];   /**< \brief silence data to fill */
};

/**
 * \brief substream runtime parameters
 */
struct awsa_core_pcm_runtime {

#if 0
    /** \brief hardware description */
    struct awsa_core_pcm_hardware   hw;
#endif

    /** \brief config description */
    awsa_pcm_config_t               config;

    /** \brief Timestamp */
    struct aw_timespec              tstamp;

    /** \brief available max */
    awsa_pcm_uframes_t              avail_max;

    /** \brief Position at buffer restart */
    awsa_pcm_uframes_t              hw_ptr_base;

    /** \brief hw ptr (0...boundary-1) */
    awsa_pcm_uframes_t              hw_ptr;

    /** \brief Position at interrupt time */
    awsa_pcm_uframes_t              hw_ptr_interrupt;

    /** \brief appl ptr (0...boundary-1) */
    awsa_pcm_uframes_t              appl_ptr;

    /** \brief pcm stream or hardware state */
    awsa_core_pcm_state_t           state;

    /** \brief suspended stream state */
    awsa_core_pcm_state_t           suspended_state;

    /** \brief Time when hw_ptr is updated */
    uint32_t                        hw_ptr_tick;

    /** \brief buffer time in systick */
    uint32_t                        hw_ptr_buffer_tick;

    /** \brief buffer size */
    awsa_pcm_uframes_t              buffer_size;

    /** \brief Min alignment for the format */
    awsa_pcm_uframes_t              min_align;

    /** \brief alignment */
    size_t                          byte_align;

    /** \brief frame bits */
    uint32_t                        frame_bits;

    /** \brief sample bits */
    uint32_t                        sample_bits;

    aw_bool_t                       no_period_wakeup;


    /** \brief pointers wrap point */
    awsa_pcm_uframes_t              boundary;

    /** \brief Silence filling size */
    awsa_pcm_uframes_t              silence_size;

    /** \brief starting pointer to silence area */
    awsa_pcm_uframes_t              silence_start;

    /** \brief size filled with silence */
    awsa_pcm_uframes_t              silence_filled;

    /** \brief extra delay; typically FIFO size */
    awsa_pcm_sframes_t   delay;

    /** \brief do transfer (!poll) wakeup if non-zero */
    awsa_pcm_uframes_t              twake;

    /** \brief DMA memory */
    uint8_t                        *p_dma_area;

#if 0

    /** \brief physical bus address (not accessible from main CPU) */
    uint32_t                        dma_addr;

    /** \brief size of DMA area */
    size_t                          dma_bytes;

#endif

    AW_SEMB_DECL(sleep);   /**< \brief poll */
    AW_SEMB_DECL(tsleep);  /**< \brief twake */
};

/**
 * \brief PCM audio data substream
 */
struct awsa_core_pcm_substream {

    /** \brief pcm device pointer */
    struct awsa_core_pcm            *p_pcm;

    /** \brief pcm stream pointer */
    struct awsa_core_pcm_stream     *p_stream;

    /** \brief private data */
    void                            *p_private_data;

    /** \brief substream index */
    int                              index;

    /** \brief stream (direction) */
    awsa_pcm_stream_t                stream;

    /** \brief stream use count */
    int                              ref_count;

    /** \brief limit ring buffer size */
    size_t                           buffer_bytes_max;

    /** \brief next substream */
    struct awsa_core_pcm_substream  *p_next;

    /** \brief spinlock */
    aw_spinlock_isr_t                lock;

    /** \brief hardware operations */
    struct awsa_core_pcm_servopts   *p_opts;

    /** \brief runtime information */
    struct awsa_core_pcm_runtime     rt;

};

/**
 * \brief PCM audio data stream
 */
struct awsa_core_pcm_stream {

    /** \brief pcm device */
    struct awsa_core_pcm           *p_pcm;

    /** \brief point to the first substream */
    struct awsa_core_pcm_substream *p_substream;

    /** \brief stream (direction) */
    awsa_pcm_stream_t               stream;

    /** \brief substream number */
    uint32_t                        cnt;

    /** \brief opened substream number */
    uint32_t                        opened_cnt;
};

/**
 * \brief core pcm device
 */
struct awsa_core_pcm {

    /** \brief pcm device */
    struct awsa_core_dev         dev;

    /** \brief sound card pointer */
    struct awsa_core_card       *p_card;

    /** \brief sound stream */
    struct awsa_core_pcm_stream  streams[2];

    int                          device;      /**< \brief pcm device index */
    struct aw_list_head          list;        /**< \brief list member */

    /** \brief private data */
    void                        *p_private_data;

    /** \brief mutex lock */
    AW_MUTEX_DECL(open_mutex);
};

/**
 * \brief pcm device init
 *
 * \param[in] p_card         : sound card pointer
 * \param[in] p_id           : sound card identifiers
 * \param[in] device         : pcm device index
 * \param[in] playback_count : playback substream number
 * \param[in] capture_count  : capture substream number
 * \param[in] pp_pcm         : return pcm device pointer
 *
 * \retval AW_OK      : success
 * \retval -AW_ENOMEM : no enough ram
 */
aw_err_t awsa_core_pcm_init (struct awsa_core_pcm           *p_pcm,
                             int                             device,
                             struct awsa_core_pcm_substream *p_playback,
                             int                             playback_count,
                             struct awsa_core_pcm_substream *p_capture,
                             int                             capture_count,
                             void                           *p_private_data,
                             struct awsa_core_card          *p_card);

/**
 * \brief pcm substream careate function
 *
 * \param[in] p_pcm           : pcm device pointer
 * \param[in] stream          : stream direction
 *                               - #AWSA_PCM_STREAM_PLAYBACK,
 *                               - #AWSA_PCM_STREAM_CAPTURE
 * \param[in] substream_count : substream number
 *
 * \retval AW_OK      : success
 * \retval -AW_ENOMEM : no enough ram
 */
aw_err_t awsa_core_pcm_stream_init (
    struct awsa_core_pcm           *p_pcm,
    awsa_pcm_stream_t               stream,
    struct awsa_core_pcm_substream *p_substream,
    int                             substream_count);

/**
 * \brief set the same service function for all substreams
 *
 * \param[in] p_pcm  : the pcm device pointer
 * \param[in] dir    : playback(#AWSA_PCM_STREAM_PLAYBACK) or
 *                     capture(#AWSA_PCM_STREAM_CAPTURE)
 * \param[in] p_opts : service function
 *
 * \return no
 */
void awsa_core_pcm_servopts_set (struct awsa_core_pcm          *p_pcm,
                                 int                            dir,
                                 struct awsa_core_pcm_servopts *p_opts);

/**
 * \brief update the pcm status for the next period
 * \note This function is called from the interrupt handler when the
 *       pcm has processed the period size.  It will update the current
 *       pointer, wake up mcu, etc.
 *
 *       Even if more than one periods have elapsed since the last call, you
 *       have to call this only once.
 *
 * \param[in] p_substream : the pcm substream instance
 * \return no
 */
void awsa_core_pcm_period_elapsed (struct awsa_core_pcm_substream *p_substream);

/**
 * \brief get pcm data format bits
 * \param[in] format : awsa_pcm_format_t type
 * \return the bits
 */
int awsa_core_pcm_format_width_get (awsa_pcm_format_t format);

/**
 * \brief get pcm data format physical bits
 * \param[in] format : awsa_pcm_format_t type
 * \return the bits
 */
int awsa_core_pcm_format_phys_get (awsa_pcm_format_t format);

/**
 * \brief pcm is running
 */
static inline aw_bool_t
awsa_core_pcm_is_running (struct awsa_core_pcm_substream *p_substream)
{
    awsa_core_pcm_state_t state = p_substream->rt.state;

    return (state               == AWSA_CORE_PCM_STATE_RUNNING  ||
           (state               == AWSA_CORE_PCM_STATE_DRAINING &&
            p_substream->stream == AWSA_PCM_STREAM_PLAYBACK));
}

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWSA_CORE_PCM_H */

/* end of file */
