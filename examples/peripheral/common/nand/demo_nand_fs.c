/*******************************************************************************
*                                 AWorks
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
 * \brief nandflash 使用文件系统读写演示例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - NANDFLASH设备宏
 *      - AW_COM_FS_LFFS
 *      - AW_COM_CONSOLE
 *      - AW_COM_SHELL_SERIAL
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 首先使用文件系统格式化 nand
 *   2. 将 nand 挂载至文件系统中
 *   3. 通过文件系统接口去读写 nand
 *   4. 通过"ls"命令,可以看到mnt目录，通过"cd mnt"命令切换到/mnt路径下，"ls"查看新建的文件"aworks_nand_test.txt"
 *
 * \par 源代码
 * \snippet demo_nand_fs.c src_demo_nand_fs
 *
 * \internal
 * \par Modification History
 * - 1.00 17-09-05  vih, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_nand_fs nandflash(文件系统)
 * \copydoc demo_nand_fs.c
 */

/** [src_demo_nand_fs] */

#include "aworks.h"
#include "aw_vdebug.h"
#include "fs/aw_mount.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/sys/aw_stat.h"
#include "aw_delay.h"
#include "aw_demo_config.h"
#include <string.h>

#define __FS_NAME        DE_NAMD_FS_NAME
#define __DISK_NAME      DE_DISK_NAME

/**
 * \brief 文件读写操作
 * \return 无
 */
static void __fs_file_rw (char  *p_mnt)
{
    int         i = 0;
    int         handle;
    uint8_t     str_buf[256] = {0};
    char        p_file_name[64] = {0};

    strncpy(p_file_name, p_mnt, sizeof(p_file_name));
    strncat(p_file_name, "/aworks_nand_test.txt", strlen("/aworks_nand_test.txt"));

    /*
     * 写文件测试（包括创建，文件写操作，关闭操作）
     */
    /* 创建新文件 */
    handle = aw_open(p_file_name, O_RDWR | O_CREAT, 0777);
    if (handle < 0) {
        AW_ERRF(("creat file error: %d\n", handle));
        return;
    }
    AW_INFOF(("creat file %s ok\n", p_file_name));

    for (i = 0; i < sizeof(str_buf); i++) {
        str_buf[i] = (uint8_t)i;
    }

    /* 写文件 */
    if (aw_write(handle, str_buf, sizeof(str_buf)) != sizeof(str_buf)) {
        aw_close(handle);
        AW_ERRF(("write file error\n"));
        return;
    }
    AW_INFOF(("write file %s ok\n", p_file_name));

    /* 关闭文件 */
    aw_close(handle);
    AW_INFOF(("close file %s ok\n", p_file_name));

    /*
     * 读文件测试（包括打开，文件读操作，关闭操作）
     */
    /* 打开文件 */
    handle = aw_open(p_file_name, O_RDONLY, 0777);
    if (handle < 0) {
        AW_ERRF(("open file error: %d\n", handle));
        return;
    }
    AW_INFOF(("open file %s ok\n", p_file_name));

    memset(str_buf, 0, sizeof(str_buf));

    /* 读取文件 */
    if (aw_read(handle, str_buf, sizeof(str_buf)) != sizeof(str_buf)) {
        aw_close(handle);
        AW_ERRF(("read file error!\n"));
        return;
    }
    AW_INFOF(("read file %s ok\n", p_file_name));

    /* 检验数据是否正确 */
    for (i = 0; i < sizeof(str_buf); i++) {
        if ((uint8_t)i != str_buf[i]) {
            AW_ERRF(("file data error!\n"));
            aw_close(handle);
            return;
        }
    }

    /* 关闭文件 */
    aw_close(handle);
    AW_INFOF(("file %s data check ok\n", p_file_name));
}

/**
 * \brief nand 使用 fs 操作 demo
 * \return 无
 */
void demo_nand_fs_entry (void)
{
    struct aw_fs_format_arg fmt = {"awdisk", 512, 0};
    int     err;

    /*
     * 在指定的mtd设备上格式化 lffs 文件系统；
     * note: mtd的名字(DE_DISK_NAME)在 nandflash 的硬件配置(awbl_hwconf_xxxxxx_nandflash.h)
     *       里查找
     * note: 若nand没有被格式化，那么需要格式化；若格式化过，就不需要此操作
     */
    if ((err = aw_make_fs(__DISK_NAME, __FS_NAME, &fmt)) != AW_OK) {
        AW_INFOF(("failed: %d\n", err));
    } else {
        AW_INFOF(("OK\n"));
    }

    /*
     * 将指定的mtd设备挂载到/mnt，文件系统类型为lffs；
     * 挂载成功之后才可以进行正常的文件系统读写了
     */
    if ((err = aw_mount("/mnt", __DISK_NAME, __FS_NAME, AW_OK)) == AW_OK) {
        AW_INFOF(("OK\n"));
    }

    /*
     * 进行文件读写操作
     */
    __fs_file_rw("/mnt");

#if 0
    /*卸载文件系统*/
    if ((err = aw_umount("/mnt",AW_OK)) == AW_OK) {
        AW_INFOF(("OK\n"));
    }
#endif

}

/** [src_demo_nand_fs] */

/* end of file */
