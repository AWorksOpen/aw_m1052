/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file   rtk_config.h
 * \brief  rtk 内核配置文件
 *
 * 用户在此文件中对模块的相关参数进行配置
 *
 * \internal
 * \par modification history:
 * - 1.00 12-10-23  or2, first implementation
 * \endinternal
 */
 
#ifndef __RTK_CONFIG_H
#define __RTK_CONFIG_H

#include "rtk_autoconf.h"

/**
 * \addtogroup grp_aw_task_usrcfg
 * @{
 */

/**
 * \name 优先级数目配置
 * @{
 */

// 实时优先级数目
#ifdef CONFIG_REALTIME_PRIORITY_COUNT
#define CONFIG_RTK_REALTIME_PRIORITY_COUNT      CONFIG_REALTIME_PRIORITY_COUNT
#else
#define CONFIG_RTK_REALTIME_PRIORITY_COUNT      2
#endif

// 系统优先级数目
#ifdef CONFIG_SYS_PRIORITY_COUNT
#define CONFIG_RTK_SYS_PRIORITY_COUNT      CONFIG_SYS_PRIORITY_COUNT
#else
#define CONFIG_RTK_SYS_PRIORITY_COUNT           8
#endif

// 用户优先级数目
#ifdef CONFIG_USR_PRIORITY_COUNT
#define CONFIG_RTK_USR_PRIORITY_COUNT      CONFIG_USR_PRIORITY_COUNT
#else
#define CONFIG_RTK_USR_PRIORITY_COUNT           20
#endif

// 系统低优先级数目
#ifdef CONFIG_SYS_LOW_PRIORITY_COUNT
#define CONFIG_RTK_SYS_LOW_PRIORITY_COUNT      CONFIG_SYS_LOW_PRIORITY_COUNT
#else
#define CONFIG_RTK_SYS_LOW_PRIORITY_COUNT       2
#endif
/** @} */

// 使能对象的动态创建，这样就可以创建任务，信号量，互斥锁等

#ifdef CONFIG_OBJECT_CREATE_EN
#define CONFIG_RTK_OBJECT_CREATE_EN             1
#else
#define CONFIG_RTK_OBJECT_CREATE_EN             0
#endif



// 是否使能处理systick任务

#ifdef CONFIG_SYSTICK_TASK_EN
#define CONFIG_RTK_SYSTICK_TASK_EN             1
#else
#define CONFIG_RTK_SYSTICK_TASK_EN             0
#endif

#define CONFIG_RTK_SYSTICK_TASK_PRIO            (CONFIG_RTK_REALTIME_PRIORITY_COUNT + \
                                                    CONFIG_RTK_SYS_PRIORITY_COUNT - 1)

#ifdef CONFIG_SYSTICK_TASK_STACK_SIZE
#define CONFIG_RTK_SYSTICK_TASK_STACK_SIZE      CONFIG_SYSTICK_TASK_STACK_SIZE
#else
#define CONFIG_RTK_SYSTICK_TASK_STACK_SIZE      2048
#endif
/**
 * \brief 空闲任务栈大小
 *
 * 由于CPU资源可能有限，为了避免不必要的RAM消耗，用户可以将此参数设置为实际
 * 需要的大小, 如果定义了IDLE_TASK_HOOK,特别要注意栈不能太小。
 *
 * \hideinitializer
 */
 
#ifdef CONFIG_IDLE_TASK_STACK_SIZE
#define CONFIG_RTK_IDLE_TASK_STACK_SIZE         CONFIG_IDLE_TASK_STACK_SIZE
#else
#define CONFIG_RTK_IDLE_TASK_STACK_SIZE         2048
#endif
/**
 * \brief 死锁检查使能
 *
 * 使能后能检查出死锁，并返回-EDEADLK.
 * 1: 使能
 * 0: 禁止
 * \hideinitializer
 */

#ifdef CONFIG_DEAD_LOCK_DETECT_ENABLE
#define DEAD_LOCK_DETECT_EN             1
#else
#define DEAD_LOCK_DETECT_EN             0
#endif

/**
 * \brief 死锁显示使能
 *
 * 需要kprintf支持。
 * 1: 使能
 * 0: 禁止
 * \hideinitializer
 */

#ifdef CONFIG_DEAD_LOCK_SHOW_EN
#define DEAD_LOCK_SHOW_EN             1
#else
#define DEAD_LOCK_SHOW_EN             0
#endif



#ifdef CONFIG_KERNEL_ARG_CHECK_EN
#define KERNEL_ARG_CHECK_EN             1
#else
#define KERNEL_ARG_CHECK_EN             0
#endif

// 使得wait_queue的pend操作时间常量化
// 如果禁用此选项，则wait_queue的pend操作时间会与已经pend在此wait_queue上的任务个数和
// 优先级相关，进而导致所有的IPC的take lock操作都会与pending任务和优先级相关
// 使能此选项，则占用内存较多，但是wait_queue的pending操作时间因子变为O(1)
// 即常量时间，wakeup操作的时间至于系统优先级数量相关，但系统优先级数量每个配置是确定的
// 所以wakeup时间因子也是O(1)

#ifdef CONFIG_WAIT_QUEUE_PEND_CONST_TIME
#define CONFIG_RTK_WAIT_QUEUE_PEND_CONST_TIME      CONFIG_WAIT_QUEUE_PEND_CONST_TIME
#else
#define CONFIG_RTK_WAIT_QUEUE_PEND_CONST_TIME   0
#endif


#ifdef CONFIG_RTK_SEMC_EN
#define CONFIG_SEMC_EN             1
#else
#define CONFIG_SEMC_EN             0
#endif


#ifdef CONFIG_RTK_SEMB_EN
#define CONFIG_SEMB_EN             1
#else
#define CONFIG_SEMB_EN             0
#endif

#ifdef CONFIG_TASK_PRIORITY_SET_ENABLE
#define CONFIG_TASK_PRIORITY_SET_EN             1
#else
#define CONFIG_TASK_PRIORITY_SET_EN             0
#endif


#ifdef CONFIG_TASK_TERMINATE_ENABLE
#define CONFIG_TASK_TERMINATE_EN             1
#else
#define CONFIG_TASK_TERMINATE_EN             0
#endif


#ifdef CONFIG_TICK_DOWN_COUNTER_ENABLE
#define CONFIG_TICK_DOWN_COUNTER_EN             1
#else
#define CONFIG_TICK_DOWN_COUNTER_EN             0
#endif


#ifdef CONFIG_NEWLIB_REENT_ENABLE
#define CONFIG_NEWLIB_REENT_EN             1
#else
#define CONFIG_NEWLIB_REENT_EN             0
#endif


#ifdef CONFIG_TASK_JOIN_ENABLE
#define CONFIG_TASK_JOIN_EN             1
#else
#define CONFIG_TASK_JOIN_EN             0
#endif


// 是否使能mutex

#ifdef CONFIG_MUTEX_EN
#define CONFIG_RTK_MUTEX_EN             1
#else
#define CONFIG_RTK_MUTEX_EN             0
#endif


// 是否使能msgq组件
#ifdef CONFIG_MSGQ_EN
#define CONFIG_RTK_MSGQ_EN             1
#else
#define CONFIG_RTK_MSGQ_EN             0
#endif

// 是否使能性能统计
#ifdef CONFIG_PERFORMANCE_STATISTIC_EN
#define CONFIG_RTK_PERFORMANCE_STATISTIC_EN             1
#else
#define CONFIG_RTK_PERFORMANCE_STATISTIC_EN             0
#endif

// 是否使能用户模式支持，即使能用户和系统模式分离

#ifdef CONFIG_USER_MODE_EN
#define CONFIG_RTK_USERMODE             1
#else
#define CONFIG_RTK_USERMODE             0
#endif

// 是否允许RTK进入临界区时调用hook
#ifdef CONFIG_CRITICAL_HOOK
#define CONFIG_RTK_CRITICAL_HOOK             1
#else
#define CONFIG_RTK_CRITICAL_HOOK             0
#endif

// 是否允许RTK任务调度开始和结束时调用HOOK
#ifdef CONFIG_SCHEDULE_HOOK
#define CONFIG_RTK_SCHEDULE_HOOK             1
#else
#define CONFIG_RTK_SCHEDULE_HOOK             0
#endif


// 是否允许RTK中断ISR开始和结束时调用HOOK
#ifdef CONFIG_ISR_HOOK
#define CONFIG_RTK_ISR_HOOK             1
#else
#define CONFIG_RTK_ISR_HOOK             0
#endif

// 使能CPU空闲统计
#ifdef CONFIG_CPU_IDLE_STATISTIC
#define CONFIG_RTK_CPU_IDLE_STATISTIC             1
#else
#define CONFIG_RTK_CPU_IDLE_STATISTIC             0
#endif


#if CONFIG_NEWLIB_REENT_EN
#include <sys/reent.h>
#endif

#if CONFIG_RTK_MSGQ_EN && !CONFIG_SEMC_EN
#error if CONFIG_RTK_MSGQ_EN==1 then CONFIG_SEMC_EN must be 1 also.
#endif


// TRACE配置
#ifdef CONFIG_CFG_TRACE_EN
#define OS_CFG_TRACE_EN             1
#else
#define OS_CFG_TRACE_EN             0
#endif


#ifdef CONFIG_CFG_TRACE_CONTROL_EN
#define OS_CFG_TRACE_CONTROL_EN             1
#else
#define OS_CFG_TRACE_CONTROL_EN             0
#endif

// 是否支持jlink RTOS plugin
#ifdef CONFIG_GDB_JLINK_RTOS_PLUGIN_EN
#define CONFIG_RTK_GDB_JLINK_RTOS_PLUGIN_EN             1
#else
#define CONFIG_RTK_GDB_JLINK_RTOS_PLUGIN_EN             0
#endif

// ARMv7m专有配置
// NVIC支持的优先级位数
#ifdef CONFIG_ARMV7M_NVIC_PRIO_BITS 
#define CONFIG_RTK_ARMV7M_NVIC_PRIO_BITS      CONFIG_ARMV7M_NVIC_PRIO_BITS
#else
#define CONFIG_RTK_ARMV7M_NVIC_PRIO_BITS            4
#endif

#ifdef CONFIG_ARMV7M_CRITICAL_USE_BASEPRI 
#define CONFIG_RTK_ARMV7M_CRITICAL_USE_BASEPRI      CONFIG_ARMV7M_CRITICAL_USE_BASEPRI
#else
#define CONFIG_RTK_ARMV7M_CRITICAL_USE_BASEPRI            1
#endif

#ifdef CONFIG_ARMV7M_CRITICAL_BASEPRI_PRIO 
#define CONFIG_RTK_ARMV7M_CRITICAL_BASEPRI_PRIO      CONFIG_ARMV7M_CRITICAL_BASEPRI_PRIO
#else
#define CONFIG_RTK_ARMV7M_CRITICAL_BASEPRI_PRIO            4
#endif

#if (CONFIG_RTK_ARMV7M_NVIC_PRIO_BITS < 3) || (CONFIG_RTK_ARMV7M_NVIC_PRIO_BITS > 7)
#error "error CONFIG_RTK_ARMV7M_NVIC_PRIO_BITS value"
#endif

#if CONFIG_RTK_ARMV7M_CRITICAL_USE_BASEPRI

#if (CONFIG_RTK_ARMV7M_CRITICAL_BASEPRI_PRIO < 1) \
    || (CONFIG_RTK_ARMV7M_CRITICAL_BASEPRI_PRIO >= ((1 << CONFIG_RTK_ARMV7M_NVIC_PRIO_BITS) -3) )
#error "error CONFIG_RTK_ARMV7M_CRITICAL_BASEPRI_PRIO value"
#endif

#define CONFIG_RTK_ARMV7M_CRITICAL_BASEPRI_VAL  (CONFIG_RTK_ARMV7M_CRITICAL_BASEPRI_PRIO << (8 - CONFIG_RTK_ARMV7M_NVIC_PRIO_BITS))
#endif

// ARMv8m专有配置
// NVIC支持的优先级位数
#ifdef CONFIG_ARMV8M_NVIC_PRIO_BITS 
#define CONFIG_RTK_ARMV8M_NVIC_PRIO_BITS      CONFIG_ARMV8M_NVIC_PRIO_BITS
#else
#define CONFIG_RTK_ARMV8M_NVIC_PRIO_BITS                 3
#endif 

#ifdef CONFIG_ARMV8M_CRITICAL_USE_BASEPRI
#define CONFIG_RTK_ARMV8M_CRITICAL_USE_BASEPRI      CONFIG_ARMV8M_CRITICAL_USE_BASEPRI
#else
#define CONFIG_RTK_ARMV8M_CRITICAL_USE_BASEPRI            0
#endif

#ifdef CONFIG_ARMV8M_CRITICAL_BASEPRI_PRIO
#define CONFIG_RTK_ARMV8M_CRITICAL_BASEPRI_PRIO      CONFIG_ARMV8M_CRITICAL_BASEPRI_PRIO
#else
#define CONFIG_RTK_ARMV8M_CRITICAL_BASEPRI_PRIO           3
#endif

#if (CONFIG_RTK_ARMV8M_NVIC_PRIO_BITS < 3) || (CONFIG_RTK_ARMV8M_NVIC_PRIO_BITS > 7)
#error "error CONFIG_RTK_ARMV8M_NVIC_PRIO_BITS value"
#endif

#if CONFIG_RTK_ARMV8M_CRITICAL_USE_BASEPRI

#if (CONFIG_RTK_ARMV8M_CRITICAL_BASEPRI_PRIO < 1) \
    || (CONFIG_RTK_ARMV8M_CRITICAL_BASEPRI_PRIO >= ((1 << CONFIG_RTK_ARMV8M_NVIC_PRIO_BITS) -3) )
#error "error CONFIG_RTK_ARMV8M_CRITICAL_BASEPRI_PRIO value"
#endif

#define CONFIG_RTK_ARMV8M_CRITICAL_BASEPRI_VAL  (CONFIG_RTK_ARMV8M_CRITICAL_BASEPRI_PRIO << (8 - CONFIG_RTK_ARMV8M_NVIC_PRIO_BITS))
#endif

/** @} grp_rtkcfg */

// 定义rtk总的优先级数目，包含了idle任务
#define RTK_TOTAL_PRIORITY_COUNT \
        (CONFIG_RTK_REALTIME_PRIORITY_COUNT + \
            CONFIG_RTK_SYS_PRIORITY_COUNT + \
            CONFIG_RTK_USR_PRIORITY_COUNT + \
            CONFIG_RTK_SYS_LOW_PRIORITY_COUNT + \
            1 )


#endif  /* __RTK_CONFIG_H */
/* end of file */

