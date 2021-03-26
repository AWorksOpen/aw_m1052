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
 * @file
 * @brief Compiler defines for lwip.
 *
 * @internal
 * @par History
 * - 130604, orz, modify for anywhere platform.
 * - 110506, orz, First implementation.
 * @endinternal
 */

#ifndef __ARCH_SYS_ARCH_H__
#define __ARCH_SYS_ARCH_H__

#include "lwip/opt.h"
#include "arch/cc.h"
#include "lwip/arch.h"

#include "apollo.h"

#include "aw_system.h"
#include "aw_sem.h"
#include "aw_msgq.h"
#include "aw_task.h"

#define LWIP_COMPAT_MUTEX   0

#define ARCH_MBOX_SIZE  TCPIP_MBOX_SIZE
#if ARCH_MBOX_SIZE < 2
#undef ARCH_MBOX_SIZE
#define ARCH_MBOX_SIZE  2
#endif

/******************************************************************************/

/** \brief sys mailbox type */
struct sys_mbox {
    AW_MSGQ_DECL(lwip_mbox, ARCH_MBOX_SIZE, sizeof(void *));
};

/** \brief sys semaphore type */
struct sys_sem {
    AW_SEMC_DECL(lwip_sem);
};

/** \brief sys mutex type */
struct sys_mutex {
    AW_MUTEX_DECL(lwip_mutex);
};


typedef int                 sys_prot_t;
typedef struct sys_sem      sys_sem_t;
typedef struct sys_mbox     sys_mbox_t;
typedef struct sys_mutex    sys_mutex_t;
typedef aw_task_id_t        sys_thread_t;

/******************************************************************************/

#define sys_sem_valid(psem)         (1)
#define sys_sem_set_invalid(psem)   AW_SEMC_TERMINATE((psem)->lwip_sem)

#define sys_mbox_valid(pmb)         AW_MSGQ_VALID((pmb)->lwip_mbox)
#define sys_mbox_set_invalid(pmb)   AW_MSGQ_SET_INVALID((pmb)->lwip_mbox)

#define sys_mutex_valid(pm)         (1)
#define sys_mutex_set_invalid(pm)   AW_MUTEX_TERMINATE((pm)->lwip_mutex)

/******************************************************************************/

/** \brief Ticks/jiffies since power up. */
#define sys_jiffies()               aw_sys_tick_get()

void sys_thread_delete(int prio);
u32_t sys_rand32 (void);
u16_t sys_rand16 (void);

#endif /* __ARCH_SYS_ARCH_H__ */

/* end of file */
