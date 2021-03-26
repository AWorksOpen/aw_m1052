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
 * \brief ͨ��shell�������SD��
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ����Ҫ���ԵĴ洢�����ļ�ϵͳ��
 *   2. ��SD��Ϊ������˳��ִ���������
 *      1) "mount /sd0 /dev/sd0 vfat" ���ء�
 *      2) "cd sd0" ����sd0Ŀ¼
 *      3) ".test"  ����.test������Ŀ¼
 *      4) "echo "How are you" > test.txt" ���ַ���"How are you"д��test.txt��
 *      5) "cat test.txt" ��test.txt�е�������ʾ����
 *      ע�⣺��4�����д����ļ���������ᴴ�����ļ������д����ļ�������Ḳ�ǵ�Դ�ļ���ʹ��">>"��ʾ
 *           ������׷�ӵ��ļ����
 *
 * \par Դ����
 * \snippet app_disk.c app_disk
 *
 * \internal
 * \par Modification History
 * - 1.00 18-07-11  sdq, first implementation.
 * \endinternal
 */

/** [src_app_disk] */
#include "aworks.h"                     /* ��ͷ�ļ����뱻���Ȱ��� */
#include "aw_shell.h"
#include "aw_shell_core.h"
#include "aw_vdebug.h"
#include "app_config.h"

#include "fs/aw_mount.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"

#include "string.h"

#ifdef APP_DISK

const char *__g_fs[] ={
        "vfat",
        "lffs",
};




/******************************************************************************/
int app_cat (int argc, char **argv, struct aw_shell_user *p_user)
{
    char        *file_name = NULL;
    int          fd = 0;
    char         buf[255] = { 0 };
    int          size;

    if (aw_shell_args_get(p_user, NULL, 1) == AW_OK) {

        /* ��ȡ�ļ��� */
        file_name = aw_shell_str_arg(p_user, 0);
        if (file_name == NULL) {
            return -AW_EINVAL;
        }

        /* ��ȡ�ļ����ݲ������shell������ */
        fd = aw_open(file_name, O_RDONLY, 0777);
        if (fd > 0) {
            do {
                size = aw_read(fd, buf, sizeof(buf));
                if (size > 0) {
                    p_user->io_puts(p_user, buf);
                }
            } while (0 < size);
            aw_close(fd);
        }
    }

    return AW_OK;
}

int app_echo (int argc, char **argv, struct aw_shell_user *p_user)
{
    char        *p_file_name = NULL;
    char        *p_cmd = NULL;
    char        *p_buf = NULL;
    int          fd = 0;

    if (aw_shell_args_get(p_user, NULL, 3) == AW_OK) {
        /* ��ȡ�ļ��� */
        p_file_name = aw_shell_str_arg(p_user, 2);
        if (p_file_name == NULL) {
            return -AW_EINVAL;
        }

        /* �ж��Ǹ��ǻ���׷�� */
        p_cmd = aw_shell_str_arg(p_user, 1);
        if (!strcmp(p_cmd, ">")) {
            fd = aw_open(p_file_name, O_CREAT | O_WRONLY, 0777);
        } else if (!strcmp(p_cmd, ">>")) {
            fd = aw_open(p_file_name, O_CREAT | O_WRONLY | O_APPEND, 0777);
        } else {
            return -AW_EINVAL;
        }

        /* ��ȡҪд������� */
        if (fd > 0) {
            p_buf = aw_shell_str_arg(p_user, 0);
            aw_write(fd, p_buf, strlen(p_buf));
            aw_close(fd);
        }
    }

    return AW_OK;
}

/** [src_app_disk] */
#endif /* APP_DISK */
