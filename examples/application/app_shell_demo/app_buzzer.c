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
 * \brief ͨ��shell����ط�����
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_DEV_PWM_BUZZER
 *      - ��������Ӧ��PWM�ꡣ
 *   2. ��shell�������".test"��Ŀ¼������"buzzer on"��������������"buzzer off"�������رա�
 *
 * \par Դ����
 * \snippet app_buzzer.c app_buzzer
 *
 * \internal
 * \par Modification History
 * - 1.00 18-07-11  sdq, first implementation.
 * \endinternal
 */

/** [src_app_buzzer] */
#include "aworks.h"                     /* ��ͷ�ļ����뱻���Ȱ��� */
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
