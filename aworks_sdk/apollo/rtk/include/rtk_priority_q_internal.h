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
 * \brief 定义rtk中涉及到优先级队列相关的内部函数
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
 * \brief 获取就绪任务中优先级最高的任务
 *
 *  此函数会检查当前优先级最高的任务是否满足优先级轮转的条件
 *  如果满足，则执行优先级轮转，返回新的优先级最高的任务
 *  此函数必须在中断保护中调用
 * \retval    就绪任务中优先级最高的任务
 */
struct rtk_task *_rtk_highest_task_loop_get_ec(void);

/**
 * \brief 获取就绪任务中优先级最高的任务
 *
 *  此函数不考虑优先级轮转
 *  此函数必须在中断保护中调用
 * \retval    就绪任务中优先级最高的任务
 */
struct rtk_task *_rtk_highest_task_get_ec(void);

/**
 * \brief 获取就绪任务中优先级最高的任务TCB的SP成员的地址
 *
 *  此函数必须在中断保护中调用
 * \retval    就绪任务中优先级最高的任务
 */
uint32_t * __rtk_highest_tcb_sp_addr_ec(void);


/**
 * \brief 执行最高优先级任务的时间片轮转
 * 当存在和就绪队列中优先级最高任务同一优先级任务的时候
 *
 * 在systick中需要执行时间片轮转，使得同一优先级的每个任务都能被执行到
 *
 *  此函数必须在中断保护中调用
 * \retval
 */
void __rtk_prio_loop_in_systick( void );

void _task_ready_prio_change_ec(struct rtk_task  *p_task,
                                unsigned int      new_readyq_prio);

#endif          /*__RTK_PRIORITY_Q_INTERNAL_H__*/
