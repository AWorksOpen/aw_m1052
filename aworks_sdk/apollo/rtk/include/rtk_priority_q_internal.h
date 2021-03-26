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
 * \file  rtk_priority_q_internal.h
 * \brief ����rtk���漰�����ȼ�������ص��ڲ�����
 *
 * \internal
 * \par modification history:
 * - 1.00 18-04-23 sni first version.
 * \endinternal
 */


#ifndef             __RTK_PRIORITY_Q_INTERNAL_H__
#define             __RTK_PRIORITY_Q_INTERNAL_H__

typedef struct
{
    struct rtk_task                    *p_highest_task;
    unsigned int                        bitmap_group;
}rtk_ready_q_t;

typedef struct
{
    rtk_list_node_t                    *prio_tasks_lists;
    uint32_t                           *prio_bitmaps;
}rtk_readyq_info_t;

typedef const rtk_readyq_info_t *p_rtk_readyq_const_info_t;

typedef rtk_task_prio_q_node_t pqn_t;

#define READY_Q_REMOVE( task )      _rtk_readyq_remove( task )
#define READY_Q_PUT( task)          _rtk_readyq_put(task)

void priority_q_init( void );

void _rtk_readyq_put_idle_task(struct rtk_task *p_task );
void _rtk_readyq_put(struct rtk_task *p_task);
int _rtk_readyq_remove( struct rtk_task *p_task );

/**
 * \brief ��ȡ�������������ȼ���ߵ�����
 *
 *  �˺������鵱ǰ���ȼ���ߵ������Ƿ��������ȼ���ת������
 *  ������㣬��ִ�����ȼ���ת�������µ����ȼ���ߵ�����
 *  �˺����������жϱ����е���
 * \retval    �������������ȼ���ߵ�����
 */
struct rtk_task *_rtk_highest_task_loop_get_ec(void);

/**
 * \brief ��ȡ�������������ȼ���ߵ�����
 *
 *  �˺������������ȼ���ת
 *  �˺����������жϱ����е���
 * \retval    �������������ȼ���ߵ�����
 */
struct rtk_task *_rtk_highest_task_get_ec(void);

/**
 * \brief ��ȡ�������������ȼ���ߵ�����TCB��SP��Ա�ĵ�ַ
 *
 *  �˺����������жϱ����е���
 * \retval    �������������ȼ���ߵ�����
 */
uint32_t * __rtk_highest_tcb_sp_addr_ec(void);


/**
 * \brief ִ��������ȼ������ʱ��Ƭ��ת
 * �����ں;������������ȼ��������ͬһ���ȼ������ʱ��
 *
 * ��systick����Ҫִ��ʱ��Ƭ��ת��ʹ��ͬһ���ȼ���ÿ�������ܱ�ִ�е�
 *
 *  �˺����������жϱ����е���
 * \retval
 */
void __rtk_prio_loop_in_systick( void );

void _task_ready_prio_change_ec(struct rtk_task  *p_task,
                                unsigned int      new_readyq_prio);

#endif          /*__RTK_PRIORITY_Q_INTERNAL_H__*/
