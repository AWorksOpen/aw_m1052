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
 * \brief segger rtt shell
 *
 * \internal
 * \par modification history:
 * - 1.00 18-08-29, phd, First implementation
 * \endinternal
 */

#ifndef __AW_SHELL_SEGGER_RTT_H
#define __AW_SHELL_SEGGER_RTT_H

#include "aw_task.h"

#include "aw_shell_core.h"

#ifdef __cplusplus
extern "C" {
#endif

struct aw_shell_segger_rtt_dev {
    struct aw_shell_context sc;
    struct aw_shell_user    sh;
    unsigned                buff_id;
    aw_task_id_t            tid;
};

int      aw_shell_segger_rtt_run(int arg);
aw_err_t aw_shell_segger_rtt_init(struct aw_shell_segger_rtt_dev *dev,
                                  unsigned                        buff_id,
                                  const char *                    user,
                                  struct aw_shell_opt *           p_shell_opt);

#ifdef __cplusplus
}
#endif

#endif
