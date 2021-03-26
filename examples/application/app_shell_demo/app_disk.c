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
 * \brief 通过shell命令挂载SD卡
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能需要测试的存储器和文件系统。
 *   2. 以SD卡为例，按顺序执行以下命令：
 *      1) "mount /sd0 /dev/sd0 vfat" 挂载。
 *      2) "cd sd0" 进入sd0目录
 *      3) ".test"  进入.test命令子目录
 *      4) "echo "How are you" > test.txt" 将字符串"How are you"写入test.txt中
 *      5) "cat test.txt" 将test.txt中的内容显示出来
 *      注意：第4步如果写入的文件不存在则会创建新文件，如果写入的文件存在则会覆盖掉源文件；使用">>"表示
 *           将内容追加到文件最后。
 *
 * \par 源代码
 * \snippet app_disk.c app_disk
 *
 * \internal
 * \par Modification History
 * - 1.00 18-07-11  sdq, first implementation.
 * \endinternal
 */

/** [src_app_disk] */
#include "aworks.h"                     /* 此头文件必须被首先包含 */
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

        /* 获取文件名 */
        file_name = aw_shell_str_arg(p_user, 0);
        if (file_name == NULL) {
            return -AW_EINVAL;
        }

        /* 读取文件内容并输出到shell窗口中 */
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
        /* 获取文件名 */
        p_file_name = aw_shell_str_arg(p_user, 2);
        if (p_file_name == NULL) {
            return -AW_EINVAL;
        }

        /* 判断是覆盖还是追加 */
        p_cmd = aw_shell_str_arg(p_user, 1);
        if (!strcmp(p_cmd, ">")) {
            fd = aw_open(p_file_name, O_CREAT | O_WRONLY, 0777);
        } else if (!strcmp(p_cmd, ">>")) {
            fd = aw_open(p_file_name, O_CREAT | O_WRONLY | O_APPEND, 0777);
        } else {
            return -AW_EINVAL;
        }

        /* 获取要写入的内容 */
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
