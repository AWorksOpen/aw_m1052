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
 * - 1.00 16-11-09  win, first implementation
 * \endinternal
 */

#ifndef __AWSA_PCM_H
#define __AWSA_PCM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_types.h"
#include "aw_errno.h"
#include "aw_time.h"

/**
 * \name pcm rate
 * @{
 */

#define AWSA_PCM_RATE_5512         (1 << 0)    /**< \brief 5512Hz */
#define AWSA_PCM_RATE_8000         (1 << 1)    /**< \brief 8000Hz */
#define AWSA_PCM_RATE_11025        (1 << 2)    /**< \brief 11025Hz */
#define AWSA_PCM_RATE_16000        (1 << 3)    /**< \brief 16000Hz */
#define AWSA_PCM_RATE_22050        (1 << 4)    /**< \brief 22050Hz */
#define AWSA_PCM_RATE_32000        (1 << 5)    /**< \brief 32000Hz */
#define AWSA_PCM_RATE_44100        (1 << 6)    /**< \brief 44100Hz */
#define AWSA_PCM_RATE_48000        (1 << 7)    /**< \brief 48000Hz */
#define AWSA_PCM_RATE_64000        (1 << 8 )   /**< \brief 64000Hz */
#define AWSA_PCM_RATE_88200        (1 << 9 )   /**< \brief 88200Hz */
#define AWSA_PCM_RATE_96000        (1 << 10)   /**< \brief 96000Hz */
#define AWSA_PCM_RATE_176400       (1 << 11)   /**< \brief 176400Hz */
#define AWSA_PCM_RATE_192000       (1 << 12)   /**< \brief 192000Hz */

/** \brief continuous range */
#define AWSA_PCM_RATE_CONTINUOUS   (1 << 30)

/** \brief supports more non-continuos rates */
#define AWSA_PCM_RATE_KNOT         (1 << 31)

/** @} */

/** \brief unsigned frames count */
typedef uint32_t  awsa_pcm_uframes_t;

/** \brief signed frames count */
typedef int32_t   awsa_pcm_sframes_t;

/**
 * \brief pcm stream (direction)
 */
typedef enum awsa_pcm_stream {

    /** \brief Playback stream */
    AWSA_PCM_STREAM_PLAYBACK = 0,

    /** \brief Capture stream */
    AWSA_PCM_STREAM_CAPTURE,

    /** \brief end define */
    AWSA_PCM_STREAM_LAST = AWSA_PCM_STREAM_CAPTURE
} awsa_pcm_stream_t;

/**
 * \brief pcm data format
 */
typedef enum awsa_pcm_format {

    /** \brief Unknown */
    AWSA_PCM_FORMAT_UNKNOWN = -1,

    /** \brief Signed 8 bit */
    AWSA_PCM_FORMAT_S8      = 0,

    /** \brief Unsigned 8 bit */
    AWSA_PCM_FORMAT_U8,

    /** \brief Signed 16 bit Little Endian */
    AWSA_PCM_FORMAT_S16_LE,

    /** \brief Signed 16 bit Big Endian */
    AWSA_PCM_FORMAT_S16_BE,

    /** \brief Unsigned 16 bit Little Endian */
    AWSA_PCM_FORMAT_U16_LE,

    /** \brief Unsigned 16 bit Big Endian */
    AWSA_PCM_FORMAT_U16_BE,

    /**
     * \brief Signed 24 bit Little Endian using low three bytes in 32-bit word
     */
    AWSA_PCM_FORMAT_S24_LE,

    /**
     * \brief Signed 24 bit Big Endian using low three bytes in 32-bit word
     */
    AWSA_PCM_FORMAT_S24_BE,

    /**
     * \brief Unsigned 24 bit Little Endian using low three bytes
     *        in 32-bit word
     */
    AWSA_PCM_FORMAT_U24_LE,

    /**
     * \brief Unsigned 24 bit Big Endian using low three bytes in 32-bit word
     */
    AWSA_PCM_FORMAT_U24_BE,

    /** \brief Signed 32 bit Little Endian */
    AWSA_PCM_FORMAT_S32_LE,

    /** \brief Signed 32 bit Big Endian */
    AWSA_PCM_FORMAT_S32_BE,

    /** \brief Unsigned 32 bit Little Endian */
    AWSA_PCM_FORMAT_U32_LE,

    /** \brief Unsigned 32 bit Big Endian */
    AWSA_PCM_FORMAT_U32_BE,

    /** \brief Float 32 bit Little Endian, Range -1.0 to 1.0 */
    AWSA_PCM_FORMAT_FLOAT_LE,

    /** \brief Float 32 bit Big Endian, Range -1.0 to 1.0 */
    AWSA_PCM_FORMAT_FLOAT_BE,

    /** \brief Float 64 bit Little Endian, Range -1.0 to 1.0 */
    AWSA_PCM_FORMAT_FLOAT64_LE,

    /** \brief Float 64 bit Big Endian, Range -1.0 to 1.0 */
    AWSA_PCM_FORMAT_FLOAT64_BE,

    /** \brief IEC-958 Little Endian */
    AWSA_PCM_FORMAT_IEC958_SUBFRAME_LE,

    /** \brief IEC-958 Big Endian */
    AWSA_PCM_FORMAT_IEC958_SUBFRAME_BE,

    /** \brief Mu-Law */
    AWSA_PCM_FORMAT_MU_LAW,

    /** \brief A-Law */
    AWSA_PCM_FORMAT_A_LAW,

    /** \brief Ima-ADPCM */
    AWSA_PCM_FORMAT_IMA_ADPCM,

    /** \brief MPEG */
    AWSA_PCM_FORMAT_MPEG,

    /** \brief GSM */
    AWSA_PCM_FORMAT_GSM,

    /** \brief Special */
    AWSA_PCM_FORMAT_SPECIAL = 31,

    /** \brief Signed 24bit Little Endian in 3bytes format */
    AWSA_PCM_FORMAT_S24_3LE = 32,

    /** \brief Signed 24bit Big Endian in 3bytes format */
    AWSA_PCM_FORMAT_S24_3BE,

    /** \brief Unsigned 24bit Little Endian in 3bytes format */
    AWSA_PCM_FORMAT_U24_3LE,

    /** \brief Unsigned 24bit Big Endian in 3bytes format */
    AWSA_PCM_FORMAT_U24_3BE,

    /** \brief Signed 20bit Little Endian in 3bytes format */
    AWSA_PCM_FORMAT_S20_3LE,

    /** \brief Signed 20bit Big Endian in 3bytes format */
    AWSA_PCM_FORMAT_S20_3BE,

    /** \brief Unsigned 20bit Little Endian in 3bytes format */
    AWSA_PCM_FORMAT_U20_3LE,

    /** \brief Unsigned 20bit Big Endian in 3bytes format */
    AWSA_PCM_FORMAT_U20_3BE,

    /** \brief Signed 18bit Little Endian in 3bytes format */
    AWSA_PCM_FORMAT_S18_3LE,

    /** \brief Signed 18bit Big Endian in 3bytes format */
    AWSA_PCM_FORMAT_S18_3BE,

    /** \brief Unsigned 18bit Little Endian in 3bytes format */
    AWSA_PCM_FORMAT_U18_3LE,

    /** \brief Unsigned 18bit Big Endian in 3bytes format */
    AWSA_PCM_FORMAT_U18_3BE,

    /** \brief G.723 (ADPCM) 24 kbit/s, 8 samples in 3 bytes */
    AWSA_PCM_FORMAT_G723_24,

    /** \brief G.723 (ADPCM) 24 kbit/s, 1 sample in 1 byte */
    AWSA_PCM_FORMAT_G723_24_1B,

    /** \brief G.723 (ADPCM) 40 kbit/s, 8 samples in 3 bytes */
    AWSA_PCM_FORMAT_G723_40,

    /* G.723 (ADPCM) 40 kbit/s, 1 sample in 1 byte */
    AWSA_PCM_FORMAT_G723_40_1B,

    /** \brief Direct Stream Digital (DSD) in 1-byte samples (x8) */
    AWSA_PCM_FORMAT_DSD_U8,

    /** \brief Direct Stream Digital (DSD) in 2-byte samples (x16) */
    AWSA_PCM_FORMAT_DSD_U16_LE,

    /** \brief Direct Stream Digital (DSD) in 4-byte samples (x32) */
    AWSA_PCM_FORMAT_DSD_U32_LE,

    /** \brief Direct Stream Digital (DSD) in 2-byte samples (x16) */
    AWSA_PCM_FORMAT_DSD_U16_BE,

    /** \brief Direct Stream Digital (DSD) in 4-byte samples (x32) */
    AWSA_PCM_FORMAT_DSD_U32_BE,

    /** \brief end define */
    AWSA_PCM_FORMAT_LAST = AWSA_PCM_FORMAT_DSD_U32_BE,
} awsa_pcm_format_t;

/**
 * \brief hardware parameter
 */
typedef struct awsa_pcm_config {

    uint32_t             channels;  /**< \brief channel number */
    uint32_t             rate;      /**< \brief AWSA_PCM_RATE_* */

    /** \brief one transfer max frame number, frame_size = format * channels */
    awsa_pcm_uframes_t   period_size;

    /** \brief buffer_size = period_size * period_count */
    uint32_t             period_count;

    /** \brief audio data format */
    awsa_pcm_format_t    format;

    /**
     * \brief values to use for start threshold
     *        default start_threshold : period_count * period_size
     */
    awsa_pcm_uframes_t   start_threshold;

    /**
     * \brief values to use for stop threshold
     *        default stop_threshold : period_count * period_size
     */
    awsa_pcm_uframes_t   stop_threshold;

    /**
     * \brief values to use for silence threshold
     *        default silence_threshold : 0
     */
    awsa_pcm_uframes_t   silence_threshold;

    /** \brief min avail frames for wakeup */
    awsa_pcm_uframes_t   avail_min;

} awsa_pcm_config_t;

/**
 * \brief pcm device
 */
typedef struct awsa_pcm {

    int                     mode;       /**< \brief mode flag */

#if 0
    bool_t                  running;    /**< \brief device running flag */
    int                     overruns;   /**< \brief buffer full count */
    int                     underruns;  /**< \brief buffer empty count */
#endif

    int                     card;       /**< \brief card number */
    int                     device;     /**< \brief device number */
    awsa_pcm_stream_t       stream;     /**< \brief playback or capture */

    /** \brief config information */
    struct awsa_pcm_config  config;

    /** \brief temporary value */
    void                   *p_cookie;
} awsa_pcm_t;

/**
 * \brief open one pcm stream
 *
 * \param[in] p_pcm  : pcm inforamtion pointer
 * \param[in] card   : sound card index
 * \param[in] device : pcm device index
 * \param[in] stream : pcm stream (playback or capture)
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_pcm_open (awsa_pcm_t        *p_pcm,
                        int                card,
                        int                device,
                        awsa_pcm_stream_t  stream,
                        awsa_pcm_config_t *p_config);

/**
 * \brief close one pcm stream
 * \param[in] p_pcm : pcm inforamtion pointer
 * \retval AW_OK : success
 */
aw_err_t awsa_pcm_close (awsa_pcm_t *p_pcm);

/**
 * \brief pcm stream config setup
 *
 * \param[in] p_pcm    : pcm inforamtion pointer
 * \param[in] p_params : need setup hardware parameter
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_pcm_config_set (awsa_pcm_t        *p_pcm,
                              awsa_pcm_config_t *p_config);

/**
 * \brief pcm stream config get
 *
 * \param[in] p_pcm    : pcm inforamtion pointer
 * \param[in] p_params : need setup hardware parameter
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_pcm_config_get (awsa_pcm_t        *p_pcm,
                              awsa_pcm_config_t *p_config);

/**
 * \brief Returns the buffer size (frames) used for pcm read/write.
 * \param[in] p_pcm : pcm inforamtion pointer
 * \retval buffer size
 */
awsa_pcm_uframes_t awsa_pcm_buffer_size_get (awsa_pcm_t *p_pcm);

/**
 * \brief the number of bytes represented by the frames
 * \param[in] p_pcm  : pcm inforamtion pointer
 * \parma[in] frames : frame count
 * \return byte count
 */
size_t awsa_pcm_frames_to_bytes (awsa_pcm_t         *p_pcm,
                                 awsa_pcm_uframes_t  frames);

/**
 * \brief the number of frame represented by the bytes
 * \param[in] p_pcm  : pcm inforamtion pointer
 * \parma[in] frames : frame count
 * \return frame count
 */
awsa_pcm_uframes_t awsa_pcm_bytes_to_frames (awsa_pcm_t *p_pcm,
                                             size_t      nbytes);

/**
 * \brief Returns available frames in pcm buffer and corresponding time stamp.
 *        For an input stream, frames available are frames ready for the
 *        application to read.
 *        For an output stream, frames available are the number of empty frames
 *        available for the application to write.
 *
 * \param[in]  p_pcm    : pcm inforamtion pointer
 * \param[out] p_avail  : return available frames
 * \param[out] p_tstamp : return corresponding time stamp
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_pcm_htimestamp_get (awsa_pcm_t         *p_pcm,
                                  awsa_pcm_uframes_t *p_avail,
                                  aw_timespec_t      *p_tstamp);

/**
 * \brief Returns available frames in pcm buffer
 *
 * \param[in]  p_pcm : pcm inforamtion pointer
 * \return available frames
 */
awsa_pcm_sframes_t awsa_pcm_avail_get (awsa_pcm_t *p_pcm);

/**
 * \brief Prepare before start stream
 * \param[in]  p_pcm : pcm inforamtion pointer
 * \retval AW_OK : success
 */
aw_err_t awsa_pcm_prepare (awsa_pcm_t *p_pcm);

/**
 * \brief Write data to the fifo
 *
 * \param[in] p_pcm : pcm inforamtion pointer
 * \param[in] p_buf : data buffer
 * \param[in] size  : data buffer size
 *
 * \return how many data(bytes) had wrriten to the fifo
 */
ssize_t awsa_pcm_write (awsa_pcm_t *p_pcm, const void *p_buf, size_t size);

/**
 * \brief Write data to the fifo
 *
 * \param[in] p_pcm : pcm inforamtion pointer
 * \param[in] p_buf : data buffer
 * \param[in] size  : data buffer size
 *
 * \return how many data(bytes) had wrriten to the fifo
 */
aw_static_inline
ssize_t awsa_pcm_write1 (awsa_pcm_t *p_pcm, const void *p_buf, size_t size)
{
    ssize_t ret;

    do {
        ret = awsa_pcm_write(p_pcm, p_buf, size);
        if (ret == -AW_EPIPE) {
            awsa_pcm_prepare(p_pcm);
            continue;
        }
        break;
    } while (1);

    return ret;
}

/**
 * \brief Read data from the fifo
 *
 * \param[in]  p_pcm : pcm inforamtion pointer
 * \param[out] p_buf : data buffer
 * \param[in]  size  : data buffer size
 *
 * \return how many data(bytes) had read from the fifo
 */
ssize_t awsa_pcm_read (awsa_pcm_t *p_pcm, void *p_buf, size_t  size);

/**
 * \brief Read data from the fifo
 *
 * \param[in]  p_pcm : pcm inforamtion pointer
 * \param[out] p_buf : data buffer
 * \param[in]  size  : data buffer size
 *
 * \return how many data(bytes) had read from the fifo
 */
aw_static_inline
ssize_t awsa_pcm_read1 (awsa_pcm_t *p_pcm, void *p_buf, size_t  size)
{
    ssize_t ret;

    do {
        ret = awsa_pcm_read(p_pcm, p_buf, size);
        if (ret == -AW_EPIPE) {
            awsa_pcm_prepare(p_pcm);
            continue;
        }
        break;
    } while (1);

    return ret;
}

/**
 * \brief Start a PCM channel that doesn't transfer data
 * \param[in] p_pcm : pcm inforamtion pointer
 * \retval AW_OK : success
 */
aw_err_t awsa_pcm_start (awsa_pcm_t *p_pcm);

/**
 * \brief stop a PCM channel that doesn't transfer data
 * \param[in] p_pcm : pcm inforamtion pointer
 * \retval AW_OK : success
 */
aw_err_t awsa_pcm_stop (awsa_pcm_t *p_pcm);

/**
 * \brief Stop a PCM preserving pending frames
 * \note For playback wait for all pending frames to be played and then stop
 *       the PCM.
 *       For capture stop PCM permitting to retrieve residual frames.
 *
 * \param[in]  p_pcm : pcm inforamtion pointer
 * \retval AW_OK : success
 */
aw_err_t awsa_pcm_drain (awsa_pcm_t *p_pcm);

/**
 * \brief Pause a PCM stream
 * \note Note that this function works only on the hardware which supports
 *       pause feature.
 *
 * \param[in]  p_pcm : pcm inforamtion pointer
 * \retval AW_OK : success
 */
aw_err_t awsa_pcm_pause (awsa_pcm_t *p_pcm, aw_bool_t enable);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWSA_PCM_H */

/* end of file */
