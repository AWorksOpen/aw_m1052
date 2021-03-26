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
 * \brief default shell
 *
 * \internal
 * \par modification history:
 * - 1.00 18-08-29, phd, First implementation
 * \endinternal
 */

#include "apollo.h"

#include "aw_serial.h"

#include "aw_shell_core.h"

static aw_shell_user_t *p_default_shell = NULL;

aw_shell_user_t *aw_default_shell_get()
{
    return p_default_shell;
}

aw_shell_user_t *aw_default_shell_set(aw_shell_user_t *sh)
{
    aw_shell_user_t *old_sh = p_default_shell;
    p_default_shell = sh;
    return old_sh;
}
