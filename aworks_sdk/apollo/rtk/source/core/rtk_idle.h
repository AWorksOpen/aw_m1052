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
 * \file  rtk_task_internal.h
 * \brief ����rtk��idle������ص��ڲ�����
 *
 * \internal
 * \par modification history:
 * - 0.9 18-04-23 sni first version.
 * \endinternal
 */


#ifndef             __RTK_IDLE_H__
#define             __RTK_IDLE_H__

struct rtk_task *__rtk_init_idle_task(void);

/**
 * \brief ����rtk��idle����
 *
 *  �ڲ�����
 *  �˺��������ʼ�������е���һ�Σ��Ժ�Ͳ����ٵ�����
 * \retval    ��
 */
void __rtk_start_idle_task(void);

int __rtk_is_startup(void);

/**
 * \brief ��һ��������ӵ�idle������Զ�����������
 *
 * ��CPU����ʱ��idle�����Զ��ͷŵ��Զ��˳�����Ķ�ջ������tcb
 *
 * �˺������Բ������κμ������Ҫ���жϱ����е���
 * \param[in]   task        Ҫ��ʼ��������
 * \retval    ��
 */
void __rtk_add_idle_cleanup_task_ec(struct rtk_task *task);

#endif          /*__RTK_IDLE_H__*/
