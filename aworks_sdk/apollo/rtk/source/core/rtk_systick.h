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
 * \file  rtk_systick.h
 * \brief 定义rtk systick的内部操作函数
 *
 * \internal
 * \par modification history:
 * - 1.00 18-07-04  sni, first implement
 * \endinternal
 */


#ifndef             __RTK_SYSTICK_H__
#define             __RTK_SYSTICK_H__

void __rtk_systick_init(void);

#if CONFIG_RTK_SYSTICK_TASK_EN

#define RTK_SYSTICK_LOCK_DECL(lock)     void *lock;


void * __rtk_systick_lock(void);
void __rtk_systick_unlock(void *p_lock);

#define RTK_SYSTICK_LOCK(lock)  rtk_barrier();(lock) = \
                                    __rtk_systick_lock();rtk_barrier();
#define RTK_SYSTICK_UNLOCK(lock)  rtk_barrier();\
                                    __rtk_systick_unlock( (lock) );rtk_barrier();

#else

#define RTK_SYSTICK_LOCK_DECL(lock)     RTK_CRITICAL_STATUS_DECL(lock);
#define RTK_SYSTICK_LOCK(lock)          RTK_ENTER_CRITICAL(lock);
#define RTK_SYSTICK_UNLOCK(lock)        RTK_EXIT_CRITICAL( (lock) );

#endif



extern volatile unsigned long           g_systick;

#endif          /* __RTK_SYSTICK_H__*/
