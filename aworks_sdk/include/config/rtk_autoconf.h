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
 * \brief  rtk �ں������ļ�
 *
 * ���ļ�Ϊ platform �� config_rtk �ж� RTK ������������ɵ��ļ�
 *
 * \internal
 * \par modification history:
 * - 1.00 12-10-23  or2, first implementation
 * \endinternal
 */

#ifndef __RTK_AUTOCONFIG_H
#define __RTK_AUTOCONFIG_H

// ʵʱ���ȼ���Ŀ
#define  CONFIG_REALTIME_PRIORITY_COUNT         2
// ϵͳ���ȼ���Ŀ
#define CONFIG_SYS_PRIORITY_COUNT               8
// �û����ȼ���Ŀ
#define CONFIG_USR_PRIORITY_COUNT           20
// ϵͳ�����ȼ���Ŀ
#define CONFIG_SYS_LOW_PRIORITY_COUNT       2

#if 1
// ʹ�ܶ���Ķ�̬�����������Ϳ��Դ��������ź�������������
#define CONFIG_OBJECT_CREATE_EN           
#endif 


// �Ƿ�ʹ�ܴ���systick����
#if 0
#define CONFIG_SYSTICK_TASK_EN              0
#endif
#define CONFIG_SYSTICK_TASK_STACK_SIZE      2048

/**
 * \brief ��������ջ��С
 *
 * ����CPU��Դ�������ޣ�Ϊ�˱��ⲻ��Ҫ��RAM���ģ��û����Խ��˲�������Ϊʵ��
 * ��Ҫ�Ĵ�С, ���������IDLE_TASK_HOOK,�ر�Ҫע��ջ����̫С��
 *
 * \hideinitializer
 */
#define CONFIG_IDLE_TASK_STACK_SIZE         2048

/**
 * \brief �������ʹ��
 *
 * ʹ�ܺ��ܼ���������������-EDEADLK.
 * 1: ʹ��
 * 0: ��ֹ
 * \hideinitializer
 */
#if 1
#define CONFIG_DEAD_LOCK_DETECT_ENABLE    
#endif

/**
 * \brief ������ʾʹ��
 *
 * ��Ҫkprintf֧�֡�
 * 1: ʹ��
 * 0: ��ֹ
 * \hideinitializer
 */
#if  0
#define CONFIG_DEAD_LOCK_SHOW_EN      
#endif

#if    1
#define CONFIG_KERNEL_ARG_CHECK_EN                         1
#endif

// ʹ��wait_queue��pend����ʱ�䳣����
// ������ô�ѡ���wait_queue��pend����ʱ������Ѿ�pend�ڴ�wait_queue�ϵ����������
// ���ȼ���أ������������е�IPC��take lock����������pending��������ȼ����
// ʹ�ܴ�ѡ���ռ���ڴ�϶࣬����wait_queue��pending����ʱ�����ӱ�ΪO(1)
// ������ʱ�䣬wakeup������ʱ������ϵͳ���ȼ�������أ���ϵͳ���ȼ�����ÿ��������ȷ����
// ����wakeupʱ������Ҳ��O(1)
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



// �Ƿ�ʹ��mutex
#if   1
#define CONFIG_MUTEX_EN
#endif


// �Ƿ�ʹ��msgq���
#if 1
#define CONFIG_MSGQ_EN              
#endif

// �Ƿ�ʹ������ͳ��
#if 1
#define CONFIG_PERFORMANCE_STATISTIC_EN     
#endif


// �Ƿ�ʹ���û�ģʽ֧�֣���ʹ���û���ϵͳģʽ����
#if  0
#define CONFIG_USER_MODE_EN            
#endif

// �Ƿ�����RTK�����ٽ���ʱ����hook
#if  0
#define CONFIG_CRITICAL_HOOK       
#endif

// �Ƿ�����RTK������ȿ�ʼ�ͽ���ʱ����HOOK
#if     0
#define CONFIG_SCHEDULE_HOOK     
#endif

// �Ƿ�����RTK�ж�ISR��ʼ�ͽ���ʱ����HOOK
#if   0
#define CONFIG_ISR_HOOK           
#endif

// ʹ��CPU����ͳ��
#if   0
#define CONFIG_CPU_IDLE_STATISTIC 
#endif


// TRACE����
#if  0
#define CONFIG_CFG_TRACE_EN  
#endif
#if  0
#define CONFIG_CFG_TRACE_CONTROL_EN        
#endif

// �Ƿ�֧��jlink RTOS plugin
#if   1
#define CONFIG_GDB_JLINK_RTOS_PLUGIN_EN        
#endif

// ARMv7mר������
// NVIC֧�ֵ����ȼ�λ��
#define CONFIG_ARMV7M_NVIC_PRIO_BITS            4
#define CONFIG_ARMV7M_CRITICAL_USE_BASEPRI      1
#define CONFIG_ARMV7M_CRITICAL_BASEPRI_PRIO     4

// ARMv8mר������
// NVIC֧�ֵ����ȼ�λ��
#define CONFIG_ARMV8M_NVIC_PRIO_BITS            3
#define CONFIG_ARMV8M_CRITICAL_USE_BASEPRI      0
#define CONFIG_ARMV8M_CRITICAL_BASEPRI_PRIO     3

#endif  /* __RTK_AUTOCONFIG_H */

/* end of file */

