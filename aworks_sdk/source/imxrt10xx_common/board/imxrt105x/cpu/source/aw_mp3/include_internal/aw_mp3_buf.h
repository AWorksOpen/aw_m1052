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

#ifndef __AW_MP3_BUF_H
#define __AW_MP3_BUF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_mp3_internal.h"

/** \brief �ļ���ȡ������ */
#define AW_MP3_BUF_FD_SIZE    (1024)

/**
 * \brief ����������
 */
enum aw_mp3_buf_type {
    AW_MP3_BUF_TYPE_FILE = 0,
    AW_MP3_BUF_TYPE_MEMORY = 1,
};

/**
 * \brief �������ṹ��
 */
typedef struct aw_mp3_buf {
    enum aw_mp3_buf_type  type;
    uint8_t              *p_mem;
    int                   pos;
    int                   mem_len;
    int                   fd;
} aw_mp3_buf_t;

/**
 * \brief ��ʼ���ڴ�������
 */
aw_err_t aw_mp3_buf_mem_init (aw_mp3_buf_t  *p_buf,
                              const uint8_t *p_mem,
                              int            len);

/**
 * \brief ��ʼ���ļ�������
 */
aw_err_t aw_mp3_buf_file_init (aw_mp3_buf_t *p_buf,
                               const char   *p_file,
                               int           oflag,
                               mode_t        mode);

/**
 * \brief �ļ�����ȡ
 */
ssize_t aw_mp3_buf_read (aw_mp3_buf_t *p_buf,
                         void         *p_data,
                         size_t        nbyte);

/**
 * \brief �ƶ�����д������ʼλ��
 */
off_t aw_mp3_buf_lseek (aw_mp3_buf_t *p_buf,
                        off_t         offset,
                        int           whence);

/**
 * \brief �ر���
 */
void aw_mp3_buf_close (aw_mp3_buf_t *p_buf);

/**
 * \brief ������д������
 */
ssize_t aw_mp3_buf_write (aw_mp3_buf_t *p_buf,
                          const void   *p_data,
                          size_t        nbyte);

#ifdef __cplusplus
}
#endif

#endif /* __AW_MP3_BUF_H */

/* end of file */
