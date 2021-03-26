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
 * \brief 通过shell命令开关蜂鸣器
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能
 *      - AW_DEV_PWM_BUZZER
 *      - 蜂鸣器对应的PWM宏。
 *   2. 在shell界面进入".test"子目录后输入"buzzer on"蜂鸣器响起，输入"buzzer off"蜂鸣器关闭。
 *
 * \par 源代码
 * \snippet app_buzzer.c app_buzzer
 *
 * \internal
 * \par Modification History
 * - 1.00 18-07-11  sdq, first implementation.
 * \endinternal
 */

/** [src_app_buzzer] */
#include "aworks.h"                     /* 此头文件必须被首先包含 */
#include "aw_shell.h"
#include "aw_buzzer.h"
#include "app_config.h"
#include "string.h"

#ifdef APP_BUZZER
/******************************************************************************/
int app_buzzer (int argc, char **argv, struct aw_shell_user *p_user)
{
    char *p_stat = NULL;

    if (aw_shell_args_get(p_user, NULL, 1) == AW_OK) {
        p_stat = aw_shell_str_arg(p_user, 0);

        if (strcmp(p_stat, "on") == 0) {
            aw_buzzer_on();
        } else if (strcmp(p_stat, "off") == 0) {
            aw_buzzer_off();
        } else {
            return -AW_EINVAL;
        }
    }

    return AW_OK;
}

/** [src_app_buzzer] */

#endif /* APP_BUZZER */
