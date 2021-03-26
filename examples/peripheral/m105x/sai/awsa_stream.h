/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
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

#ifndef __AWSA_STREAM_H
#define __AWSA_STREAM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "string.h"

/**
 * \brief ������
 */
typedef enum awsa_stream_type {
    AWSA_STREAM_TYPE_FILE = 0,
    AWSA_STREAM_TYPE_MEMORY = 1,
} awsa_stream_type_t;

/**
 * \brief ���ṹ��
 */
typedef struct awsa_stream {
    awsa_stream_type_t  type;
    uint8_t            *p_buf;
    int                 pos;
    int                 mem_len;
    int                 fd;
} awsa_stream_t;

/**
 * \brief ��ʼ���ڴ�������
 *
 * \param[in] p_stream : ������ṹ��
 * \param[in] p_buf    : �������Ӧ���ڴ滺����
 * \param[in] len      : ������
 *
 * \retval 0 : �ɹ�
 */
int awsa_stream_mem_init (awsa_stream_t *p_stream,
                          const uint8_t *p_buf,
                          int            len);

/**
 * \brief ��ʼ���ļ�������
 *
 * \param[in] p_stream : ������ṹ��
 * \param[in] p_file   : �������Ӧ���ļ�������
 * \param[in] oflag    :
 *
 * \retval 0 : �ɹ�
 */
int awsa_stream_file_init (awsa_stream_t *p_stream,
                           const char    *p_file,
                           int            oflag,
                           mode_t         mode);

/**
 * \brief ��ȡ��������
 *
 * \param[in] p_stream : ������ṹ��
 * \param[in] p_buf    : ��Ŵ����ж������ݵĻ�����
 * \param[in] nbyte    : ��Ҫ��������
 *
 * \return �������ݵ���ʵ����
 */
ssize_t awsa_stream_read (awsa_stream_t *p_stream,
                          void          *p_buf,
                          size_t         nbyte);

/**
 * \brief �ƶ�����д������ʼλ��
 *
 * \param[in] p_stream : ������ṹ��
 * \param[in] offset   : ƫ��ֵ
 * \param[in] whence   : SEEK_SET, ����offset��Ϊ�µĶ�дλ��
 *                       SEEK_CUR, ��Ŀǰ�Ķ�дλ����������offset��λ����
 *                       SEEK_END, ����дλ��ָ���ļ�β��������offset��λ����
 *                       ��whenceֵΪSEEK_CUR��SEEK_ENDʱ������offset����ֵ�ĳ���
 *
 * \retval 0 : �ɹ�
 */
off_t awsa_stream_lseek (awsa_stream_t *p_stream,
                         off_t          offset,
                         int            whence);

/**
 * \brief �ر���
 * \param[in] p_stream : ������ṹ��
 * \retval 0 : �ɹ�
 */
void awsa_stream_close (awsa_stream_t *p_stream);

/**
 * \brief ������д������
 *
 * \param[in] p_stream : ������ṹ��
 * \param[in] p_buf    : �����Ҫд�������
 * \param[in] nbyte    : ��Ҫд��ĳ���
 *
 * \return д�����ݵ���ʵ����
 */
ssize_t awsa_stream_write (awsa_stream_t *p_stream,
                           const void    *p_buf,
                           size_t         nbyte);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWSA_STREAM_H */

/* end of file */
