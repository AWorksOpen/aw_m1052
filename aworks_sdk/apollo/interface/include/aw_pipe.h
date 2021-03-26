/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief ͨ�ùܵ�������
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_pipe.h
 *
 * \par ��ʾ��
 * \code
 *
 * #include "apollo.h"
 * #include "aw_pipe.h"
 *
 * AW_PIPE_DECL(pipe);
 *
 * char buffer[60];
 *
 * uint8_t data = 0;
 *
 * size_t ncount = 0;
 *
 * aw_pipe_init(&pipe, buffer, 60);      // ��ʼ���ܵ�������
 *
 *
 * aw_pipe_putbyte(&pipe, 'a', AW_SEM_WAIT_FOREVER);    // ��ܵ���д��һ���ַ�
 * aw_pipe_getbyte(&pipe, &data, AW_SEM_WAIT_FOREVER);  // �ӹܵ��ж�ȡ1���ַ�
 *
 * aw_pipe_write(&pipe, "12345678", 8, &ncount, AW_SEM_WAIT_FOREVER);  // ��ܵ�д��8���ַ�
 * aw_pipe_read(&pipe, &data, 1, &ncount, AW_SEM_WAIT_FOREVER);        // �ӹܵ��ж�ȡ1���ַ�
 *
 * aw_pipe_terminate(&pipe);             // ���ٹܵ�
 * \endcode
 *
 * // �������ݴ���ӡ�����
 *
 * \internal
 * \par modification history
 * - 1.00 15-05-14  dav, create file
 * \endinternal
 */

#ifndef AW_PIPE_H_
#define AW_PIPE_H_

#include "aw_common.h"
#include "aw_sem.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_pipe
 * \copydoc aw_pipe.h
 * @{
 */

/**
 * \brief ����ܵ��ṹ��
 */
#define AW_PIPE_DECL(pipe)  struct aw_pipe pipe;

/**
 * \brief �ܵ����ݹ���ṹ
 * \note ��Ҫֱ�Ӳ������ṹ�ĳ�Ա
 */
struct aw_pipe {
    int    wr;                      /**< \brief ��������ǰд��ƫ�� */
    int    rd;                      /**< \brief ������ ��ǰ��ȡƫ�� */

    int    unread;                  /**< \brief �ܵ��д���δ��ȡ���������� */
    int    len;                     /**< \brief �ܵ����������� */
    char   *buf;                    /**< \brief ������ */

    AW_SEMC_DECL( sem_rd );         /**< \brief �������ź��� */
    AW_SEMC_DECL( sem_wr );         /**< \brief д�����ź��� */
    AW_MUTEX_DECL( mutex );         /**< \brief ���������� */
};

typedef struct aw_pipe *aw_pipe_t;

/**
 * \brief ��ʼ�����򿪹ܵ�
 *
 * \param p_pipe        Ҫ�����Ĺܵ�
 * \param p_buf         ��ʼ������Ļ�����
 * \param nbytes        ��ʼ���Ļ�������С
 *
 * \return ����0��ʾ��ʼ���ɹ��������ʾ��ʼ��ʧ��
 */

aw_err_t aw_pipe_init(aw_pipe_t p_pipe, void *p_buf, size_t nbytes);

/**
 * \brief �ӹܵ��ж�ȡ����
 *
 * \param p_pipe        Ҫ�����Ĺܵ�
 * \param p_buf         ��Ż�ȡ���ݵĻ���
 * \param nbytes        Ҫ��ȡ�����ݸ���
 * \param p_readbytes   �Ѷ�ȡ�������ݸ���
 * \param timeout       �ź�����ʱʱ��
 *
 * \return �ɹ���ȡ�����ݸ���
 */

aw_err_t aw_pipe_read(aw_pipe_t p_pipe, 
                      void     *p_buf, size_t nbytes, 
                      size_t   *p_readbytes, 
                      int       timeout);

/**
 * \brief ��ܵ�д�������ֽ�
 *
 * \param p_pipe        Ҫ�����Ĺܵ�
 * \param p_buf         Ҫ��ŵ��ܵ������������ݻ���
 * \param nbytes        Ҫ��ŵ��ܵ������������ݸ���
 * \param p_writebytes  �ɹ�д����ֽ���Ŀ
 * \param timeout       �ź�����ʱʱ��
 *
 * \return �ɹ���ŵ����ݸ���
 */
aw_err_t aw_pipe_write(aw_pipe_t   p_pipe, 
                       const void *p_buf, 
                       size_t      nbytes, 
                       size_t     *p_writebytes,
                       int         timeout);


/**
 * \brief �ӹܵ���ȡ��һ���ֽ�
 *
 * \param p_pipe        Ҫ�����Ĺܵ�
 * \param p_data        ��Ż�ȡ���ݵĻ���
 * \param timeout       �ź�����ʱʱ��
 *
 * \return �ɹ���ȡ�����ݸ���
 */

aw_err_t aw_pipe_getbyte(aw_pipe_t p_pipe, uint8_t *p_data, int timeout);

/**
 * \brief ��ܵ�д��һ���ֽ�
 *
 * \param p_pipe      Ҫ�����Ĺܵ�
 * \param data        Ҫ��ŵ��������������ֽ�
 * \param timeout     �ź�����ʱʱ��
 *
 * \return �ɹ���ŵ����ݸ���
 */

aw_err_t aw_pipe_putbyte(aw_pipe_t p_pipe, const uint8_t data, int timeout);

/**
 * \brief �رչܵ�
 *
 * \param p_pipe      Ҫ�����Ĺܵ�
 *
 * \return ��״̬
 */

/** @} */

aw_err_t aw_pipe_terminate(aw_pipe_t p_pipe);

#ifdef __cplusplus
}
#endif

#endif /* AW_PIPE_H_ */

/* end of file */
