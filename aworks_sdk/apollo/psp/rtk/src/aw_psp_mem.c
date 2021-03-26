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
 * \brief implementation of _errno().
 *
 * \internal
 * \par modification history:
 * - 1.00 14-10-29  may, first implementation.
 * \endinternal
 */

#include "aw_common.h"
#include "aw_mem.h"
#include "aw_psp_errno.h"
#include "rtk.h"
#include "aw_task.h"


/******************************************************************************/

void rtk_sys_mem_free(void *ptr)
{
    return aw_mem_free(ptr);
}

void rtk_sys_mem_alloc(void *ptr)
{
    return aw_mem_alloc(ptr);
}


/* end of file */
