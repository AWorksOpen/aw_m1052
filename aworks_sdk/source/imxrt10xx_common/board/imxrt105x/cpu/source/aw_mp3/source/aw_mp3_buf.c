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
 * \brief ��ʼ���ڴ�������
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
 * \brief ��ʼ���ļ�������
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
 * \brief ���ڴ����ж�ȡ����
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
 * \brief ���ļ����ж�ȡ����
 */
static ssize_t __mp3_buf_file_read (aw_mp3_buf_t *p_buf,
                                    void         *p_data,
                                    size_t        nbyte)
{
    return aw_read(p_buf->fd, p_data, nbyte);
}

/**
 * \brief �ļ�����ȡ
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
 * \brief �����ڴ�����д������ʼλ��
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
 * \brief �����ļ�����д������ʼλ��
 */
static off_t __mp3_buf_file_lseek (aw_mp3_buf_t *p_buf,
                                   off_t         offset,
                                   int           whence)
{
    return aw_lseek(p_buf->fd, offset, whence);
}

/**
 * \brief �ƶ�����д������ʼλ��
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
 * \brief �ر���
 */
void aw_mp3_buf_close (aw_mp3_buf_t *p_buf)
{
    if (AW_MP3_BUF_TYPE_FILE == p_buf->type) {
        aw_close(p_buf->fd);
        p_buf->fd = -1;
    }
}

/**
 * \brief ���ڴ�����д������
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
 * \brief ���ļ�����д������
 */
static ssize_t __mp3_buf_file_write (aw_mp3_buf_t *p_buf,
                                     const void   *p_data,
                                     size_t        nbyte)
{
    return aw_write(p_buf->fd, p_data, nbyte);
}

/**
 * \brief ������д������
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
