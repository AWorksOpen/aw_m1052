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
 * \file
 * \brief CanFestival timer��������
 *
 * \internal
 * \par modification history:
 * - 1.00 18-10-08  anu, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "aw_isr_defer.h"
#include "aw_spinlock.h"
#include "aw_time.h"
#include "aw_hwtimer.h"
#include "aw_sem.h"

#include "timers_driver.h"

#define __TIMER_INTERRUPT_FREQ_100KHZ 10000 /* 100KHZ�ж�Ƶ�� */
#define __TIMER_100US_UINT            100   /* 100us��λ */

typedef struct __timer_handle {
    TIMEVAL                  count;         /**< \brief ��ʱ������ */
    TIMEVAL                  match_count;   /**< \brief ��ʱ��ƥ����� */
    TIMEVAL                  now_count;     /**< \brief ��ǰ���� */
    
    /** \brief ��һ�ε���getElapsedTime�ļ��� */
    TIMEVAL                  last_count;    
    AW_MUTEX_DECL           (cop_mutex);    /**< \brief Э��ջ������ */
    aw_hwtimer_handle_t      p_hwtimer;     /**< \brief Ӳ����ʱ����� */
    struct aw_isr_defer_job  isr_defer_job; /**< \brief Ӳ����ʱ���ж��ӳ���ҵ */
} __timer_handle_t;

static __timer_handle_t __g_timer;

static void __timer_notify (void *p_arg)
{
    __timer_handle_t *p_timer = p_arg;

    if (p_timer == NULL) {
        return;
    }

    p_timer->count += __TIMER_100US_UINT;
    p_timer->now_count += __TIMER_100US_UINT;

    if (p_timer->count >= p_timer->match_count) {
        p_timer->count = 0;
        aw_isr_defer_job_add(&p_timer->isr_defer_job);
    }
}

static void __timer_isr_defer_job (void *p_arg)
{
    __timer_handle_t *p_timer = p_arg;
    aw_spinlock_isr_t lock = AW_SPINLOCK_ISR_UNLOCK;

    if (p_timer == NULL) {
        return;
    }

    EnterMutex();
    aw_spinlock_isr_take(&lock);
    __g_timer.last_count = __g_timer.now_count;
    aw_spinlock_isr_give(&lock);
    TimeDispatch();
    LeaveMutex();
}

/**
 * \brief CANfestival ע�ᶨʱ��
 *
 * \param[in] p_hwtimer �жϲ���
 */
void aw_canfestival_timer_register (const aw_hwtimer_handle_t *p_hwtimer)
{
    if (p_hwtimer == NULL) {
        return;
    }

    memset(&__g_timer, 0x00, sizeof(__g_timer));

    AW_MUTEX_INIT(__g_timer.cop_mutex, AW_SEM_Q_PRIORITY);

    aw_isr_defer_job_init(
        &__g_timer.isr_defer_job, __timer_isr_defer_job, &__g_timer);

    __g_timer.p_hwtimer = (aw_hwtimer_handle_t)p_hwtimer;
}

/**
 * \brief CANfestival ��ʱ���жϷ�����
 *
 * \param[in] p_arg �жϲ���
 */
void aw_canfestival_timer_irq (void *p_arg)
{
    __timer_notify(&__g_timer);
}

void EnterMutex (void)
{
    AW_MUTEX_LOCK(__g_timer.cop_mutex, AW_SEM_WAIT_FOREVER);
}

void LeaveMutex (void)
{
    AW_MUTEX_UNLOCK(__g_timer.cop_mutex);
}

void TimerInit(void)
{
}

void StopTimerLoop (TimerCallback_t exitfunction)
{
    if (__g_timer.p_hwtimer == NULL) {
        return;
    }

    EnterMutex();
    aw_hwtimer_disable(__g_timer.p_hwtimer);
    aw_isr_defer_job_del(&__g_timer.isr_defer_job);
    exitfunction(NULL, 0);
    LeaveMutex();
}

void StartTimerLoop (TimerCallback_t init_callback)
{
    if (__g_timer.p_hwtimer == NULL) {
        return;
    }

    EnterMutex();
    aw_hwtimer_enable(__g_timer.p_hwtimer, __TIMER_INTERRUPT_FREQ_100KHZ);
    /* At first, TimeDispatch will call init_callback. */
    SetAlarm(NULL, 0, init_callback, 0, 0);
    LeaveMutex();
}

void setTimer (TIMEVAL value)
{
    aw_spinlock_isr_t lock = AW_SPINLOCK_ISR_UNLOCK;

    aw_spinlock_isr_take(&lock);
    __g_timer.match_count = value;
    aw_spinlock_isr_give(&lock);
}

TIMEVAL getElapsedTime (void)
{
    TIMEVAL val;

    aw_spinlock_isr_t lock = AW_SPINLOCK_ISR_UNLOCK;

    aw_spinlock_isr_take(&lock);

    val = __g_timer.now_count > __g_timer.last_count ?
        __g_timer.now_count - __g_timer.last_count :
        __g_timer.last_count - __g_timer.now_count;

    aw_spinlock_isr_give(&lock);

    return val;
}

/* end of file */
