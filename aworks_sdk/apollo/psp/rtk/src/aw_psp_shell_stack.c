/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief I/O system shell commands.
 *
 * \internal
 * \par modification history:
 * - 1.00 20-04-01 wk, first implementation.
 * \endinternal
 */

/*******************************************************************************
    includes
*******************************************************************************/
#include "apollo.h"
#include "aw_shell.h"

#include "aw_common.h"
#include "rtk_task.h"
#include "string.h"

aw_local aw_const char stack_help_info[] = {
    "Key \"stack\" to inquire all task information\n"
};

static int __stack (int argc, char *argv[], struct aw_shell_user *sh)
{
    int err, i;
    uint32_t total_size = 0;
    struct rtk_task_info info[64];

    if(argc == 1 && !strcmp(argv[0],"--help")){
        aw_shell_printf(sh,stack_help_info);
        return AW_OK;
    }

    if (argc != 0) {
    }

    err = rtk_enum_all_task_info(info, 64, NULL);
    if (err < 0) {
        aw_shell_printf(sh, "failed: %d\n", err);
        return err;
    }

    aw_shell_printf(sh, "  NO prio   tatol  max_deep    used    free  name \n\r");
    for (i=0; i < err; i++) {
        aw_shell_printf(sh, "%4d %4d %7d  %8d %7d %7d  %s\n\r", \
                            i+1,
                            info[i].prio,
                            info[i].stack_high - info[i].stack_low,
                            info[i].stack_high - info[i].stack_deep,
                            info[i].stack_high - info[i].sp,
                            info[i].sp - info[i].stack_low,
                            info[i].name);

        total_size += info[i].stack_high - info[i].stack_low;
    }
    aw_shell_printf(sh, "\ntotal stack size:%d\n\r", total_size);
    return AW_OK;
}

/******************************************************************************/
static const struct aw_shell_cmd __g_stack_shell_cmd_tbl[] = {
    {__stack,   "stack", "Inquire task information"},
};

/******************************************************************************/
void aw_stack_shell_init (void)
{
    static struct aw_shell_cmd_list cl;

    (void)AW_SHELL_REGISTER_CMDS(&cl, __g_stack_shell_cmd_tbl);
}

/* end of file */
