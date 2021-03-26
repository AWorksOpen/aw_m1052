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
 * \brief 通过shell命令开启FTPD
 *
 * - 操作步骤：
 *   1.在app_config.h中打开APP_FTPD宏定义
 *   2. 需要在aw_prj_params.h头文件里使能
 *      - AW_COM_FTPD
 *      - 以太网设备宏
 *      - 需要挂载的存储设备如SD卡
 *   3. 以SD卡为例，输入"mount /sd0 /dev/sd0 vfat"挂载文件系统
 *   4. 在shell界面进入".test"子目录后
 *   5. 使用网线与PC相连，确保二者在同一网段下
 *   6. 输入"ftpd / 21"或者"ftpd / 21 AWorks 1324"
 *   7. 在PC的文件管理器中输入"ftp://开发板IP"，如"ftp://172.16.18.10"
 *   8. 若第5步中输入的是"ftpd / 21"则可以直接在PC中访问开发板中SD卡的内容，若第4步中输入的是
 *      "ftpd / 21 AWorks 1324"则需要输入用户名和密码才可以访问，用户名为AWorks，密码为1234
 *
 *   - 实验现象：
 *   1. 可以在PC端访问开发板中SD卡中的文件。
 *
 * \par 源代码
 * \snippet app_ftpd.c app_ftpd
 *
 * \internal
 * \par Modification History
 * - 1.00 18-07-13  sdq, first implementation.
 * \endinternal
 */

/** [src_app_ftpd] */
#include "aworks.h"                     /* 此头文件必须被首先包含 */
#include "app_config.h"

#ifdef APP_FTPD
#include "aw_ftpd.h"
#include "aw_shell.h"
#include "string.h"

/******************************************************************************/
static char __g_usr[32]  = { 0 };  /* 用户名 */
static char __g_pass[32] = { 0 };  /* 密码 */

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
