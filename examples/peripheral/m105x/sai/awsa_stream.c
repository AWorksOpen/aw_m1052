/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief data stream api
 *
 * \internal
 * \par modification history:
 * - 1.00 16-08-09  win, first implemetation
 * \endinternal
 */

#include "aw_vdebug.h"
#include "aw_common.h"
#include "aw_delay.h"
#include "aw_mem.h"
#include "string.h"
#include "awsa_stream.h"
#include "aw_shell.h"

#include "fs/aw_mount.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "aw_errno.h"

/**
 * \brief 初始化内存流对象
 */
int awsa_stream_mem_init (awsa_stream_t *p_stream,
                          const uint8_t *p_buf,
                          int            len)
{
    memset(p_stream, 0, sizeof(*p_stream));
    p_stream->type = AWSA_STREAM_TYPE_MEMORY;
    p_stream->p_buf = (uint8_t *)p_buf;
    p_stream->mem_len = len;

    return 0;
}

/**
 * \brief 初始化文件流对象
 */
int awsa_stream_file_init (awsa_stream_t *p_stream,
                           const char    *p_file,
                           int            oflag,
                           mode_t         mode)
{
    int             fd;
    memset(p_stream, 0, sizeof(*p_stream));
    p_stream->type = AWSA_STREAM_TYPE_FILE;
    p_stream->fd = -1;

    fd = aw_open(p_file, oflag, mode);
    if ( 0 > fd) {
        return fd;
    }

    p_stream->fd = fd;
    return 0;
}

/**
 * \brief 从内存流中读取数据
 */
static ssize_t __stream_mem_read (awsa_stream_t *p_stream,
                                  void          *p_buf,
                                  size_t         nbyte)
{
    int         len;

    len = p_stream->mem_len - p_stream->pos;
    if (len > nbyte) {
        len = nbyte;
    }
    memcpy(p_buf, p_stream->p_buf + p_stream->pos, len);
    p_stream->pos += len;

    return len;
}

/**
 * \brief 从文件流中读取数据
 */
static ssize_t __stream_file_read (awsa_stream_t *p_stream,
                                   void              *p_buf,
                                   size_t             nbyte)
{
    int         len;
    len = aw_read(p_stream->fd, p_buf, nbyte);
    return len;
}

/**
 * \brief 初始化文件流对象
 */
ssize_t awsa_stream_read (awsa_stream_t *p_stream,
                          void          *p_buf,
                          size_t         nbyte)
{
    if (AWSA_STREAM_TYPE_MEMORY == p_stream->type) {
        return __stream_mem_read(p_stream, p_buf, nbyte);
    }

    return __stream_file_read(p_stream, p_buf, nbyte);
}

/**
 * \brief 设置内存流读写操作起始位置
 */
static off_t __stream_mem_lseek (awsa_stream_t *p_stream,
                                 off_t          offset,
                                 int            whence)
{
    off_t ret = -1;

    switch (whence) {

    case SEEK_SET:
        p_stream->pos = offset;
        break;

    case SEEK_CUR:
        p_stream->pos += offset;
        break;

    case SEEK_END:
        p_stream->pos = p_stream->mem_len +offset;
        break;

    default:
        return ret;
    }

    if (p_stream->pos > p_stream->mem_len) {
        p_stream->pos = p_stream->mem_len;
    }
    if (p_stream->pos < 0) {
        p_stream->pos = 0;
    }
    ret = p_stream->pos;
    return ret;
}

/**
 * \brief 设置文件流读写操作起始位置
 */
static off_t __stream_file_lseek (awsa_stream_t *p_stream,
                                  off_t          offset,
                                  int            whence)
{
    off_t ret = -1;

    ret = aw_lseek(p_stream->fd, offset, whence);

    return ret;
}

/**
 * \brief 移动流读写操作起始位置
 */
off_t awsa_stream_lseek (awsa_stream_t *p_stream,
                         off_t          offset,
                         int            whence)
{
    if (AWSA_STREAM_TYPE_MEMORY == p_stream->type) {
        return __stream_mem_lseek(p_stream, offset, whence);
    }

    return __stream_file_lseek(p_stream, offset, whence);
}

/**
 * \brief 关闭流
 */
void awsa_stream_close (awsa_stream_t *p_stream)
{
    if (AWSA_STREAM_TYPE_FILE == p_stream->type) {
        aw_close(p_stream->fd);
        p_stream->fd = -1;
    }
}

/**
 * \brief 往内存流中写入数据
 */
static ssize_t __stream_mem_write (awsa_stream_t *p_stream,
                                   const void    *p_buf,
                                   size_t         nbyte)
{
    memcpy(p_stream->p_buf + p_stream->pos, p_buf, nbyte);
    p_stream->pos += nbyte;

    return nbyte;
}

/**
 * \brief 往文件流中写入数据
 */
static ssize_t __stream_file_write (awsa_stream_t *p_stream,
                                    const void    *p_buf,
                                    size_t         nbyte)
{
    int         len;
    len = aw_write(p_stream->fd, p_buf, nbyte);
    return len;
}

/**
 * \brief 往流中写入数据
 */
ssize_t awsa_stream_write (awsa_stream_t *p_stream,
                           const void    *p_buf,
                           size_t         nbyte)
{
    if (AWSA_STREAM_TYPE_MEMORY == p_stream->type) {
        return __stream_mem_write(p_stream, p_buf, nbyte);
    }

    return __stream_file_write(p_stream, p_buf, nbyte);
}

/* end of file */
