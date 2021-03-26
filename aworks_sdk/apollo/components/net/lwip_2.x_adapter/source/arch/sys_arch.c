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
 * \brief LWIP "OS support" emulation layer
 *
 * \internal
 * \par History
 * - 141216, afm, modify sys_thread_new function, add sys_thread_delete function.
 * - 130604, orz, Porting for anywhere platform.
 * - 130318, orz, First implementation.
 * \endinternal
 */

/******************************************************************************/
#include "lwip/sys.h"
#include "lwip/opt.h"
#include "lwip/stats.h"
#include "arch/sys_arch.h"

#include "aw_assert.h"
#include "aw_sem.h"
#include "aw_msgq.h"
#include "aw_task.h"
#include "aw_system.h"
#include "aw_delay.h"

#undef AW_VDEBUG
#undef AW_VDEBUG_DEBUG
#undef AW_VDEBUG_INFO
#undef AW_VDEBUG_WARN
#undef AW_VDEBUG_ERROR

#include "aw_vdebug.h"

#include <string.h>

/******************************************************************************/

#ifndef ARCH_MBOX_COUNT
#define ARCH_MBOX_COUNT 4
#endif

#ifndef SYS_STATS
#undef  SYS_ARCH_DECL_PROTECT
#undef  SYS_ARCH_PROTECT
#undef  SYS_ARCH_UNPROTECT
#define SYS_ARCH_DECL_PROTECT(lev)
#define SYS_ARCH_PROTECT(lev)       do { } while (0)
#define SYS_ARCH_UNPROTECT(lev)     do { } while (0)
#endif

/******************************************************************************/

/**
 * \brief Convert a millisecond value to number of RTOS ticks
 * \param ms milliseconds to convert
 * \return ticks of RTOS
 */
static int sys_ms2osticks (u32_t ms)
{
    if (ms == 0) {
        return AW_WAIT_FOREVER;
    }
    return aw_ms_to_ticks(ms);
}

/**
 * \brief System arch init
 * \param none
 * \return none
 */
void sys_init (void)
{
    AW_DBGF(("arc_sys_init()\n"));
}

u32_t sys_now(void)
{
    u32_t ticks = aw_sys_tick_get();
    return aw_ticks_to_ms(ticks);
}

/******************************************************************************/

#if !LWIP_COMPAT_MUTEX

err_t sys_mutex_new (sys_mutex_t *mutex)
{
    SYS_ARCH_DECL_PROTECT(lev);

    AW_DBGF(("%s(%p)\n", __func__, mutex));

    AW_MUTEX_INIT(mutex->lwip_mutex, AW_SEM_Q_FIFO);

    SYS_ARCH_PROTECT(lev);
    SYS_STATS_INC_USED(mutex);
    SYS_ARCH_UNPROTECT(lev);

    return (ERR_OK);
}

void sys_mutex_lock (sys_mutex_t *mutex)
{
    AW_DBGF(("%s(%p)\n", __func__, mutex));

    AW_MUTEX_LOCK(mutex->lwip_mutex, AW_WAIT_FOREVER);
}

void sys_mutex_unlock (sys_mutex_t *mutex)
{
    AW_DBGF(("%s(%p)\n", __func__, mutex));

    AW_MUTEX_UNLOCK(mutex->lwip_mutex);
}

void sys_mutex_free (sys_mutex_t *mutex)
{
    SYS_ARCH_DECL_PROTECT(lev);

    AW_DBGF(("%s(%p)\n", __func__, mutex));

    AW_MUTEX_TERMINATE(mutex->lwip_mutex);

    SYS_ARCH_PROTECT(lev);
    SYS_STATS_DEC(mutex.used);
    SYS_ARCH_UNPROTECT(lev);
}
#endif /* LWIP_COMPAT_MUTEX */

/******************************************************************************/

err_t sys_sem_new (sys_sem_t *sem, u8_t count)
{
    SYS_ARCH_DECL_PROTECT(lev);

    AW_DBGF(("%s(%p):%d\n", __func__, sem, count));

    AW_SEMC_INIT(sem->lwip_sem, count, AW_SEM_Q_FIFO);

    SYS_ARCH_PROTECT(lev);
    SYS_STATS_INC_USED(sem);
    SYS_ARCH_UNPROTECT(lev);

    return ERR_OK;
}

void sys_sem_free (sys_sem_t *sem)
{
    SYS_ARCH_DECL_PROTECT(lev);

    AW_DBGF(("%s(%p)\n", __func__, sem));

    AW_SEMC_TERMINATE(sem->lwip_sem);

    SYS_ARCH_PROTECT(lev);
    SYS_STATS_DEC(sem.used);
    SYS_ARCH_UNPROTECT(lev);
}

void sys_sem_signal (sys_sem_t *sem)
{
    AW_DBGF(("%s(%p)\n", __func__, sem));

    AW_SEMC_GIVE(sem->lwip_sem);
}

u32_t sys_arch_sem_wait (sys_sem_t *sem, u32_t timeout)

{
    u32_t ticks;

    AW_DBGF(("%s(%p):%"U32_F"\n", __func__, sem, timeout));

    ticks = aw_sys_tick_get();
    if (AW_SEMC_TAKE(sem->lwip_sem, sys_ms2osticks(timeout)) == AW_OK) {
        ticks = aw_sys_tick_get() - ticks;
        return aw_ticks_to_ms(ticks);
    }
    return SYS_ARCH_TIMEOUT;
}

/******************************************************************************/

err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
    SYS_ARCH_DECL_PROTECT(level);

    AW_DBGF(("%s(%p)\n", __func__, mbox));

    if (size > ARCH_MBOX_SIZE) {
        AW_ERRF(("sys_mbox_new: size too big\n"));
        return ERR_ARG;
    }

    AW_MSGQ_INIT(mbox->lwip_mbox, ARCH_MBOX_SIZE,
                 sizeof(void *), AW_MSGQ_Q_FIFO);

    SYS_ARCH_PROTECT(level);
    SYS_STATS_INC_USED(mbox);
    SYS_ARCH_UNPROTECT(level);

    return ERR_OK;
}

void sys_mbox_free (sys_mbox_t *mbox)
{
    SYS_ARCH_DECL_PROTECT(lev);

    AW_DBGF(("%s(%p)\n", __func__, mbox));

    AW_MSGQ_TERMINATE(mbox->lwip_mbox);

    SYS_ARCH_PROTECT(lev);
    SYS_STATS_DEC(mbox.used);
    SYS_ARCH_UNPROTECT(lev);
}

void sys_mbox_post (sys_mbox_t *mbox, void *msg)
{
    AW_DBGF(("%s(%p)\n", __func__, mbox));

/*   aw_assert(mbox_is_valid(&mbox->mb)); */

    if (AW_OK != AW_MSGQ_SEND(mbox->lwip_mbox,
                              (void *)&msg,
                              sizeof(void *),
                              AW_WAIT_FOREVER,
                              AW_MSGQ_PRI_NORMAL)) {
        AW_WARNF(("sys_mbox_post(): mbox full\n"));
    }

}

err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
    AW_DBGF(("%s(%p)\n", __func__, mbox));

/*   aw_assert(mbox_is_valid(&mbox->mb)); */

    if (AW_OK == AW_MSGQ_SEND(mbox->lwip_mbox,
                              (void *)&msg,
                              sizeof(void *),
                              AW_NO_WAIT,
                              AW_MSGQ_PRI_NORMAL)) {
        return ERR_OK;
    }

    return ERR_MEM;
}

err_t sys_mbox_trypost_fromisr(sys_mbox_t *mbox, void *msg)
{
    return sys_mbox_trypost(mbox, msg);
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
    u32_t ticks;

    AW_DBGF(("%s(%p)\n", __func__, mbox));

    ticks = aw_sys_tick_get();
    if (AW_OK != AW_MSGQ_RECEIVE(mbox->lwip_mbox,
                                 msg,
                                 sizeof(void *),
                                 sys_ms2osticks(timeout))) {
        return SYS_ARCH_TIMEOUT;
    }
    ticks = aw_sys_tick_get() - ticks;

    return aw_ticks_to_ms(ticks);
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
    AW_DBGF(("%s(%p)\n", __func__, mbox));

    if (AW_OK != AW_MSGQ_RECEIVE(mbox->lwip_mbox,
                                 msg,
                                 sizeof(void *),
                                 AW_NO_WAIT)) {
        return SYS_MBOX_EMPTY;
    }
    return 0;
}

/******************************************************************************/

sys_thread_t tcpip_task_id;
sys_thread_t ppp_task_id;
AW_TASK_DECL_STATIC(arch_tcpip_thread, TCPIP_THREAD_STACKSIZE);
AW_TASK_DECL_STATIC(arch_ppp_thread, PPP_THREAD_STACKSIZE);


/** \fixme: we only support one task here */
sys_thread_t sys_thread_new (const char     *name,
                             lwip_thread_fn  thread,
                             void           *arg,
                             int             stacksize,
                             int             prio)
{

     AW_DBGF(("sys_thread_new(%d): %s\n", prio, name));

        if (strcmp(name, TCPIP_THREAD_NAME) == 0) {
            tcpip_task_id = AW_TASK_INIT(arch_tcpip_thread, name, prio,
                                TCPIP_THREAD_STACKSIZE, thread, arg);
            AW_TASK_STARTUP(arch_tcpip_thread);
            return tcpip_task_id;
        }
#if PPP_SUPPORT
        if (strcmp(name, PPP_THREAD_NAME) == 0) {
            ppp_task_id = AW_TASK_INIT(arch_ppp_thread, name, prio,
                                PPP_THREAD_STACKSIZE, thread, arg);
            AW_TASK_STARTUP(arch_ppp_thread);
            return ppp_task_id;
        }
#endif
    return NULL;
}

void sys_thread_delete(int prio)
{
    AW_DBGF(("sys_thread_delete(%d)\r\n\r\n", prio));

    if (prio == TCPIP_THREAD_PRIO) {
        aw_task_terminate(tcpip_task_id);
        return;
    }

#if PPP_SUPPORT
    if (prio == PPP_THREAD_PRIO) {
        aw_task_terminate(ppp_task_id);
        return;
    }
#endif
}

/* end of file */


