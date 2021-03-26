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
 * \brief SDCARD演示例程（文件系统）
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - AW_COM_SDCARD
 *      - 对应平台的SD宏（如：AW_DEV_xxx_USDHCx）
 *      - AW_COM_FS_FATFS
 *      - AW_COM_CONSOLE
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *   3. 在开发板SD卡槽中插入SD卡.
 *
 * - 实验现象：
 *   1. 串口打印调试信息。
 *
 * \par 源代码
 * \snippet demo_sdcard_fs.c src_sdcard_fs
 *
 * \internal
 * \par Modification History
 * - 1.00 16-03-25  cod, first implementation.
 * - 1.01 17-08-31  xdn, modify some demo code.
 * \endinternal
 */

/**
 * \addtogroup demo_if_sdcard_fs SDCARD(文件系统)
 * \copydoc demo_sdcard_fs.c
 */

/** [src_sdcard_fs] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "fs/aw_mount.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/sys/aw_stat.h"
#include "aw_sdcard.h"
#include "fs/aw_blk_dev.h"
#include "aw_demo_config.h"

#define __BLK_NAME      DE_SD_DEV_NAME

aw_local void __disk_evt_cb (const char *name, int event, void *p_arg)
{
    /* 通过 SDcard 的块设备名字判断是否为 SDcard 插拔 */
    if (strcmp(name, __BLK_NAME)) {
        return;
    }

    if (event == AW_BD_EVT_INJECT) {
        AW_INFOF(("sdcard insert...\r\n"));
    }

    if (event == AW_BD_EVT_EJECT) {
        AW_INFOF(("sdcard remove...\r\n"));
    }
}

aw_local void __fs_file_rw (void)
{
    int         i = 0;
    int         handle;
    char       *p_file_name = "/sd/aworks_sd_test.txt";
    uint8_t     str_buf[256] = {0};

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

    AW_INFOF(("file %s data check ok\n", p_file_name));
}

/**
 * \brief 使用文件系统读写SD卡 demo
 * \return 无
 *
 */
void demo_sdcard_fs_entry (void)
{
    aw_err_t    ret;
    aw_bool_t   regist_ok   = AW_TRUE;
    int         event_index = 0;

    /*
     * 添加块设备插拔事件检测, 当有SDcard插拔时, 会回调__disk_evt_cb
     */
    ret = aw_blk_dev_event_register(__disk_evt_cb, NULL, &event_index);
    if (ret != AW_OK) {
        AW_ERRF(("block event register error: %d!\n", ret));
        regist_ok = AW_FALSE;
    }

    /* 检测SD卡块设备是否插入 */
    ret = aw_blk_dev_wait_install(__BLK_NAME, 10000);
    if (ret != AW_OK) {
        AW_ERRF(("wait sdcard insert failed, err = %d\r\n", ret));
        goto __sd_fs_end;
    }


#if 0  /* 格式化一次即可  */

    /* 卷名为"awdisk"， 卷大小为4k  */
    struct aw_fs_format_arg fmt = {"awdisk", 1024 * 4, 0};
    /* 制作文件系统 ，将存储器名为 "/dev/sd0"制作为"vfat"类型的文件系统  */
    ret = aw_make_fs(__BLK_NAME, "vfat", &fmt);
    if (ret != AW_OK) {
        AW_ERRF(("failed: %d\n", ret));
        goto __sd_fs_end;
    }
    AW_INFOF(("make fs OK\n"));

#endif

    /* 文件系统挂载到"/sd"结点  */
    ret = aw_mount("/sd", __BLK_NAME, "vfat", 0);
    if (ret != AW_OK) {
        AW_ERRF(("/sd mount FATFS error: %d!\n", ret));
        goto __sd_fs_end;
    }
    AW_INFOF(("mount OK\n"));

    /* SD 卡读写测试  */
    __fs_file_rw();

__sd_fs_end:
    /* 注销事件 */
    if (regist_ok) {
        ret = aw_blk_dev_event_unregister(event_index);
        if (ret != AW_OK) {
            AW_ERRF(("block event unregister error: %d!\n", ret));
        }
    }

    return ;
}

/** [src_sdcard_fs] */

/* end of file */
