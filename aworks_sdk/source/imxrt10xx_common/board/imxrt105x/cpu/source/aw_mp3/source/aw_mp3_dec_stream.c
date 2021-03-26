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

/**
 * \brief 流初始化
 */
void aw_mp3_dec_stream_init (struct aw_mp3_dec_stream *p_stream)
{
    p_stream->pos_buf_fer    = 0;
    p_stream->pos_buf_end    = 0;
    p_stream->skip_len       = 0;

    p_stream->sync           = 0;
    p_stream->free_rate      = 0;

    p_stream->pos_this_frame = 0;
    p_stream->pos_next_frame = 0;

    aw_mp3_dec_bit_init(&p_stream->ptr,     AW_MP3_DEC_BIT_TYPE_BUF, NULL, 0);
    aw_mp3_dec_bit_init(&p_stream->anc_ptr, AW_MP3_DEC_BIT_TYPE_BUF, NULL, 0);

    p_stream->anc_bitlen = 0;

    //memset(p_stream->md, 0, sizeof(p_stream->md));
    aw_mp3_buf_mem_init(&p_stream->md_buf,
                        p_stream->md,
                        AW_MP3_DEC_BUFFER_MD_LEN);

    p_stream->md_len     = 0;

    p_stream->options    = 0;
    p_stream->error      = AW_MP3_DEC_ERR_NONE;
}

/**
 * \brief 设置流缓冲区
 */
void aw_mp3_dec_stream_buffer_set (struct aw_mp3_dec_stream *p_stream,
                                   aw_mp3_buf_t             *p_buffer,
                                   uint32_t                  pos,
                                   uint32_t                  len)
{
    p_stream->pos_buf_fer    = pos;
    p_stream->pos_buf_end    = pos + len;

    p_stream->pos_this_frame = pos;
    p_stream->pos_next_frame = pos;

    //p_stream->p_buf          = p_buffer;
    aw_mp3_rngbuf_init(&p_stream->rngbuf, p_buffer, pos);

    p_stream->sync = 1;

    aw_mp3_dec_bit_init(&p_stream->ptr,
                        AW_MP3_DEC_BIT_TYPE_RNGBUF,
                        &p_stream->rngbuf,
                        pos);
}

/**
 * \brief 跳过固定长度
 */
void aw_mp3_dec_stream_skip (struct aw_mp3_dec_stream *p_stream,
                             uint32_t                  len)
{
    p_stream->skip_len += len;
}

/**
 * \brief 定位下一个同步字
 */
aw_err_t aw_mp3_dec_stream_sync (struct aw_mp3_dec_stream *p_stream)
{
    register uint32_t pos_ptr, pos_end;
    uint8_t           data[2];

    pos_ptr = aw_mp3_dec_bit_next_byte(&p_stream->ptr);
    pos_end = p_stream->pos_buf_end;

    while (pos_ptr < pos_end - 1) {
        aw_mp3_rngbuf_lseek(&p_stream->rngbuf, pos_ptr);
        aw_mp3_rngbuf_read(&p_stream->rngbuf, data, 2);

        if (data[0] == 0xff && (data[1] & 0xe0) == 0xe0) {
            break;
        }

        ++pos_ptr;
    }

    if (pos_end - pos_ptr < AW_MP3_DEC_BUFFER_GUARD) {
        return AW_ERROR;
    }

    aw_mp3_dec_bit_init(&p_stream->ptr,
                        AW_MP3_DEC_BIT_TYPE_RNGBUF,
                        &p_stream->rngbuf,
                        pos_ptr);
    return AW_OK;
}

/* end of file */
