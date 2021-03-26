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
 * \brief implementation of mutex
 *
 * \internal
 * \par modification history:
 * - 1.00 19-11-12 ral, first implementation.
 * \endinternal
 */

#include "apollo.h"
#include "aw_sem.h"
#include "aw_mem.h"

/******************************************************************************/
__attribute__((used))
struct rtk_mutex *_mutex_initialize(struct rtk_mutex **m)
{
    *m = aw_mem_alloc(sizeof(struct rtk_mutex));
    return AW_MUTEX_INIT(**m, AW_SEM_Q_PRIORITY);
}

__attribute__((used))
void _mutex_acquire(struct rtk_mutex **m)
{
    aw_mutex_lock(*m, AW_SEM_WAIT_FOREVER);
}

__attribute__((used))
void _mutex_release(struct rtk_mutex **m)
{
    aw_mutex_unlock(*m);
}

__attribute__((used))
void _mutex_free(struct rtk_mutex **m)
{
    aw_mutex_terminate(*m);
    aw_mem_free(*m);
}

/* end of file */