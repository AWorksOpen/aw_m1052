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
 * \brief ͨ��shell�����LED
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_DEV_GPIO_LED
 *   2. ��shell�������".test"��Ŀ¼������"led 0 on"LED��������"led 0 off"LEDϨ��
 *
 * \par Դ����
 * \snippet app_led.c app_led
 *
 * \internal
 * \par Modification History
 * - 1.00 18-07-11  sdq, first implementation.
 * \endinternal
 */

/** [src_app_led] */
#include "aworks.h"                     /* ��ͷ�ļ����뱻���Ȱ��� */
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
