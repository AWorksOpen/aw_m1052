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
 * \brief segger rtt shell config
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-06  18-08-29, phd, First implementation
 * \endinternal
 */

#include "aw_shell.h"
#include "aw_task.h"

#include "aw_shell_segger_rtt.h"

#ifndef AW_CFG_SHELL_THREAD_PRIO
#if defined(AXIO_AWORKS) && defined(CONFIG_SHELL_TASK_PRIO)
#define AW_CFG_SHELL_THREAD_PRIO          CONFIG_SHELL_TASK_PRIO
#else
#define AW_CFG_SHELL_THREAD_PRIO          6u
#endif
#endif

#ifndef AW_CFG_SHELL_THREAD_STK_SIZ
#if defined(AXIO_AWORKS) && defined(CONFIG_SHELL_TASK_SIZE)
#define AW_CFG_SHELL_THREAD_STK_SIZ       CONFIG_SHELL_TASK_SIZE * 1024u
#else
#define AW_CFG_SHELL_THREAD_STK_SIZ       12 * 1024u
#endif
#endif

#ifndef AW_CFG_SHELL_BUF_SIZ
#if defined(AXIO_AWORKS) && defined(CONFIG_SHELL_BUF_SIZE)
#define AW_CFG_SHELL_BUF_SIZ              CONFIG_SHELL_BUF_SIZE
#else
#define AW_CFG_SHELL_BUF_SIZ              1024
#endif
#endif

#ifndef AW_CFG_SHELL_RTT_COMID
#if defined(AXIO_AWORKS) && defined(CONFIG_SHELL_RTT_COMID)
#define AW_CFG_SHELL_RTT_COMID              CONFIG_SHELL_RTT_COMID
#else
#define AW_CFG_SHELL_RTT_COMID              0
#endif
#endif

static struct aw_shell_segger_rtt_dev __g_shell_segger_rtt_dev;
static char                           __g_shell_segger_rtt_buffer[AW_CFG_SHELL_BUF_SIZ];
AW_TASK_DECL_STATIC(__g_shell_segger_rtt_task, AW_CFG_SHELL_THREAD_STK_SIZ);

static struct aw_shell_opt __g_shell_segger_rtt_opt = {
    .use_chart      = 1,
    .use_color      = 1,
    .use_history    = 1,
    .use_escape_seq = 1,
    .use_cmdset     = 1,
    .use_cursor     = 1,
    .use_complete   = 1,

    .p_endchar    = NULL,
    .p_desc       = "# ",
    .p_desc_color = AW_SHELL_FCOLOR_GREEN,
    .p_his_buf    = __g_shell_segger_rtt_buffer,
    .his_buf_size = sizeof(__g_shell_segger_rtt_buffer),
};

aw_err_t aw_shell_segger_rtt_cfg(void)
{
    aw_err_t ret;

    __g_shell_segger_rtt_dev.tid = AW_TASK_INIT(__g_shell_segger_rtt_task,
                                                "RTT Shell",
                                                AW_CFG_SHELL_THREAD_PRIO,
                                                AW_CFG_SHELL_THREAD_STK_SIZ,
                                                aw_shell_segger_rtt_run,
                                                &__g_shell_segger_rtt_dev);

    ret = aw_shell_segger_rtt_init(
        &__g_shell_segger_rtt_dev,
         AW_CFG_SHELL_RTT_COMID, 
         "rtt shell", 
         &__g_shell_segger_rtt_opt);

    return ret;
}
