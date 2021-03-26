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
 * \brief lua 演示例程（SD卡）
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *    - AW_COM_SDCARD设备
 *    - 对应平台的SD卡宏
 *    - AW_COM_FS_FATFS
 *    - AW_COM_SHELL_LUA
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *   3. 准备一张SD卡，插入开发板的SD卡槽中
 *   4. 在shell命令行执行"lua /sd/test.lua"命令。
 *
 * - 实验现象：
 *   1. 执行shell命令后，串口打印Hello world!
 *
 * - 备注：
 *   暂时只支持eclipse版本，因为lua源码依赖g++编译选项，而当前版本armcc编译器无g++编译器
 *   默认不格式化SD卡，如需格式化SD卡，请将屏蔽段代码打开
 *
 * \par 源代码
 * \snippet demo_lua.c src_demo_lua
 *
 * \internal
 * \par Modification History
 * - 1.00 19-04-25  vih, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_lua
 * \copydoc demo_lua.c
 */

/** [src_demo_lua] */

#include "aworks.h"
#include "aw_vdebug.h"
#include "fs/aw_mount.h"
#include "aw_delay.h"
#include <string.h>
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "aw_demo_config.h"
#include "fs/aw_fs_type.h"
#include "fs/aw_blk_dev.h"

#define __MOUNT_NAME        "/sd"
#define __FAT_NAME          "vfat"
#define __DISK_NAME         DE_SD_DEV_NAME

#define __LUA               "lua"
#define __FILE_NAME         "/sd/test.lua"

#define __MAKE_FS           1

/**
 * \brief 启动lua脚本  demo
 * \return 无
 */
void demo_lua_entry (void)
{
#if __MAKE_FS
    struct aw_fs_format_arg fmt = {"awdisk", 512, 0};
#endif
    int err;


    /* 检测SD卡块设备是否插入 */
    while (aw_blk_dev_wait_install(__DISK_NAME, 10000) != AW_OK) {
        AW_ERRF(("sdcard is not exist\r\n"));
        aw_mdelay(1000);
    }

    /*
     * 在指定的设备上格式化  vfat文件系统；
     * note: 若sd卡没有被格式化，那么需要格式化；若格式化过，就不需要此操作
     */
#if __MAKE_FS
    /* 制作文件系统 ，将存储器名为 "/dev/sd0"制作为"vfat"类型的文件系统  */
    if ((err = aw_make_fs(__DISK_NAME, __FAT_NAME, &fmt)) != AW_OK) {
        AW_INFOF(("failed: %d\n", err));
    } else {
        AW_INFOF(("OK\n"));
    }
#endif

    /*
     * 将指定的sd卡设备挂载到/sd，文件系统类型为vfat；
     * 挂载成功之后才可以进行正常的文件系统读写了
     */
    int fd;
    if ((err = aw_mount(__MOUNT_NAME, __DISK_NAME, __FAT_NAME, AW_OK)) == AW_OK) {

        AW_INFOF(("mount OK\n"));

        fd = aw_open (__FILE_NAME,O_RDWR | O_CREAT, 0777);
        if (fd > 0) {
            const char * strbuf = "print(\"Hello world!\")";
            ssize_t len = aw_write (fd, strbuf, strlen(strbuf));
            if (len == strlen(strbuf)) {
                aw_kprintf("write ok.\r\n");
            }
            aw_close(fd);
        }
    }

    return ;
}

/** [src_demo_lua] */

/* end of file */
