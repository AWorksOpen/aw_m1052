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
 * \brief 将ftl设备注册成块设备用于fat文件系统的例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - NANDFLASH外设宏
 *      - AW_COM_FTL
 *      - AW_COM_FS_FATFS
 *      - AW_COM_CONSOLE
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象:
 *   1. 首先会创建一个FTL设备，并用FatFs格式化
 *   2. 通过文件系统接口读写
 *
 * \par 源代码
 * \snippet demo_ftl_fs.c src_ftl_fs
 */

/**
 * \addtogroup demo_if_ftl_fs FTL(文件系统)
 * \copydoc demo_ftl_fs.c
 */

/** [src_ftl_fs] */

#include "aworks.h"
#include "aw_vdebug.h"
#include "string.h"
#include "fs/aw_mount.h"
#include "io/sys/aw_stat.h"
#include "aw_ftl.h"
#include "aw_delay.h"
#include "io/aw_unistd.h"
#include "io/aw_fcntl.h"

/* mtd的名字在nandflash的硬件配置(awbl_hwconf_xxx_nandflash.h)里查找 */
#define __MTD_NAME  "/dev/mtd1"

/**
 * \brief 文件读写操作
 * \return 无
 */
static void __fs_file_rw (char *p_mnt)
{
    int             i = 0;
    int             handle;
    static uint8_t  str_buf[256]    = {0};
    static char     p_file_name[64] = {0};

    strncpy(p_file_name, p_mnt, sizeof(p_file_name));
    strncat(p_file_name, "/aworks_sd_test.txt", strlen("/aworks_sd_test.txt"));

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
    if (aw_close(handle)) {
        AW_INFOF(("close file %s ok\n", p_file_name));
        return;
    }

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
    aw_close(handle);
    AW_INFOF(("file %s data check ok\n", p_file_name));
}

/**
 * \brief 任务入口
 * \param[in] p_arg : 任务参数
 * \return 无
 */
void demo_ftl_fs_entry (void)
{
    int    err;

    /*
     * 创建一个ftl设备
     * note: mtd的名字在nandflash的硬件配置(awbl_hwconf_xxx_nandflash.h)里查找
     */
    err = aw_ftl_dev_create ("/dev/ftl", __MTD_NAME);
    if (err != AW_OK) {
        AW_ERRF(("aw_ftl_dev_create err: %d\n", err));
        goto __task_entry_end;
    }

    /* 将该设备注册成块设备  */
    err = aw_ftl_blk_register("/dev/ftl");
    if (err != AW_OK) {
        AW_ERRF(("aw_ftl_blk_register err: %d\n", err));
        goto __task_entry_end;
    }

    /* 格式化存储器（格式化一次就够了），格式化后所有文件将删除，所以谨慎处理  */
#if 1
    {
        /* 卷名为"awdisk"， 卷大小为2K，
         *
         */
        struct aw_fs_format_arg fmt = {"awdisk", 2048, 0};

        /* 用 fat 文件系统格式化该块设备 */
        if ((err = aw_make_fs("/dev/ftl", "vfat", &fmt)) != AW_OK) {
            AW_ERRF(("make %s use %s failed: %d\n", "/dev/ftl", "vfat", err));
            goto  __task_entry_end;
        }
        AW_INFOF(("make %s use %s OK\n", "/dev/ftl", "vfat"));
    }
#endif

    /* 用 fat 挂载该块设备 */
    if ((err = aw_mount("/fat_mnt", "/dev/ftl", "vfat", 0)) != AW_OK) {
        AW_ERRF(("aw_mount err: %d\n", err));
        goto __task_entry_end;
    } else {
        AW_INFOF(("mount OK\n"));
    }

    /*
     * 进行文件读写操作
     */
    __fs_file_rw("/fat_mnt");

    /*
     * 卸载该挂载点
     */
    if ((err = aw_umount("/fat_mnt", 0)) == AW_OK) {
        AW_INFOF(("umount OK\n"));
    }

    /* 注销 ftl 的块设备 */
    aw_ftl_blk_unregister("/dev/ftl");

__task_entry_end:
    for (;;) {
        aw_mdelay(1000);
    }
}

/** [src_ftl_fs] */

/* end of file */
