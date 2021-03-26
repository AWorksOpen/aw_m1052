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
 * \file  rtk_mutex.h
 * \brief rtk����mutex�Ľṹ�ͺ���
 *
 * \internal
 * \par modification history:
 * - 1.00 18-05-15  sni, first implement
 * \endinternal
 */

#ifndef         __RTK_MUTEX_H__
#define         __RTK_MUTEX_H__

struct rtk_mutex
{
    rtk_wait_queue_t    wait_queue;
    struct rtk_task    *owner;
    /** \brief ���������б�����л���������Ľڵ� */
    rtk_list_node_t     mutex_hold_node;     
    /** \brief ���������ݹ���еĴ��� */    
    unsigned long       mutex_recurse_count;    
    unsigned long       magic_num;
};

/**
 *  @brief mutex declaration macro.
 */
#define RTK_MUTEX_DECL(var) struct rtk_mutex var

/**
 * \brief ��ʼ��һ��������
 *
 * �������ж��е���
 *
 * \param[in] p_mutex       struct rtk_mutex�ṹ
 *
 * \retval    0             �ɹ������򷵻ظ��Ĵ�����
 */
int rtk_mutex_init( struct rtk_mutex *p_mutex );

/**
 *  \brief aquire a mutex lock.
 *
 *  \param[in] p_mutex    mutex pointer
 *  \param[in] tick     max waiting time in systerm tick.
 *                      if tick == 0, it will return immedately without block.
 *                      if tick == -1, it will wait forever.
 *
 *  \return     0       successfully.
 *  \return     -EPERM  permission denied.
 *  \return     -EINVAL Invalid argument
 *  \return     -ETIME  time out.
 *  \return     -EDEADLK Deadlock condition detected.
 *  \return     -ENXIO  mutex is terminated by other task or interrupt service routine.
 *  \return     -EAGAIN Try again. Only when tick==0 and mutex is not available.
 *
 *  \attention          cannot be used in interrupt service.
 */
/**
 * \brief ��ȡһ��������
 *
 * �������ж��е���
 *
 * \param[in] p_mutex       struct rtk_mutex�ṹ
 * \param[in] tick          ��system tick���������ȴ�ʱ��
 *                          0��ʾ�����Ƿ��ȡ�ɹ���������������
 *                          AW_WAIT_FOREVER����ʾ�����õȴ�
 *
 * \retval  0               �ɹ�����0�����򷵻ظ��Ĵ�����
 * \retval  -AW_EINVAL      ������Ч
 * \retval  -AW_ETIME       ��ȡ��ʱ
 * \retval  -AW_ENXIO       ������������������ֹ��
 * \retval  -AW_EAGAIN      ��Ҫ���ԣ�ֻ��tick==0���Ҳ��������������Ż᷵��
 * \retval  -AW_EPERM       ���ж��е���
 */
int rtk_mutex_lock( struct rtk_mutex *p_mutex, unsigned int tick );

/**
 * \brief �ͷ�һ��������
 *
 * �������ж��е���
 *
 * \param[in] p_mutex       struct rtk_mutex�ṹ
 *
 * \retval  0               �ɹ�����0�����򷵻ظ��Ĵ�����
 * \retval  -AW_EINVAL      ������Ч
 * \retval  -AW_EPERM       ���ж��е��ã����ߵ�ǰ������mutex��owener
 */
int rtk_mutex_unlock( struct rtk_mutex *p_mutex );

/**
 * \brief ����ʼ��һ��������
 *
 * �������ж��е��ã��κ�һ���ɹ���ʼ���Ļ����������Ե���rtk_mutex_terminate
 * ������ʼ��������ʼ���ỽ�����Եȴ��ڴ��ź����ϵ�������Щ�����lock������
 * �õ�����ֵ-AW_ENXIO
 *
 * \param[in] p_mutex       struct rtk_mutex�ṹ
 * \retval    0             �ɹ�����0�����򷵻ظ��Ĵ�����
 * \retval    -AW_EPERM     û��Ȩ�޽��д˲������������ж��в���
 * \retval    -AW_EINVAL    ��Ч�Ĳ���
 */
int rtk_mutex_terminate( struct rtk_mutex *p_mutex );


#endif          //__RTK_MUTEX_H__
