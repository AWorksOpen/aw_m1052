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
 * - 1.00 20-10-21  win, first implementation.
 * \endinternal
 */

#include "aw_mp3_internal.h"

/**
 * \brief 初始化内存流对象
 */
aw_err_t aw_mp3_buf_mem_init (aw_mp3_buf_t  *p_buf,
                              const uint8_t *p_mem,
                              int            len)
{
    memset(p_buf, 0, sizeof(*p_buf));

    p_buf->type    = AW_MP3_BUF_TYPE_MEMORY;
    p_buf->p_mem   = (uint8_t *)p_mem;
    p_buf->mem_len = len;

    return AW_OK;
}

/**
 * \brief 初始化文件流对象
 */
aw_err_t aw_mp3_buf_file_init (aw_mp3_buf_t *p_buf,
                               const char   *p_file,
                               int           oflag,
                               mode_t        mode)
{
    int fd;

    memset(p_buf, 0, sizeof(*p_buf));

    p_buf->type = AW_MP3_BUF_TYPE_FILE;
    p_buf->fd = -1;

    fd = aw_open(p_file, oflag, mode);
    if (0 > fd) {
        return fd;
    }

    p_buf->fd = fd;
    return AW_OK;
}

/**
 * \brief 从内存流中读取数据
 */
static ssize_t __mp3_buf_mem_read (aw_mp3_buf_t *p_buf,
                                   void         *p_data,
                                   size_t        nbyte)
{
    int len;

    len = p_buf->mem_len - p_buf->pos;
    if (len > nbyte) {
        len = nbyte;
    }

    memcpy(p_data, p_buf->p_mem + p_buf->pos, len);
    p_buf->pos += len;

    return len;
}

/**
 * \brief 从文件流中读取数据
 */
static ssize_t __mp3_buf_file_read (aw_mp3_buf_t *p_buf,
                                    void         *p_data,
                                    size_t        nbyte)
{
    return aw_read(p_buf->fd, p_data, nbyte);
}

/**
 * \brief 文件流读取
 */
ssize_t aw_mp3_buf_read (aw_mp3_buf_t *p_buf,
                         void         *p_data,
                         size_t        nbyte)
{
    if (AW_MP3_BUF_TYPE_MEMORY == p_buf->type) {
        return __mp3_buf_mem_read(p_buf, p_data, nbyte);
    }

    return __mp3_buf_file_read(p_buf, p_data, nbyte);
}

/**
 * \brief 设置内存流读写操作起始位置
 */
static off_t __mp3_buf_mem_lseek (aw_mp3_buf_t *p_buf,
                                  off_t         offset,
                                  int           whence)
{
    off_t ret = -1;

    switch (whence) {

    case SEEK_SET:
        p_buf->pos = offset;
        break;

    case SEEK_CUR:
        p_buf->pos += offset;
        break;

    case SEEK_END:
        p_buf->pos = p_buf->mem_len +offset;
        break;

    default:
        return ret;
    }

    if (p_buf->pos > p_buf->mem_len) {
        p_buf->pos = p_buf->mem_len;
    }

    if (p_buf->pos < 0) {
        p_buf->pos = 0;
    }

    ret = p_buf->pos;
    return ret;
}

/**
 * \brief 设置文件流读写操作起始位置
 */
static off_t __mp3_buf_file_lseek (aw_mp3_buf_t *p_buf,
                                   off_t         offset,
                                   int           whence)
{
    return aw_lseek(p_buf->fd, offset, whence);
}

/**
 * \brief 移动流读写操作起始位置
 */
off_t aw_mp3_buf_lseek (aw_mp3_buf_t *p_buf,
                        off_t         offset,
                        int           whence)
{
    if (AW_MP3_BUF_TYPE_MEMORY == p_buf->type) {
        return __mp3_buf_mem_lseek(p_buf, offset, whence);
    }

    return __mp3_buf_file_lseek(p_buf, offset, whence);
}

/**
 * \brief 关闭流
 */
void aw_mp3_buf_close (aw_mp3_buf_t *p_buf)
{
    if (AW_MP3_BUF_TYPE_FILE == p_buf->type) {
        aw_close(p_buf->fd);
        p_buf->fd = -1;
    }
}

/**
 * \brief 往内存流中写入数据
 */
static ssize_t __mp3_buf_mem_write (aw_mp3_buf_t *p_buf,
                                    const void   *p_data,
                                    size_t        nbyte)
{
    memcpy(p_buf->p_mem + p_buf->pos, p_data, nbyte);
    p_buf->pos += nbyte;

    return nbyte;
}

/**
 * \brief 往文件流中写入数据
 */
static ssize_t __mp3_buf_file_write (aw_mp3_buf_t *p_buf,
                                     const void   *p_data,
                                     size_t        nbyte)
{
    return aw_write(p_buf->fd, p_data, nbyte);
}

/**
 * \brief 往流中写入数据
 */
ssize_t aw_mp3_buf_write (aw_mp3_buf_t *p_buf,
                          const void   *p_data,
                          size_t        nbyte)
{
    if (AW_MP3_BUF_TYPE_MEMORY == p_buf->type) {
        return __mp3_buf_mem_write(p_buf, p_data, nbyte);
    }

    return __mp3_buf_file_write(p_buf, p_data, nbyte);
}

/* end of file */
