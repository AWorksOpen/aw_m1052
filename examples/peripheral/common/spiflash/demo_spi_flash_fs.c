/*******************************************************************************
 *                                 AWorks
 *                       ---------------------------
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
 * \brief 例程（使用文件系统读写）
 *
 * - 操作步骤:
 *   1. aw_prj_params.h中使能
 *      - AW_DEV_xxx_FLEXSPI
 *      - AW_COM_FS_LFFS
 *      - AW_CFG_FLEXSPI_FLASH_MTD
 *      - AW_COM_CONSOLE
 *      - AW_COM_MTD_DEV
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控。
 *   3. 若有写保护，还需要在对应的配置文件中打开写保护。
 *
 * - 实验现象:
 *   1. 串口打印操作结果信息。
 *
 * \par 源代码
 * \snippet demo_spi_flash_fs.c src_spi_flash_fs
 *
 * \internal
 * \par History
 * - 1.00 17-09-05  deo, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_spi_flash_fs SPI Flash(文件系统)
 * \copydoc demo_spi_flash_fs.c
 */

/** [src_spi_flash_fs] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "fs/aw_mount.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/aw_dirent.h"
#include "io/sys/aw_stat.h"
#include "aw_demo_config.h"

#define __DISK_NAME     DE_SPI_FLASH        /* flash名称，根据需求修改 */
#define __FS_NAME       "lffs"

#define BUF_SIZE        512                 /* 读写缓冲区大小 */

aw_local uint8_t        __g_buf[BUF_SIZE];  /* 读写缓冲区 */

/**
 * \brief 使用文件系统读写 SPI Flash 示例
 * \return 无
 */
void demo_spi_flash_fs_entry (void)
{
    aw_err_t    ret;
    int         fd, i;

    /* 第一次使用进行格式化 */
    struct aw_fs_format_arg fmt = {"awdisk", 4 * 1024, 1};
    ret = aw_make_fs(__DISK_NAME, __FS_NAME, &fmt);
    if (ret != AW_OK) {
        aw_kprintf("SPI-Flash make LFFS failed %d.\r\n", ret);
        goto __exit;
    }
    aw_kprintf("SPI-Flash make LFFS done.\r\n");

    /* 挂载 “/sflash0” 到 “/sf” */
    ret = aw_mount("/sf", __DISK_NAME, __FS_NAME, 0);
    if (ret != AW_OK) {
        aw_kprintf("SPI-Flash mount LFFS failed %d.\r\n", ret);
        goto __exit;
    }
    aw_kprintf("SPI-Flash mount LFFS done.\r\n");

    /* 打开test.txt文件，如果不存在则创建 */
    fd = aw_open("/sf/test.txt", O_RDWR | O_CREAT | O_TRUNC, 0777);
    if (fd < 0) {
        aw_kprintf("SPI-Flash create file failed %d.\r\n", ret);
        goto __exit;
    }
    aw_kprintf("SPI-Flash create file done.\r\n");

    for (i = 0; i < BUF_SIZE; i++) {
        __g_buf[i] = (uint8_t)(i & 0xFF);
    }

    /* 写入数据 */
    ret = aw_write(fd, __g_buf, BUF_SIZE);
    if (ret != BUF_SIZE) {
        aw_close(fd);
        aw_kprintf("SPI-Flash write file failed %d.\r\n", ret);
        goto __exit;
    }
    aw_kprintf("SPI-Flash write file done.\r\n");

    /* 关闭文件 */
    aw_close(fd);
    aw_kprintf("SPI-Flash close file.\r\n");

    /* 重新打开文件 */
    fd = aw_open("/sf/test.txt", O_RDONLY, 0777);
    if (fd < 0) {
        aw_kprintf("SPI-Flash open file failed %d.\r\n", ret);
        goto __exit;
    }
    aw_kprintf("SPI-Flash open file done.\r\n");

    /* 读回数据 */
    ret = aw_read(fd, __g_buf, BUF_SIZE);
    if (ret != BUF_SIZE) {
        aw_close(fd);
        aw_kprintf("SPI-Flash read file failed %d.\r\n", ret);
        goto __exit;
    }
    aw_kprintf("SPI-Flash read file done.\r\n");

    /* 关闭文件 */
    aw_close(fd);
    aw_kprintf("SPI-Flash close file.\r\n");

    /* 校验读写数据是否一致 */
    for (i = 0; i < BUF_SIZE; i++) {
        if (__g_buf[i] != (uint8_t)(i & 0xFF)) {
            aw_kprintf("SPI-Flash verify file data failed at %d.\r\n", i);
            goto __exit;
        }
    }

    aw_kprintf("SPI-Flash verify file data successfully.\r\n");

__exit:
    return ;
}

/** [src_spi_flash_fs] */

/* end of file */
