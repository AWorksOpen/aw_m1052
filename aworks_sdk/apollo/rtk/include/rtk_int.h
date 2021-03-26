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
 * \file  rtk_int.h
 * \brief rtk关于中断的结构和函数
 *
 * \internal
 * \par modification history:
 * - 1.00 18-05-15  sni, first implement
 * \endinternal
 */

#ifndef         __RTK_INT_H__
#define         __RTK_INT_H__

#include "rtk_barrier.h"

/**
 * \brief 判断当前是否运行在isr环境中
 *
 * \retval    1             当前运行在isr环境中
 * \retval    0             当前不是运行在isr环境中
 */
int rtk_is_int_context(void);

/**
 * \brief 判断当前是否运行在fault环境中
 *
 * \retval    1             当前运行在fault环境中
 * \retval    0             当前不是运行在fault环境中
 */
int rtk_is_fault_context(void);

#define RTK_INT_STATUS_DECL(save_int_status)    uint32_t save_int_status;

/**
 * \brief 禁用当前CPU中断
 *
 * \return 返回禁用中断前的CPU中断标志
 */
uint32_t __rtk_interrupt_disable(void);

#define RTK_INTERRUPT_DISABLE(save_int_status)    \
    rtk_barrier(); (save_int_status) = __rtk_interrupt_disable(); rtk_barrier();

/**
 * \brief 启用当前CPU中断
 *
 * \param   old     上次__rtk_interrupt_disable的返回值
 */
void __rtk_interrupt_enable(uint32_t old);

#define RTK_INTERRUPT_ENABLE(old)    \
    rtk_barrier();__rtk_interrupt_enable((old)); rtk_barrier();


#define RTK_CRITICAL_STATUS_DECL(old) uint32_t  old

/**
 * \brief 进入操作系统临界区
 *
 * 在相当多的CPU上，进入操作系统临界区等同于禁用CPU中断
 * 但是，这可能并不完全正确，某些情况下进入操作系统临界区仅仅意味着
 * 某个优先级之下的中断被禁止执行，之上优先级的中断依然可以继续执行
 * 当然这些中断的isr绝对不能调用任何操作系统API，除非它特别声明可以调用
 *
 * \return 进入操作系统临界区之前的临界区状态
 */
uint32_t  __rtk_enter_critical( void );
#define RTK_ENTER_CRITICAL(save_status)    \
    rtk_barrier();(save_status) = __rtk_enter_critical(); rtk_barrier();

/**
 * \brief 离开操作系统临界区
 *
 * \param   old     前次rtk_enter_critical的返回值
 */
void __rtk_exit_critical(uint32_t old_status );
#define RTK_EXIT_CRITICAL(old)    \
    rtk_barrier();__rtk_exit_critical(old); rtk_barrier();




typedef void (*pfn_rtk_hook_t)(void);
void rtk_enter_critical_hook_set(pfn_rtk_hook_t hook_fn);
void rtk_exit_critical_hook_set(pfn_rtk_hook_t hook_fn);

void rtk_sched_begin_hook_set(pfn_rtk_hook_t hook_fn);
void _rtk_sched_begin_hook_exec(void);
void rtk_sched_end_hook_set(pfn_rtk_hook_t hook_fn);
void _rtk_sched_end_hook_exec(void);

void rtk_isr_enter_hook_set(pfn_rtk_hook_t hook_fn);
void rtk_isr_exit_hook_set(pfn_rtk_hook_t hook_fn);

/**
 * \brief 刚进入ISR时调用，根据需要完成一些ISR辅助功能，
 *        例如保证对中断上下文的判断，记录isr的开始执行时刻，调用追踪hook函数等
 */
void __rtk_enter_int_context( void );

/**
 * \brief 退出ISR钱调用，根据需要完成一些ISR的辅助功能
 *          例如保证对中断上下文的判断，记录isr的退出时刻，调用追踪hook函数等
 */
void __rtk_exit_int_context( void );


/**
 * \brief 在刚进入isr中调用RTK_ENTER_INT_CONTEXT，
 *        可以向rtk表明当前进入了中断上下文
 *
 * 通常情况下，rtk内部会通过rtk_is_int_context函数来判断当前是否执行在中断上下文
 * 而rtk_is_int_context内部会直接读取CPU状态寄存器来完成判断，所以RTK_ENTER_INT_CONTEXT
 * 不需要执行任何东西，为提高效率，将其定义为空宏
 *
 * 然而某些CPU可能无法通过读取CPU状态寄存器来判断中断状态，这是RTK_ENTER_INT_CONTEXT
 * 内部实现就可以通过设置某个变量，rtk_is_int_context就可以通过这个变量来判断是否
 * 中断上下文
 *
 * 更进一步，某些时候我们可能需要统计每个ISR的执行时间，或者通过某种手段来追踪系
 * 统执行路径这时RTK_ENTER_INT_CONTEXT内部实现就可以完成这些操作
 *
 * 终上所述，通常情况下RTK_ENTER_INT_CONTEXT被宏定义为空，以保证执行效率
 * 在需要额外辅助来判断中断状态，或者需要统计ISR的执行时间，或者追踪系统执行路径
 * 的情况下RTK_ENTER_INT_CONTEXT被宏定义为__rtk_enter_int_context
 */
 
#define RTK_ENTER_INT_CONTEXT() 

/**
 * \brief 在退出isr前调用RTK_EXIT_INT_CONTEXT，
 *        可以向rtk表明当前准备离开中断上下文
 *
 * 类似RTK_ENTER_INT_CONTEXT中已经解释过得原因，
 * 通常情况下RTK_EXIT_INT_CONTEXT被宏定义为空，以保证执行效率
 * 在需要额外辅助来判断中断状态，或者需要统计ISR的执行时间，或者
 * 追踪系统执行路径的情况下
 * RTK_EXIT_INT_CONTEXT被宏定义为__rtk_exit_int_context
 */
#define RTK_EXIT_INT_CONTEXT()



#endif          //__RTK_INT_H__
