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
#include "apollo.h"
#include "aw_common.h"
#include "aw_psp_errno.h"
#include "rtk.h"

/******************************************************************************/
#if defined (__CC_ARM) || defined(__ARMCOMPILER_VERSION)

#elif defined (__GNUC__)
aw_psp_error_t  *__errno(void)
{
    return rtk_task_errno_addr();
}
#endif

int *_aw_get_errno_addr(void)
{
    return rtk_task_errno_addr();
}
/* end of file */
