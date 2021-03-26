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
 * \brief ͨ��shell�����FTPD
 *
 * - �������裺
 *   1.��app_config.h�д�APP_FTPD�궨��
 *   2. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_COM_FTPD
 *      - ��̫���豸��
 *      - ��Ҫ���صĴ洢�豸��SD��
 *   3. ��SD��Ϊ��������"mount /sd0 /dev/sd0 vfat"�����ļ�ϵͳ
 *   4. ��shell�������".test"��Ŀ¼��
 *   5. ʹ��������PC������ȷ��������ͬһ������
 *   6. ����"ftpd / 21"����"ftpd / 21 AWorks 1324"
 *   7. ��PC���ļ�������������"ftp://������IP"����"ftp://172.16.18.10"
 *   8. ����5�����������"ftpd / 21"�����ֱ����PC�з��ʿ�������SD�������ݣ�����4�����������
 *      "ftpd / 21 AWorks 1324"����Ҫ�����û���������ſ��Է��ʣ��û���ΪAWorks������Ϊ1234
 *
 *   - ʵ������
 *   1. ������PC�˷��ʿ�������SD���е��ļ���
 *
 * \par Դ����
 * \snippet app_ftpd.c app_ftpd
 *
 * \internal
 * \par Modification History
 * - 1.00 18-07-13  sdq, first implementation.
 * \endinternal
 */

/** [src_app_ftpd] */
#include "aworks.h"                     /* ��ͷ�ļ����뱻���Ȱ��� */
#include "app_config.h"

#ifdef APP_FTPD
#include "aw_ftpd.h"
#include "aw_shell.h"
#include "string.h"

/******************************************************************************/
static char __g_usr[32]  = { 0 };  /* �û��� */
static char __g_pass[32] = { 0 };  /* ���� */

aw_local aw_bool_t __ftpd_check_user (const char *user_name, const char *pass_word)
{
    if (0 == strcmp(user_name, __g_usr)) {
        if (0 == strcmp(pass_word, __g_pass)) {
            return AW_TRUE;
        }
    }
    return AW_FALSE;
}

int app_ftpd (int argc, char **argv, struct aw_shell_user *p_user)
{
    static char  path[255] = {0};
    int   port   = 0;

    if (aw_shell_args_get(p_user, NULL, 2) == AW_OK) {
        strncpy(path, aw_shell_str_arg(p_user, 0), sizeof(path));
        port = aw_shell_long_arg(p_user, 1);
        aw_ftpd_init((const char *)path, port, 1, NULL);
    } else if (aw_shell_args_get(p_user, NULL, 4) == AW_OK) {
        strncpy(path, aw_shell_str_arg(p_user, 0), sizeof(path));
        port = aw_shell_long_arg(p_user, 1);
        strncpy(__g_usr, aw_shell_str_arg(p_user, 2), sizeof(__g_usr));
        strncpy(__g_pass, aw_shell_str_arg(p_user, 3), sizeof(__g_pass));
        aw_ftpd_init((const char *)path, port, 0, __ftpd_check_user);
    }

    return AW_OK;
}

/** [src_app_ftpd] */

#endif /* APP_FTPD */
