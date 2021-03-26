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
 * \brief 定义rtk中涉及到任务相关的内部函数
 *
 * \internal
 * \par modification history:
 * - 0.9 18-04-23 sni first version.
 * \endinternal
 */


#ifndef             __RTK_TASK_INTERNAL_H__
#define             __RTK_TASK_INTERNAL_H__

/**
 * \brief rtk task全局初始化
 *
 *  内部函数
 *  此函数必须初始化过程中调用一次，以后就不能再调用了
 * \retval    无
 */
void __rtk_task_global_init(void);


#define PRIO_NODE_TO_PTCB(pdn)  \
    RTK_LIST_ENTRY(pdn, struct rtk_task, task_prio_q_node)
#define TICK_NODE_TO_PTCB(pdn)  \
    RTK_LIST_ENTRY(pdn, struct rtk_task, tick_node)
#define PEND_NODE_TO_PTCB(pdn)  \
    RTK_LIST_ENTRY(pdn, struct rtk_task, wait_queue_node)

/**
 * \brief rtk内部初始化一个任务
 *
 * 此函数不对参数作任何检查
 * \param[in]   task        要初始化的任务
 * \param[in]   name        任务的名称
 * \param[in]   priority    任务的优先级
 * \param[in]   option      任务的选项
 * \param[in]   stack_low   任务所使用堆栈内存的最低地址
 * \param[in]   stack_high  任务使用堆栈内存的最高地址
 * \param[in]   pfunc       任务的入口函数
 * \param[in]   arg1        任务入口函数参数1
 * \param[in]   arg2        任务入口函数参数2
 * \retval    失败返回NULL，否则返回指向任务控制块的指针
 */
struct rtk_task *__rtk_task_init_internal(
        struct rtk_task    *task,
        const char         *name,
        int                 priority,
        rtk_task_option_t   option,
        char *              stack_low,
        char *              stack_high,
        void               *pfunc,
        void               *arg1,
        void               *arg2);

/**
 * \brief 在任务环境中执行一次任务调度
 * 此函数的执行会考虑到优先级轮转，它会先获得当前优先级最高的任务
 * 并检查此最高优先级的任务的同优先级任务是否最后执行时刻比此
 * 最高优先级的任务早，如果是，则执行优先级轮转
 *
 * \retval      无
 */
void rtk_schedule(void);

/**
 * \brief 在普通isr中触发一次任务调度
 * 此函数的执行不会考虑到优先级轮转
 * 即他会获取当前优先级最高的任务并尝试切换到它
 *
 * 之所以如此设计，是因为非systick中断不应该引发优先级轮转
 * \retval      无
 */
void rtk_schedule_isr(void);

/**
 * \brief 在systick isr中触发一次任务调度
 * 和rtk_schedule类似，此函数的执行会考虑到优先级轮转
 * 因为优先级轮转本来就由systick引发
 * 但是请注意，rtk_schedule_systick_isr只能够在systick中断的isr中调用
 * 并且仅当在isr中执行了RTK的systick处理逻辑，即g_systick值增加后才能调用
 * 将systick放到线程中处理时，isr中就不要调用此函数了。
 * \retval      无
 */
void rtk_schedule_systick_isr(void);


/**
 * \brief 使得任务开始等待此等待队列
 * 并设置任务唤醒后要执行的回调函数
 *
 * \param[in]   task            要操作的任务
 * \param[in]   p_wait_queue    要等待的等待队列
 * \retval      无
 */
void __task_pend_on_waitq_ec(struct rtk_task  *task,
                             rtk_wait_queue_t *p_wait_queue);

void __task_set_wakeup_after_tick(struct rtk_task *task,
                                  unsigned int    tick);

void __task_unset_wakeup_after_tick(
        struct rtk_task *task);

/**
 * \brief 唤醒正在执行join操作的任务
 *
 * 内部函数，需要在中断保护上下文中调用
 * \param[in]   task        要唤醒正在join的任务
 * \param[in]   err         task执行join操作得到的退出码
 * \retval    无
 */
void __rtk_task_wakeup_joining_task(struct rtk_task *task,void *err);

/**
 * \brief 释放一个被创建的任务的栈，tcb等资源
 *
 *  内部函数
 * \param[in]   task        要释放的任务
 * \param[in]
 * \retval    无
 */
void __rtk_task_release_created_task(struct rtk_task *task);

/**
 * \brief 由wait_queue唤醒一个任务
 *
 *  内部函数
 * \param[in]   task        要释放的任务
 * \param[in]   err         任务唤醒后得到的错误码
 * \retval      无
 */
void __task_wakup_with_err_ec(struct rtk_task *task,
                              int              err);

aw_err_t __task_get_wake_reason(struct rtk_task *task_current);

/**
 * \brief 计算当前最合适的readyq优先级
 *
 *  内部函数，在重新设置任务优先级时调用
 * \param[in]   p_task      要计算的任务
 * \retval      无
 */
unsigned int _task_cal_suit_readyq_prio_ec(struct rtk_task *p_task);


#endif          /* __RTK_TASK_INTERNAL_H__*/
