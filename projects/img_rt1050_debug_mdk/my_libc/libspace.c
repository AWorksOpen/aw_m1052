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
 * \brief implementation of libspace.
 *
 * \internal
 * \par modification history:
 * - 1.00 19-11-01 ral, first implementation.
 * \endinternal
 */

#include "apollo.h"
#include "rtk_task.h"

/******************************************************************************/
static char __g_perproc_libspace[96];

void * __user_perproc_libspace(void)
{
    return  __g_perproc_libspace;
}

void * __user_perthread_libspace(void)
{
    struct rtk_task *p_cur_task = rtk_task_self();
    return p_cur_task->libc_perthread_libspace;
}

/* end of file */
