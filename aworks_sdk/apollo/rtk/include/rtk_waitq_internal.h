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
 * \file  rtk_wait_queue.h
 * \brief ����rtk_wait_queue�Ĳ�������
 *
 * \internal
 * \par modification history:
 * - 1.00 18-05-15  sni, first implement
 * \endinternal
 */

#ifndef         __RTK_WAITQ_INTERNAL_H__
#define         __RTK_WAITQ_INTERNAL_H__

void __rtk_waitq_init(
        rtk_wait_queue_t *p_wait_queue,
        rtk_wait_queue_type_t type,
        rtk_wait_queue_option_t option);

/**
 * \brief ������ȴ��ڴ˵ȴ������ϵ�����
 *
 * ��task�����ȼ��ı����ô˺���
 * �����ǰ��µ����ȼ���������task�ڵȴ�p_wait_queue�����λ��
 * ����ֵ�������������к�task�����еȴ�p_wait_queue�������Ƿ����ȼ����
 *
 * \param[in]   p_wait_queue    Ҫִ�в����ĵȴ�����
 * \param[in]   task            Ҫ�������������
 * \retval      0   �������к�task�����еȴ�p_wait_queue���������ȼ��������
 * \retval      ��0 �������к�task�����еȴ�p_wait_queue���������ȼ����
 */
int __rtk_waitq_resort_pender_ec(
        rtk_wait_queue_t *p_wait_queue,
        struct rtk_task *task);

/**
 * \brief ����һ���ȴ��ڴ˵ȴ������ϵ�����
 *          ��������ڽ�����������
 *          �ȴ����������޴�pender
 *
 * \param[in] p_wait_queue  Ҫִ�в����ĵȴ�����
 * \retval    ����������
 */
struct rtk_task *__rtk_waitq_pop_1_pender_ec(rtk_wait_queue_t *p_wait_queue);

/**
 * \brief ��ȡ�˵ȴ����������ȼ���ߵ�pender
 *
 * \param[in] p_wait_queue  Ҫִ�в����ĵȴ�����
 * \retval   �˵ȴ����������ȼ���ߵ�pender����pender����NULL
 */
struct rtk_task *__rtk_waitq_highest_pender_get_lc(
        rtk_wait_queue_t *p_wait_queue);

/**
 * \brief �жϵȴ��������Ƿ��������ڵȴ�
 *
 * \param[in] p_wait_queue  Ҫִ�в����ĵȴ�����
 * \retval      0       û�������ڵȴ�
 *              ����ֵ  �������ٵȴ�
 */
int __rtk_waitq_has_penders_ec(
        rtk_wait_queue_t *p_wait_queue );


/**
 * \brief ��ȡ�ȴ��ڴ˵ȴ������ϵ������������ȼ�
 *
 * \param[in] p_wait_queue  Ҫִ�в����ĵȴ�����
 * \retval   �ȴ��ڴ˵ȴ������ϵ������������ȼ�
 */
int __rtk_waitq_highest_pri_get_lc (
        rtk_wait_queue_t *p_wait_queue );

/**
 * \brief ������񵽵ȴ����У�ʹ������ʼ�ȴ��˵ȴ�����
 *
 * \param[in]   task            Ҫ��ӵ�����
 * \param[in]   p_wait_queue    Ҫִ�в����ĵȴ�����
 * \retval   ��
 */
void __rtk_waitq_add_pender_ec(
        struct rtk_task *task ,
        rtk_wait_queue_t *p_wait_queue );



/**
 * \brief ������ӵȴ��������Ƴ�
 * \param[in]   task            Ҫ�Ƴ�������
 * \retval      ��
 */
void __task_detach_pending_waitq_ec( struct rtk_task *task );

int __rtk_waitq_terminate( rtk_wait_queue_t *p_wait_queue );


#endif          //__RTK_WAIT_QUEUE_H__
