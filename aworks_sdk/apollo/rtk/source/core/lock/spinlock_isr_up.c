/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http:/*www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief ISR可调用自旋锁的单处理器(UP)实现
 *
 * \internal
 * \par modification history:
 * - 1.00 12-08-27  zen, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "apollo.h"
#include "aw_int.h"
#include "aw_spinlock.h"

/*******************************************************************************
  implementation
*******************************************************************************/

/******************************************************************************/
void aw_spinlock_isr_init (aw_spinlock_isr_t *p_lock, int flags)
{
    p_lock->lock.owner = __SPIN_LOCK_NOBODY;
    p_lock->flags      = flags;
    p_lock->lock.key = 0;
}

/******************************************************************************/
void aw_spinlock_isr_take(aw_spinlock_isr_t *p_lock)
{
    AW_INT_CPU_LOCK_DECL(key);

    AW_INT_CPU_LOCK(key);

    if ( p_lock->lock.owner != __SPIN_LOCK_NOBODY) {
        while(1);
    }
    p_lock->lock.key = key;
    p_lock->lock.owner = 0;
}

/******************************************************************************/
void aw_spinlock_isr_give (aw_spinlock_isr_t *p_lock)
{
    AW_INT_CPU_LOCK_DECL(key);


    key = p_lock->lock.key;
    p_lock->lock.owner = __SPIN_LOCK_NOBODY;
    p_lock->lock.key = 0;

    AW_INT_CPU_UNLOCK(key);
}

/* end of file */
