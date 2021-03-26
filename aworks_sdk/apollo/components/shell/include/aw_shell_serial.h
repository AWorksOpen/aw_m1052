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
 * \brief serial shell impl
 *
 * \internal
 * \par History
 * - 1.01 16-01-07, deo, add user.
 * - 1.00 15-05-13, rnk, first implementation.
 * @endinternal
 */

#ifndef __AW_SHELL_SERIAL_H
#define __AW_SHELL_SERIAL_H

#include "aw_task.h"

#include "aw_shell_core.h"

#ifdef __cplusplus
extern "C" {
#endif

struct aw_serial_shell_dev {
    struct aw_shell_context sc;
    struct aw_shell_user    sh;
    int                     com; /* ´®¿ÚºÅ */
    aw_task_id_t            tsk;
};

int aw_serial_shell_run(int arg);
aw_err_t aw_serial_shell_init(struct aw_serial_shell_dev *dev,
                              int                         com,
                              const char                 *user,
                              struct aw_shell_opt *p_shell_opt);

#ifdef __cplusplus
}
#endif

#endif /* __AW_SHELL_SERIAL_H */

/* end of file */
