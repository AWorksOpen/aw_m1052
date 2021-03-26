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
 * \file   aw_rtk_config.h
 * \brief  rtk 内核配置文件
 *
 * 该文件为 platform 中 config_rtk 中对 RTK 相关配置项生成的文件
 *
 * \internal
 * \par modification history:
 * - 1.00 12-10-23  or2, first implementation
 * \endinternal
 */

#ifndef __RTK_AUTOCONFIG_H
#define __RTK_AUTOCONFIG_H

// 实时优先级数目
#define  CONFIG_REALTIME_PRIORITY_COUNT         2
// 系统优先级数目
#define CONFIG_SYS_PRIORITY_COUNT               8
// 用户优先级数目
#define CONFIG_USR_PRIORITY_COUNT           20
// 系统低优先级数目
#define CONFIG_SYS_LOW_PRIORITY_COUNT       2

#if 1
// 使能对象的动态创建，这样就可以创建任务，信号量，互斥锁等
#define CONFIG_OBJECT_CREATE_EN           
#endif 


// 是否使能处理systick任务
#if 0
#define CONFIG_SYSTICK_TASK_EN              0
#endif
#define CONFIG_SYSTICK_TASK_STACK_SIZE      2048

/**
 * \brief 空闲任务栈大小
 *
 * 由于CPU资源可能有限，为了避免不必要的RAM消耗，用户可以将此参数设置为实际
 * 需要的大小, 如果定义了IDLE_TASK_HOOK,特别要注意栈不能太小。
 *
 * \hideinitializer
 */
#define CONFIG_IDLE_TASK_STACK_SIZE         2048

/**
 * \brief 死锁检查使能
 *
 * 使能后能检查出死锁，并返回-EDEADLK.
 * 1: 使能
 * 0: 禁止
 * \hideinitializer
 */
#if 1
#define CONFIG_DEAD_LOCK_DETECT_ENABLE    
#endif

/**
 * \brief 死锁显示使能
 *
 * 需要kprintf支持。
 * 1: 使能
 * 0: 禁止
 * \hideinitializer
 */
#if  0
#define CONFIG_DEAD_LOCK_SHOW_EN      
#endif

#if    1
#define CONFIG_KERNEL_ARG_CHECK_EN                         1
#endif

// 使得wait_queue的pend操作时间常量化
// 如果禁用此选项，则wait_queue的pend操作时间会与已经pend在此wait_queue上的任务个数和
// 优先级相关，进而导致所有的IPC的take lock操作都会与pending任务和优先级相关
// 使能此选项，则占用内存较多，但是wait_queue的pending操作时间因子变为O(1)
// 即常量时间，wakeup操作的时间至于系统优先级数量相关，但系统优先级数量每个配置是确定的
// 所以wakeup时间因子也是O(1)
#define CONFIG_WAIT_QUEUE_PEND_CONST_TIME   0

#if  1
#define CONFIG_RTK_SEMC_EN                
#endif
#if   1
#define CONFIG_RTK_SEMB_EN     
#endif

#if    1
#define CONFIG_TASK_PRIORITY_SET_ENABLE   
#endif 

#if  1
#define CONFIG_TASK_TERMINATE_ENABLE     
#endif 

#if 1
#define CONFIG_TICK_DOWN_COUNTER_ENABLE    
#endif

#if 0
#define CONFIG_NEWLIB_REENT_ENABLE
#endif

#if  1
#define CONFIG_TASK_JOIN_ENABLE  
#endif



// 是否使能mutex
#if   1
#define CONFIG_MUTEX_EN
#endif


// 是否使能msgq组件
#if 1
#define CONFIG_MSGQ_EN              
#endif

// 是否使能性能统计
#if 1
#define CONFIG_PERFORMANCE_STATISTIC_EN     
#endif


// 是否使能用户模式支持，即使能用户和系统模式分离
#if  0
#define CONFIG_USER_MODE_EN            
#endif

// 是否允许RTK进入临界区时调用hook
#if  0
#define CONFIG_CRITICAL_HOOK       
#endif

// 是否允许RTK任务调度开始和结束时调用HOOK
#if     0
#define CONFIG_SCHEDULE_HOOK     
#endif

// 是否允许RTK中断ISR开始和结束时调用HOOK
#if   0
#define CONFIG_ISR_HOOK           
#endif

// 使能CPU空闲统计
#if   0
#define CONFIG_CPU_IDLE_STATISTIC 
#endif


// TRACE配置
#if  0
#define CONFIG_CFG_TRACE_EN  
#endif
#if  0
#define CONFIG_CFG_TRACE_CONTROL_EN        
#endif

// 是否支持jlink RTOS plugin
#if   1
#define CONFIG_GDB_JLINK_RTOS_PLUGIN_EN        
#endif

// ARMv7m专有配置
// NVIC支持的优先级位数
#define CONFIG_ARMV7M_NVIC_PRIO_BITS            4
#define CONFIG_ARMV7M_CRITICAL_USE_BASEPRI      1
#define CONFIG_ARMV7M_CRITICAL_BASEPRI_PRIO     4

// ARMv8m专有配置
// NVIC支持的优先级位数
#define CONFIG_ARMV8M_NVIC_PRIO_BITS            3
#define CONFIG_ARMV8M_CRITICAL_USE_BASEPRI      0
#define CONFIG_ARMV8M_CRITICAL_BASEPRI_PRIO     3

#endif  /* __RTK_AUTOCONFIG_H */

/* end of file */

