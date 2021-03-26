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
 * - 1.00 20-10-13  win, first implementation.
 * \endinternal
 */

#include "aw_mp3_internal.h"

/******************************************************************************/

/** \brief bitrate */
const static uint32_t __g_frame_bit_rate_table[5][15] = {

    /* MPEG-1 */

    {   /* Layer I   */
             0,  32000,  64000,  /* 0000, 0001, 0010, */
         96000, 128000, 160000,  /* 0011, 0100, 0101, */
        192000, 224000, 256000,  /* 0110, 0111, 1000, */
        288000, 320000, 352000,  /* 1001, 1010, 1011, */
        384000, 416000, 448000,  /* 1100, 1101, 1110  */
    },
    {   /* Layer II  */
             0,  32000,  48000,  /* 0000, 0001, 0010, */
         56000,  64000,  80000,  /* 0011, 0100, 0101, */
         96000, 112000, 128000,  /* 0110, 0111, 1000, */
        160000, 192000, 224000,  /* 1001, 1010, 1011, */
        256000, 320000, 384000,  /* 1100, 1101, 1110  */
    },
    {   /* Layer III */
             0,  32000,  40000,  /* 0000, 0001, 0010, */
         48000,  56000,  64000,  /* 0011, 0100, 0101, */
         80000,  96000, 112000,  /* 0110, 0111, 1000, */
        128000, 160000, 192000,  /* 1001, 1010, 1011, */
        224000, 256000, 320000,  /* 1100, 1101, 1110  */
    },

    /* MPEG-2 LSF */
    {   /* Layer I   */
             0,  32000,  48000,  /* 0000, 0001, 0010, */
         56000,  64000,  80000,  /* 0011, 0100, 0101, */
         96000, 112000, 128000,  /* 0110, 0111, 1000, */
        144000, 160000, 176000,  /* 1001, 1010, 1011, */
        192000, 224000, 256000,  /* 1100, 1101, 1110  */
    },
    {   /* Layers II & III */
             0,   8000,  16000,  /* 0000, 0001, 0010, */
         24000,  32000,  40000,  /* 0011, 0100, 0101, */
         48000,  56000,  64000,  /* 0110, 0111, 1000, */
         80000,  96000, 112000,  /* 1001, 1010, 1011, */
        128000, 144000, 160000,  /* 1100, 1101, 1110  */
    },
};

/** \brief sample rate */
const int __g_frame_sample_rate_table[] = { 44100, 48000, 32000 };

static int __frame_decode_header (struct aw_mp3_dec_frame_header *p_header,
                                  struct aw_mp3_dec_stream       *p_stream);

/**
 * \brief 解码器函数类型
 */
typedef aw_err_t (*pfn_frame_decode_t) (struct aw_mp3_dec_stream *p_stream,
                                        struct aw_mp3_dec_frame  *p_frame);

/**
 * \brief 解码器
 */
const static pfn_frame_decode_t __g_frame_decoder[] = {
    aw_mp3_dec_layer_i_decode,
    aw_mp3_dec_layer_ii_decode,
    aw_mp3_dec_layer_iii_decode,
};

/**
 * \brief attempt to discover the bitstream's free bitrate
 */
static
int __frame_free_bitrate (struct aw_mp3_dec_stream             *p_stream,
                          const struct aw_mp3_dec_frame_header *p_header)
{
    struct aw_mp3_dec_bit  keep;
    uint32_t               rate = 0;
    uint32_t               pad_slot, slots_per_frame;

    uint32_t               pos_ptr = 0;

    keep            = p_stream->ptr;
    pad_slot        = (p_header->flags & AW_MP3_DEC_FRAME_FLAG_PADDING) ? 1 : 0;
    slots_per_frame = (p_header->layer == AW_MP3_DEC_FRAME_LAYER_III &&
                      (p_header->flags & AW_MP3_DEC_FRAME_FLAG_LSF_EXT)) ?
                       72 : 144;

    while (aw_mp3_dec_stream_sync(p_stream) == AW_OK) {
        struct aw_mp3_dec_stream       peek_stream;
        struct aw_mp3_dec_frame_header peek_header;

        peek_stream = *p_stream;
        peek_header = *p_header;

        if (__frame_decode_header(&peek_header, &peek_stream) == AW_OK &&
            peek_header.layer       == p_header->layer &&
            peek_header.sample_rate == p_header->sample_rate) {

            uint32_t n;

            pos_ptr = aw_mp3_dec_bit_next_byte(&p_stream->ptr);
            n       = pos_ptr - p_stream->pos_this_frame;

            if (p_header->layer == AW_MP3_DEC_FRAME_LAYER_I) {
                rate = (uint32_t)p_header->sample_rate *
                       (n - 4 * pad_slot + 4) / 48 / 1000;
            } else {
                rate = (uint32_t)p_header->sample_rate *
                       (n - pad_slot + 1) / slots_per_frame / 1000;
            }

            if (rate >= 8) {
                break;
            }
        }

        aw_mp3_dec_bit_skip(&p_stream->ptr, 8);
    }

    p_stream->ptr = keep;

    if (rate < 8 || (p_header->layer == AW_MP3_DEC_FRAME_LAYER_III &&
                     rate > 640)) {
        p_stream->error = AW_MP3_DEC_ERR_LOSTSYNC;
        return AW_ERROR;
    }

    p_stream->free_rate = rate * 1000;
    return AW_OK;
}

/**
 * \brief read header data and following CRC word
 */
static int __frame_decode_header (struct aw_mp3_dec_frame_header *p_header,
                                  struct aw_mp3_dec_stream       *p_stream)
{
    uint32_t index;

    p_header->flags        = 0;
    p_header->private_bits = 0;

    /* syncword */
    aw_mp3_dec_bit_skip(&p_stream->ptr, 11);

    /* MPEG 2.5 indicator (really part of syncword) */
    if (aw_mp3_dec_bit_read(&p_stream->ptr, 1) == 0) {
        p_header->flags |= AW_MP3_DEC_FRAME_FLAG_MPEG_2_5_EXT;
    }

    /* ID */
    if (aw_mp3_dec_bit_read(&p_stream->ptr, 1) == 0) {
        p_header->flags |= AW_MP3_DEC_FRAME_FLAG_LSF_EXT;
    } else if (p_header->flags & AW_MP3_DEC_FRAME_FLAG_MPEG_2_5_EXT) {
        p_stream->error = AW_MP3_DEC_ERR_LOSTSYNC;
        return AW_ERROR;
    }

    /* ID */
#if 0
    p_header->id = aw_mp3_dec_bit_read(&p_stream->ptr, 2);
    if (p_header->id == AW_MP3_DEC_FRAME_ID_RESERVE) {
        p_stream->error = AW_MP3_DEC_ERR_LOSTSYNC;
        return AW_ERROR;
    }
#endif

    /* layer */
    p_header->layer = (enum aw_mp3_dec_frame_layer)
                      (4 - aw_mp3_dec_bit_read(&p_stream->ptr, 2));
    if (p_header->layer == 4) {
        p_stream->error = AW_MP3_DEC_ERR_BADLAYER;
        return AW_ERROR;
    }

    /* protection_bit */
    if (aw_mp3_dec_bit_read(&p_stream->ptr, 1) == 0) {
        p_header->flags     |= AW_MP3_DEC_FRAME_FLAG_PROTECTION;
        p_header->crc_check  = aw_mp3_dec_bit_crc(p_stream->ptr, 16, 0xFFFF);
    }

    /* bitrate_index */
    index = aw_mp3_dec_bit_read(&p_stream->ptr, 4);

    if (index == 15) {
        p_stream->error = AW_MP3_DEC_ERR_BADBITRATE;
        return AW_ERROR;
    }

    if (p_header->flags & AW_MP3_DEC_FRAME_FLAG_LSF_EXT) {
        p_header->bit_rate =
            __g_frame_bit_rate_table[3 + (p_header->layer >> 1)][index];
    } else {
        p_header->bit_rate =
            __g_frame_bit_rate_table[p_header->layer - 1][index];
    }

    /* sampling_frequency */
    index = aw_mp3_dec_bit_read(&p_stream->ptr, 2);

    if (index == 3) {
        p_stream->error = AW_MP3_DEC_ERR_BADSAMPLERATE;
        return AW_ERROR;
    }

    /* real frequency */
    p_header->sample_rate = __g_frame_sample_rate_table[index];

    if (p_header->flags & AW_MP3_DEC_FRAME_FLAG_LSF_EXT) {
        p_header->sample_rate /= 2;

        if (p_header->flags & AW_MP3_DEC_FRAME_FLAG_MPEG_2_5_EXT) {
            p_header->sample_rate /= 2;
        }
    }

    /* padding_bit */
    if (aw_mp3_dec_bit_read(&p_stream->ptr, 1)) {
        p_header->flags |= AW_MP3_DEC_FRAME_FLAG_PADDING;
    }

    /* private_bit */
    if (aw_mp3_dec_bit_read(&p_stream->ptr, 1)) {
        p_header->private_bits |= AW_MP3_DEC_FRAME_PRIVATE_HEADER;
    }

    /* mode */
    p_header->mode           = (enum aw_mp3_dec_frame_mode)
                               (3 - aw_mp3_dec_bit_read(&p_stream->ptr, 2));

    /* mode_extension */
    p_header->mode_extension = aw_mp3_dec_bit_read(&p_stream->ptr, 2);

    /* copyright */
    if (aw_mp3_dec_bit_read(&p_stream->ptr, 1)) {
        p_header->flags |= AW_MP3_DEC_FRAME_FLAG_COPYRIGHT;
    }

    /* original/copy */
    if (aw_mp3_dec_bit_read(&p_stream->ptr, 1)) {
        p_header->flags |= AW_MP3_DEC_FRAME_FLAG_ORIGINAL;
    }

    /* emphasis */
    p_header->emphasis = (enum aw_mp3_dec_frame_emphasis)
                          aw_mp3_dec_bit_read(&p_stream->ptr, 2);

    #if (AW_MP3_DEC_OPT_STRICT)

    /*
    * ISO/IEC 11172-3 says this is a reserved emphasis value, but
    * streams exist which use it anyway. Since the value is not important
    * to the decoder proper, we allow it unless OPT_STRICT is defined.
    */
    if (p_header->emphasis == AW_MP3_DEC_FRAME_EMPHASIS_RESERVED) {
        p_stream->error = AW_MP3_DEC_ERR_BADEMPHASIS;
        return AW_ERROR;
    }
    #endif

    /* crc check */
    if (p_header->flags & AW_MP3_DEC_FRAME_FLAG_PROTECTION) {
        p_header->crc_target = aw_mp3_dec_bit_read(&p_stream->ptr, 16);
    }

    return AW_OK;
}

/******************************************************************************/

/**
 * \brief 初始化帧头部
 * \param[in] p_header : 帧头部结构体
 * \return 无
 */
void aw_mp3_dec_frame_header_init (struct aw_mp3_dec_frame_header *p_header)
{
    p_header->layer            = AW_MP3_DEC_FRAME_LAYER_I;
    p_header->mode             = AW_MP3_DEC_FRAME_MODE_SINGLE_CHANNEL;
    p_header->mode_extension   = 0;
    p_header->emphasis         = AW_MP3_DEC_FRAME_EMPHASIS_NONE;

    p_header->bit_rate         = 0;
    p_header->sample_rate      = 0;

    p_header->crc_check        = 0;
    p_header->crc_target       = 0;

    p_header->flags            = 0;
    p_header->private_bits     = 0;

    aw_mp3_time_init(&p_header->duration);
}

/**
 * \brief 帧头部解码
 */
int aw_mp3_dec_frame_header_decode (struct aw_mp3_dec_frame_header *p_header,
                                    struct aw_mp3_dec_stream       *p_stream)
{
    register uint32_t  pos_ptr, pos_end;
    uint32_t           pad_slot, n;
    uint8_t            data[2];

    pos_ptr = p_stream->pos_next_frame;
    pos_end = p_stream->pos_buf_end;

#if 0
    if (pos_ptr == 0) {
        p_stream->error = AW_MP3_DEC_ERR_BUFPTR;
        goto __fail;
    }
#endif

    /* stream skip */
    if (p_stream->skip_len != 0) {
        if (!p_stream->sync) {
            pos_ptr = p_stream->pos_this_frame;
        }

        if (pos_end - pos_ptr < p_stream->skip_len) {
            p_stream->skip_len       -= pos_end - pos_ptr;
            p_stream->pos_next_frame  = pos_end;

            p_stream->error = AW_MP3_DEC_ERR_BUFLEN;
            goto __fail;
        }

        pos_ptr            += p_stream->skip_len;
        p_stream->skip_len  = 0;
        p_stream->sync      = 1;
    }

__sync:

    aw_mp3_rngbuf_lseek(&p_stream->rngbuf, pos_ptr);
    aw_mp3_rngbuf_read(&p_stream->rngbuf, data, 2);

    /* synchronize */
    if (p_stream->sync) {
        if (pos_end - pos_ptr < AW_MP3_DEC_BUFFER_GUARD) {
            p_stream->pos_next_frame = pos_ptr;
            p_stream->error          = AW_MP3_DEC_ERR_BUFLEN;

            goto __fail;
        } else if (!(data[0] == 0xff && (data[1] & 0xe0) == 0xe0)) {

            /* mark point where frame sync word was expected */
            p_stream->pos_this_frame = pos_ptr;
            p_stream->pos_next_frame = pos_ptr + 1;
            p_stream->error          = AW_MP3_DEC_ERR_LOSTSYNC;

            goto __fail;
        }
    } else {

        aw_mp3_dec_bit_init(&p_stream->ptr,
                            AW_MP3_DEC_BIT_TYPE_RNGBUF,
                            &p_stream->rngbuf,
                            pos_ptr);

        if (aw_mp3_dec_stream_sync(p_stream) != AW_OK) {
            if (pos_end - p_stream->pos_next_frame >= AW_MP3_DEC_BUFFER_GUARD) {
                p_stream->pos_next_frame = pos_end - AW_MP3_DEC_BUFFER_GUARD;
            }

            p_stream->error = AW_MP3_DEC_ERR_BUFLEN;

            goto __fail;
        }

        pos_ptr = aw_mp3_dec_bit_next_byte(&p_stream->ptr);
    }

    /* begin processing */
    p_stream->pos_this_frame = pos_ptr;
    p_stream->pos_next_frame = pos_ptr + 1;  /* possibly bogus sync word */

    aw_mp3_dec_bit_init(&p_stream->ptr,
                        AW_MP3_DEC_BIT_TYPE_RNGBUF,
                        &p_stream->rngbuf,
                        p_stream->pos_this_frame);

    /* decode header */
    if (__frame_decode_header(p_header, p_stream) != AW_OK) {
        goto __fail;
    }

    /* calculate frame duration */
    aw_mp3_time_set(&p_header->duration,
                    0,
                    32 * AW_MP3_DEC_FRAME_NSBSAMPLES(p_header),
                    p_header->sample_rate);

    /* calculate free bit rate */
    if (p_header->bit_rate == 0) {
        if ((p_stream->free_rate == 0 || !p_stream->sync ||
            (p_header->layer == AW_MP3_DEC_FRAME_LAYER_III &&
            p_stream->free_rate > 640000)) &&
            __frame_free_bitrate(p_stream, p_header) != AW_OK) {
            goto __fail;
        }

        p_header->bit_rate  = p_stream->free_rate;
        p_header->flags    |= AW_MP3_DEC_FRAME_FLAG_FREEFORMAT;
    }

    /* calculate beginning of next frame */
    pad_slot = (p_header->flags & AW_MP3_DEC_FRAME_FLAG_PADDING) ? 1 : 0;

    if (p_header->layer == AW_MP3_DEC_FRAME_LAYER_I) {
        n = ((12 * p_header->bit_rate / p_header->sample_rate) + pad_slot) * 4;
    } else {
        uint32_t slots_per_frame;

        slots_per_frame = (p_header->layer == AW_MP3_DEC_FRAME_LAYER_III &&
                          (p_header->flags & AW_MP3_DEC_FRAME_FLAG_LSF_EXT)) ?
                           72 : 144;

        n = (slots_per_frame * p_header->bit_rate /
             p_header->sample_rate) + pad_slot;
    }

    /* verify there is enough data left in buffer to decode this frame */
    if (n + AW_MP3_DEC_BUFFER_GUARD > pos_end - p_stream->pos_this_frame) {
        p_stream->pos_next_frame = p_stream->pos_this_frame;
        p_stream->error          = AW_MP3_DEC_ERR_BUFLEN;
        goto __fail;
    }

    p_stream->pos_next_frame = p_stream->pos_this_frame + n;

    if (!p_stream->sync) {

        /* check that a valid frame header follows this frame */
        pos_ptr = p_stream->pos_next_frame;

        aw_mp3_rngbuf_lseek(&p_stream->rngbuf, pos_ptr);
        aw_mp3_rngbuf_read(&p_stream->rngbuf, data, 2);

        if (!(data[0] == 0xff && (data[1] & 0xe0) == 0xe0)) {
            pos_ptr = p_stream->pos_next_frame = p_stream->pos_this_frame + 1;
            goto __sync;
        }

        p_stream->sync = 1;
    }

    p_header->flags |= AW_MP3_DEC_FRAME_FLAG_INCOMPLETE;
    return AW_OK;

__fail:
    p_stream->sync = 0;
    return AW_ERROR;
}

/**
 * \brief MPEG 帧初始化
 */
void aw_mp3_dec_frame_init (struct aw_mp3_dec_frame *p_frame)
{
    aw_mp3_dec_frame_header_init(&p_frame->header);

    p_frame->options       = 0;

    aw_mp3_dec_frame_mute(p_frame);
}

/**
 * \brief 帧解码
 *
 * \param[in] p_frame  : 帧结构体
 * \param[in] p_stream : 流结构体
 *
 * \return 无
 */
aw_err_t aw_mp3_dec_frame_decode (struct aw_mp3_dec_frame  *p_frame,
                                  struct aw_mp3_dec_stream *p_stream)
{
    pfn_frame_decode_t pfn_decode;

    p_frame->options = p_stream->options;

    /* header() */
    /* error_check() */

    if (!(p_frame->header.flags & AW_MP3_DEC_FRAME_FLAG_INCOMPLETE) &&
        AW_OK != aw_mp3_dec_frame_header_decode(&p_frame->header, p_stream)) {
            goto __fail;
    }

    /* audio_data() */
    p_frame->header.flags &= ~AW_MP3_DEC_FRAME_FLAG_INCOMPLETE;
    pfn_decode             = __g_frame_decoder[p_frame->header.layer - 1];

    if (pfn_decode(p_stream, p_frame) != AW_OK) {
        if (!(p_stream->error & 0xFF00)) {
            p_stream->pos_next_frame = p_stream->pos_this_frame;
        }

        goto __fail;
    }

    /* ancillary_data() */

    if (p_frame->header.layer != AW_MP3_DEC_FRAME_LAYER_III) {
        struct aw_mp3_dec_bit next_frame;

        aw_mp3_dec_bit_init(&next_frame,
                            AW_MP3_DEC_BIT_TYPE_RNGBUF,
                            &p_stream->rngbuf,
                            p_stream->pos_next_frame);

        p_stream->anc_ptr    = p_stream->ptr;
        p_stream->anc_bitlen = aw_mp3_dec_bit_len_get(&p_stream->ptr,
                                                      &next_frame);
    }

    return AW_OK;

__fail:
    p_stream->anc_bitlen = 0;
    return -1;
}

/**
 * \brief 禁音当前帧
 */
void aw_mp3_dec_frame_mute (struct aw_mp3_dec_frame *p_frame)
{
    memset(p_frame->sbsample, 0, sizeof(p_frame->sbsample));
    memset(p_frame->overlap,  0, sizeof(p_frame->overlap));
}

/**
 * \brief 获取通道数量
 * \param[in] p_frame : 帧结构体
 * \return 通道数量
 */
uint8_t
aw_mp3_dec_frame_channel_get (const struct aw_mp3_dec_frame_header *p_header)
{
    uint8_t channel;

    switch (p_header->mode) {

    case AW_MP3_DEC_FRAME_MODE_STEREO:
    case AW_MP3_DEC_FRAME_MODE_JOINT_STEREO:
    case AW_MP3_DEC_FRAME_MODE_DUAL_CHANNEL:
        channel = 2;
        break;

    case AW_MP3_DEC_FRAME_MODE_SINGLE_CHANNEL:
        channel = 1;
        break;

    default:
        channel = 0;
        break;
    }

    return channel;
}

/* end of file */
