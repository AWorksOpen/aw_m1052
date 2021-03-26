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
 * \brief  rtk �ں������ļ�
 *
 * �û��ڴ��ļ��ж�ģ�����ز�����������
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
 * \name ���ȼ���Ŀ����
 * @{
 */

// ʵʱ���ȼ���Ŀ
#ifdef CONFIG_REALTIME_PRIORITY_COUNT
#define CONFIG_RTK_REALTIME_PRIORITY_COUNT      CONFIG_REALTIME_PRIORITY_COUNT
#else
#define CONFIG_RTK_REALTIME_PRIORITY_COUNT      2
#endif

// ϵͳ���ȼ���Ŀ
#ifdef CONFIG_SYS_PRIORITY_COUNT
#define CONFIG_RTK_SYS_PRIORITY_COUNT      CONFIG_SYS_PRIORITY_COUNT
#else
#define CONFIG_RTK_SYS_PRIORITY_COUNT           8
#endif

// �û����ȼ���Ŀ
#ifdef CONFIG_USR_PRIORITY_COUNT
#define CONFIG_RTK_USR_PRIORITY_COUNT      CONFIG_USR_PRIORITY_COUNT
#else
#define CONFIG_RTK_USR_PRIORITY_COUNT           20
#endif

// ϵͳ�����ȼ���Ŀ
#ifdef CONFIG_SYS_LOW_PRIORITY_COUNT
#define CONFIG_RTK_SYS_LOW_PRIORITY_COUNT      CONFIG_SYS_LOW_PRIORITY_COUNT
#else
#define CONFIG_RTK_SYS_LOW_PRIORITY_COUNT       2
#endif
/** @} */

// ʹ�ܶ���Ķ�̬�����������Ϳ��Դ��������ź�������������

#ifdef CONFIG_OBJECT_CREATE_EN
#define CONFIG_RTK_OBJECT_CREATE_EN             1
#else
#define CONFIG_RTK_OBJECT_CREATE_EN             0
#endif



// �Ƿ�ʹ�ܴ���systick����

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
 * \brief ��������ջ��С
 *
 * ����CPU��Դ�������ޣ�Ϊ�˱��ⲻ��Ҫ��RAM���ģ��û����Խ��˲�������Ϊʵ��
 * ��Ҫ�Ĵ�С, ���������IDLE_TASK_HOOK,�ر�Ҫע��ջ����̫С��
 *
 * \hideinitializer
 */
 
#ifdef CONFIG_IDLE_TASK_STACK_SIZE
#define CONFIG_RTK_IDLE_TASK_STACK_SIZE         CONFIG_IDLE_TASK_STACK_SIZE
#else
#define CONFIG_RTK_IDLE_TASK_STACK_SIZE         2048
#endif
/**
 * \brief �������ʹ��
 *
 * ʹ�ܺ��ܼ���������������-EDEADLK.
 * 1: ʹ��
 * 0: ��ֹ
 * \hideinitializer
 */

#ifdef CONFIG_DEAD_LOCK_DETECT_ENABLE
#define DEAD_LOCK_DETECT_EN             1
#else
#define DEAD_LOCK_DETECT_EN             0
#endif

/**
 * \brief ������ʾʹ��
 *
 * ��Ҫkprintf֧�֡�
 * 1: ʹ��
 * 0: ��ֹ
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

// ʹ��wait_queue��pend����ʱ�䳣����
// ������ô�ѡ���wait_queue��pend����ʱ������Ѿ�pend�ڴ�wait_queue�ϵ����������
// ���ȼ���أ������������е�IPC��take lock����������pending��������ȼ����
// ʹ�ܴ�ѡ���ռ���ڴ�϶࣬����wait_queue��pending����ʱ�����ӱ�ΪO(1)
// ������ʱ�䣬wakeup������ʱ������ϵͳ���ȼ�������أ���ϵͳ���ȼ�����ÿ��������ȷ����
// ����wakeupʱ������Ҳ��O(1)

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


// �Ƿ�ʹ��mutex

#ifdef CONFIG_MUTEX_EN
#define CONFIG_RTK_MUTEX_EN             1
#else
#define CONFIG_RTK_MUTEX_EN             0
#endif


// �Ƿ�ʹ��msgq���
#ifdef CONFIG_MSGQ_EN
#define CONFIG_RTK_MSGQ_EN             1
#else
#define CONFIG_RTK_MSGQ_EN             0
#endif

// �Ƿ�ʹ������ͳ��
#ifdef CONFIG_PERFORMANCE_STATISTIC_EN
#define CONFIG_RTK_PERFORMANCE_STATISTIC_EN             1
#else
#define CONFIG_RTK_PERFORMANCE_STATISTIC_EN             0
#endif

// �Ƿ�ʹ���û�ģʽ֧�֣���ʹ���û���ϵͳģʽ����

#ifdef CONFIG_USER_MODE_EN
#define CONFIG_RTK_USERMODE             1
#else
#define CONFIG_RTK_USERMODE             0
#endif

// �Ƿ�����RTK�����ٽ���ʱ����hook
#ifdef CONFIG_CRITICAL_HOOK
#define CONFIG_RTK_CRITICAL_HOOK             1
#else
#define CONFIG_RTK_CRITICAL_HOOK             0
#endif

// �Ƿ�����RTK������ȿ�ʼ�ͽ���ʱ����HOOK
#ifdef CONFIG_SCHEDULE_HOOK
#define CONFIG_RTK_SCHEDULE_HOOK             1
#else
#define CONFIG_RTK_SCHEDULE_HOOK             0
#endif


// �Ƿ�����RTK�ж�ISR��ʼ�ͽ���ʱ����HOOK
#ifdef CONFIG_ISR_HOOK
#define CONFIG_RTK_ISR_HOOK             1
#else
#define CONFIG_RTK_ISR_HOOK             0
#endif

// ʹ��CPU����ͳ��
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


// TRACE����
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

// �Ƿ�֧��jlink RTOS plugin
#ifdef CONFIG_GDB_JLINK_RTOS_PLUGIN_EN
#define CONFIG_RTK_GDB_JLINK_RTOS_PLUGIN_EN             1
#else
#define CONFIG_RTK_GDB_JLINK_RTOS_PLUGIN_EN             0
#endif

// ARMv7mר������
// NVIC֧�ֵ����ȼ�λ��
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

// ARMv8mר������
// NVIC֧�ֵ����ȼ�λ��
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

// ����rtk�ܵ����ȼ���Ŀ��������idle����
#define RTK_TOTAL_PRIORITY_COUNT \
        (CONFIG_RTK_REALTIME_PRIORITY_COUNT + \
            CONFIG_RTK_SYS_PRIORITY_COUNT + \
            CONFIG_RTK_USR_PRIORITY_COUNT + \
            CONFIG_RTK_SYS_LOW_PRIORITY_COUNT + \
            1 )


#endif  /* __RTK_CONFIG_H */
/* end of file */

