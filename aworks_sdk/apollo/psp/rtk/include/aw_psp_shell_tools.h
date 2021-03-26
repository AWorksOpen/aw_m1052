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
 * \brief rtk配置工具

 *
 * \par 源代码
 * \snippet aw_psp_shell_cpuload.c aw_psp_shell_stack.c
 *
 * \internal
 * \par Modification history
 * - 1.00 20-04-07  wk, first implementation.
 * \endinternal
 */

/** [src_aw_rtk_tools_cfg] */

#include "aworks.h"
#include "aw_shell.h"

#define AW_CFG_RTK_TOOLS_STACK
#if CONFIG_RTK_PERFORMANCE_STATISTIC_EN
#define AW_CFG_RTK_TOOLS_CPULOAD
#endif

void aw_rtk_tools_register (void)
{
#ifdef AW_CFG_RTK_TOOLS_STACK
    {
        aw_import void  aw_stack_shell_init(void);
        aw_stack_shell_init();
    }
#endif

#ifdef AW_CFG_RTK_TOOLS_CPULOAD
    {
        aw_import void cpuload_shell_reg (void);
        cpuload_shell_reg();
    }
#endif
}

/** [src_aw_rtk_tools_cfg] */

/* end of file */
