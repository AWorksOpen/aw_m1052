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

#ifndef __AW_MP3_RNGBUF_H
#define __AW_MP3_RNGBUF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_mp3_internal.h"

#define AW_MP3_RNGBUF_SIZE (2 * 1024)

/**
 * \brief MP3环形缓冲区
 */
struct aw_mp3_rngbuf {
    uint32_t           out;         /**< \brief 缓冲区数据写入位置 */
    uint8_t            buffer1[AW_MP3_RNGBUF_SIZE];
    uint8_t            buffer2[AW_MP3_RNGBUF_SIZE];
    off_t              off1;
    off_t              off2;
    uint8_t           *p_cur;

    struct aw_mp3_buf *p_buf;
};

/**
 * \brief 初始化环形缓冲区
 */
aw_err_t aw_mp3_rngbuf_init (struct aw_mp3_rngbuf *p_rngbuf,
                             struct aw_mp3_buf    *p_buf,
                             off_t                 off);

/**
 * \brief 获取1个字节
 */
uint8_t aw_mp3_rngbuf_get (struct aw_mp3_rngbuf *p_rngbuf);

/**
 * \brief 获取数据
 */
aw_err_t aw_mp3_rngbuf_read (struct aw_mp3_rngbuf *p_rngbuf,
                             uint8_t              *p_buf,
                             uint32_t              len);

/**
 * \brief 调整索引位置
 */
aw_err_t aw_mp3_rngbuf_lseek (struct aw_mp3_rngbuf *p_rngbuf,
                              off_t                 off);

#ifdef __cplusplus
}
#endif

#endif /* __AW_MP3_RNGBUF_H */

/* end of file */

