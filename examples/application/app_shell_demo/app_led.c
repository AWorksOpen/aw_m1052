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
 * \brief 通过shell命令开关LED
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能
 *      - AW_DEV_GPIO_LED
 *   2. 在shell界面进入".test"子目录后输入"led 0 on"LED亮起，输入"led 0 off"LED熄灭。
 *
 * \par 源代码
 * \snippet app_led.c app_led
 *
 * \internal
 * \par Modification History
 * - 1.00 18-07-11  sdq, first implementation.
 * \endinternal
 */

/** [src_app_led] */
#include "aworks.h"                     /* 此头文件必须被首先包含 */
#include "aw_shell.h"
#include "aw_led.h"
#include "app_config.h"
#include "string.h"

#ifdef APP_LED
/******************************************************************************/
int app_led (int argc, char **argv, struct aw_shell_user *p_user)
{
    int id;

    if (aw_shell_args_get(p_user, NULL, 2) == AW_OK) {
        id = aw_shell_long_arg(p_user, 0);
        if (strcmp(aw_shell_str_arg(p_user, 1), "on") == 0) {
            aw_led_on(id);
        } else if (strcmp(aw_shell_str_arg(p_user, 1), "off") == 0) {
            aw_led_off(id);
        } else {
            return -AW_EINVAL;
        }
    } else {
        return -AW_EINVAL;
    }

    return AW_OK;
}


/** [src_app_led] */
#endif /* APP_LED */
