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

int arch_interrupt_disable()
{
    RTK_CRITICAL_STATUS_DECL(old);
    
    RTK_ENTER_CRITICAL(old)
    return (int)old;
}

void arch_interrupt_enable(int old)
{
    RTK_EXIT_CRITICAL(old);
}

/* end of file */
