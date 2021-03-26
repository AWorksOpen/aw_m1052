/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief 标准C库函数使用例程（文件读写）
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - AW_COM_CONSOLE
 *      - AW_COM_FS_FATFS
 *      以及和调试串口对应的宏和使能SD卡的宏
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控；
 *   3. 板子插入SD卡。
 *
 * - 实验现象：
 *   1. 串口打印出创建文件、写文件、关闭文件、打开文件、读文件、文件重命名等操作信息。
 *
 *
 * \par 源代码
 * \snippet demo_libc_fileio.c src_libc_fileio
 *
 * \internal
 * \par Modification history
 * - 1.00 17-09-07  mkr, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_libc_fileio 文件系统(标准C库)
 * \copydoc demo_libc_fileio.c
 */

/** [src_libc_fileio] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "fs/aw_mount.h"
#include "fs/aw_blk_dev.h"
#include <stdio.h>
#include <string.h>

/******************************************************************************/
#define LIBC_IO_FS          "vfat"     /**< \brief 使用FAT文件系统 */
#define LIBC_IO_STORER      "/dev/sd0" /**< \brief 存储器名称 */
/******************************************************************************/
/**
 * \brief 文件系统初始化
 *
 * \return 无
 */
aw_local aw_err_t __fs_init (void)
{
    aw_err_t                ret    = AW_OK;

#if 0
    uint8_t                 agains = 0;
    struct aw_fs_format_arg fmt    = {"awdisk", 4096, 1};

    /* 用文件系统格式化存储器 */
    while ((ret = aw_make_fs(LIBC_IO_STORER, LIBC_IO_FS, &fmt)) != AW_OK) {
        if (-AW_ENODEV == ret) {
            agains++;
            aw_mdelay(1000);
        }
        if (agains > 5) {
            AW_ERRF(("failed: %d\n", ret));
            return ret;
        }
    }
    AW_INFOF(("make %s use %s OK\n", LIBC_IO_STORER, LIBC_IO_FS));
#endif

    /* 文件系统挂载 */
    ret = aw_mount("/export", LIBC_IO_STORER, LIBC_IO_FS, 0);
    if (ret != AW_OK) {
        AW_ERRF(("/export mount /dev/mtd0 error: %d!\n", ret));
        return ret;
    }
    AW_INFOF(("mount OK\n"));

    return ret;
}

/**
 * \brief libc io 入口函数
 * \return 无
 */
void demo_libc_io_entry (void)
{
    char    *p_filename  = "/export/libc_test.txt";
    char    *p_newname   = "/export/libc_new.txt";
    char    *p_str       = "Demo libc file io testing.";
    uint8_t  wr_buf[256] = {0};
    uint8_t  rd_buf[256] = {0};
    uint16_t i;
    uint16_t len;
    FILE    *fp          = NULL;
    size_t   size;
    aw_err_t ret = 0;

    AW_INFOF(("\r\nLibc IO testing...\n"));

    /* 检测SD卡块设备是否插入 */
    while (aw_blk_dev_wait_install(LIBC_IO_STORER, 10000) != AW_OK) {
        AW_ERRF(("wait sdcard insert failed, err = %d\r\n", ret));
        aw_mdelay(1000);
    }

    ret = __fs_init(); /* 文件系统初始化 */
    if (AW_OK != ret) {
        AW_ERRF(("fs init failed!\n"));
        return ;
    }

    /* 以读写方式创建文件 */
    fp = fopen(p_filename, "w+");
    if (NULL == fp) {
        AW_ERRF(("fopen(): fopen error!\n"));
        return ;
    } else {
        AW_INFOF(("fopen(): fopen success - creating new file : %s\n", p_filename));
    }

    for (i = 0; i < 256; i++) {
        wr_buf[i] = i;
    }

    /* 写入256字节数据 */
    size = fwrite(wr_buf, 1, 256, fp);
    if (256 != size) {
        AW_ERRF(("fwrite(): fwrite error!\n"));
    } else {
        AW_INFOF(("fwrite(): fwrite success - write 256 bytes data!\n"));
    }

    /* 关闭文件 */
    if (0 != fclose(fp)) {
        AW_ERRF(("\nfclose(): flose error!\n"));
    } else {
        AW_INFOF(("\nfclose(): fclose success - close file!\n"));
    }
    fp = NULL;

    /* 文件重命名操作 */
    ret = rename(p_filename, p_newname);
    if (0 != ret) {
        AW_ERRF(("rename(): rename error!\n"));
    } else {
        AW_INFOF(("rename(): rename success - rename file %s to %s\n",
                  p_filename,
                  p_newname));
    }

    /* 以只读方式打开文件 */
    fp = fopen(p_newname, "r");
    if (NULL == fp) {
        AW_ERRF(("fopen(): fopen error!\n"));
        return ;
    } else {
        AW_INFOF(("fopen(): repeat open file - using new filename!\n"));
    }

    /* 设置文件偏移值为距起始位置200的地址位置 */
    ret = fseek(fp, 200, SEEK_SET);
    if (0 != ret) {
        AW_ERRF(("fseek(): fseek error!\n"));
    } else {
        AW_INFOF(("fseek(): setting file pointer to 200!\n"));
    }

    /* 读取文件56个数据 */
    size = fread(&rd_buf[200], 1, 56, fp);
    if (56 != size) {
        AW_ERRF(("fread(): read error!\n"));
    } else {
        AW_INFOF(("fread(): read 56 bytes data!\n"));
    }

    /* 检测数据有效性 */
    for (i = 200; i < 256; i++) {
        if (wr_buf[i] != rd_buf[i]) {
            AW_ERRF(("reading data no validate!\n"));
            break;
        }
    }

    /* 关闭文件 */
    if (0 != fclose(fp)) {
        AW_ERRF(("\nfclose(): flose error!\n"));
    } else {
        AW_INFOF(("\nfclose(): repeat close file!\n"));
    }
    fp = NULL;

    /* 以附加方式打开文件 */
    fp = fopen(p_newname, "a+");
    if (NULL == fp) {
        AW_ERRF(("fopen(): fopen error!\n"));
        return ;
    } else {
        AW_INFOF(("fopen(): repeat open file!\n"));
    }

    /* 写一个字符串到文件末尾 */
    len  = strlen(p_str);
    fputs(p_str, fp);
    AW_INFOF(("fputs(): write a string to the end of file!\n%s\n", p_str));

    /*将文件指针指向文件开始*/
    rewind(fp);

    ret = fseek(fp, 256, SEEK_SET);

    /* 从文件中读取刚刚写入的字符串 */
    fgets((char *)rd_buf, len + 1, fp);
    if (0 != strcmp((const char *)rd_buf, p_str)) {
        AW_ERRF(("fgets(): fgets error!\n"));
    } else {
        AW_INFOF(("fgets(): read the string that just recently wrote!\n"
                  "the string is valid by checking!\n"));
    }

    /* 关闭文件 */
    if (0 != fclose(fp)) {
        AW_ERRF(("flose error!\n"));
    } else {
        AW_INFOF(("flose(): close file!\n"));
    }
    fp = NULL;

    return ;
}

/** [src_libc_fileio] */

/* end of file */
