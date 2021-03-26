/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/


/**
 * \file
 * \brief 通过shell命令重启开发板
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能
 *      - WDT设备宏(看门狗)
 *   2. 将程序下载到flash里面，拔掉开发板WDT上的条线帽，重新启动后，按第3步操作
 *   3. 在shell界面进入".test"子目录后输入"sys_rst"。
 *
 * \par 源代码
 * \snippet app_system_restart.c app_system_restart
 *
 * \internal
 * \par Modification History
 * - 1.00 18-07-11  sdq, first implementation.
 * \endinternal
 */

#include "aworks.h"
#include "aw_wdt.h"
#include "aw_shell.h"
#include "aw_delay.h"
#include "app_config.h"
#include "aw_vdebug.h"

#ifdef APP_SYS_RST


int app_system_restart  (int argc, char **argv, struct aw_shell_user *p_user)
{
    struct awbl_wdt wdt_dev;
    aw_err_t  ret = AW_OK;

    /* 设置看门狗时间 */
    ret = aw_wdt_add(&wdt_dev, 1);
    if (ret != AW_OK) {
        aw_kprintf("*ERR: WDT add error %d\n", ret);
        return -AW_EINVAL;
    }


    /* LED0 间隔闪烁 */
    while (1) {
        aw_mdelay(100);
    }

    return AW_OK;
}


#endif
