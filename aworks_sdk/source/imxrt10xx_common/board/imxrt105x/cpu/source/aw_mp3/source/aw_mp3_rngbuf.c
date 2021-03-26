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
 * - 1.00 20-10-27  win, first implementation.
 * \endinternal
 */

#include "aw_mp3_internal.h"

/**
 * \brief 初始化环形缓冲区
 */
aw_err_t aw_mp3_rngbuf_init (struct aw_mp3_rngbuf *p_rngbuf,
                             struct aw_mp3_buf    *p_buf,
                             off_t                 off)
{
    p_rngbuf->p_buf = p_buf;

    p_rngbuf->off1 = off;
    aw_mp3_buf_lseek(p_rngbuf->p_buf, p_rngbuf->off1, SEEK_SET);
    aw_mp3_buf_read(p_rngbuf->p_buf, p_rngbuf->buffer1, AW_MP3_RNGBUF_SIZE);

    p_rngbuf->off2 = p_rngbuf->off1 + AW_MP3_RNGBUF_SIZE;
    aw_mp3_buf_lseek(p_rngbuf->p_buf, p_rngbuf->off2, SEEK_SET);
    aw_mp3_buf_read(p_rngbuf->p_buf, p_rngbuf->buffer2, AW_MP3_RNGBUF_SIZE);

    p_rngbuf->p_cur = p_rngbuf->buffer1;
    p_rngbuf->out   = 0;
    return AW_OK;
}

/**
 * \brief 获取1个字节
 */
uint8_t aw_mp3_rngbuf_get (struct aw_mp3_rngbuf *p_rngbuf)
{
    if (p_rngbuf->p_cur == p_rngbuf->buffer1) {
        aw_mp3_rngbuf_lseek(p_rngbuf, p_rngbuf->off1 + p_rngbuf->out);
    } else {
        aw_mp3_rngbuf_lseek(p_rngbuf, p_rngbuf->off2 + p_rngbuf->out);
    }
    return p_rngbuf->p_cur[p_rngbuf->out++];
}

/**
 * \brief 获取数据
 */
aw_err_t aw_mp3_rngbuf_read (struct aw_mp3_rngbuf *p_rngbuf,
                             uint8_t              *p_buf,
                             uint32_t              len)
{
    uint32_t i;

    for (i = 0; i < len; i++) {
        p_buf[i] = aw_mp3_rngbuf_get(p_rngbuf);
    }

    return AW_OK;
}

/**
 * \brief 调整索引位置
 */
aw_err_t aw_mp3_rngbuf_lseek (struct aw_mp3_rngbuf *p_rngbuf,
                              off_t                 off)
{
    if (p_rngbuf->off1 < p_rngbuf->off2) {
        aw_assert((p_rngbuf->off2 - p_rngbuf->off1) == AW_MP3_RNGBUF_SIZE);

        /* 更新off1 的值 */
        if (off >= p_rngbuf->off2 + AW_MP3_RNGBUF_SIZE) {

            /* 跳过off2，因为off2指向的buffer数据还需要保留 */
            p_rngbuf->off1 += (2 * AW_MP3_RNGBUF_SIZE);

            aw_assert(off < (p_rngbuf->off1 + AW_MP3_RNGBUF_SIZE));

            /* out 指向buffer1中的数据 */
            p_rngbuf->p_cur = p_rngbuf->buffer1;

            aw_mp3_buf_lseek(p_rngbuf->p_buf, p_rngbuf->off1, SEEK_SET);
            aw_mp3_buf_read(p_rngbuf->p_buf,
                            p_rngbuf->p_cur,
                            AW_MP3_RNGBUF_SIZE);

            p_rngbuf->out = off - p_rngbuf->off1;
            return AW_OK;
        }

        if (off >= p_rngbuf->off2) {
            p_rngbuf->p_cur = p_rngbuf->buffer2;
            p_rngbuf->out = off - p_rngbuf->off2;
            return AW_OK;
        }

        if (off >= p_rngbuf->off1) {
            p_rngbuf->p_cur = p_rngbuf->buffer1;
            p_rngbuf->out = off - p_rngbuf->off1;
            return AW_OK;
        }

        aw_assert(off >= p_rngbuf->off1);

    } else if (p_rngbuf->off1 > p_rngbuf->off2) {

        aw_assert((p_rngbuf->off1 - p_rngbuf->off2) == AW_MP3_RNGBUF_SIZE);

        /* 更新off2 的值 */
        if (off >= p_rngbuf->off1 + AW_MP3_RNGBUF_SIZE) {

            /* 跳过off1，因为off1指向的buffer数据还需要保留 */
            p_rngbuf->off2 += (2 * AW_MP3_RNGBUF_SIZE);

            aw_assert(off < (p_rngbuf->off2 + AW_MP3_RNGBUF_SIZE));

            /* out 指向buffer1中的数据 */
            p_rngbuf->p_cur = p_rngbuf->buffer2;

            aw_mp3_buf_lseek(p_rngbuf->p_buf, p_rngbuf->off2, SEEK_SET);
            aw_mp3_buf_read(p_rngbuf->p_buf,
                            p_rngbuf->p_cur,
                            AW_MP3_RNGBUF_SIZE);

            p_rngbuf->out = off - p_rngbuf->off2;
            return AW_OK;
        }

        if (off >= p_rngbuf->off1) {
            p_rngbuf->p_cur = p_rngbuf->buffer1;
            p_rngbuf->out = off - p_rngbuf->off1;
            return AW_OK;
        }

        if (off >= p_rngbuf->off2) {
            p_rngbuf->p_cur = p_rngbuf->buffer2;
            p_rngbuf->out = off - p_rngbuf->off2;
            return AW_OK;
        }

        aw_assert(off >= p_rngbuf->off2);
    } else {
        aw_assert(p_rngbuf->off1 != p_rngbuf->off2);
    }

    return AW_OK;
}

/* end of file */
