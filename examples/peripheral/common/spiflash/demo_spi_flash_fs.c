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
 * \brief ���̣�ʹ���ļ�ϵͳ��д��
 *
 * - ��������:
 *   1. aw_prj_params.h��ʹ��
 *      - AW_DEV_xxx_FLEXSPI
 *      - AW_COM_FS_LFFS
 *      - AW_CFG_FLEXSPI_FLASH_MTD
 *      - AW_COM_CONSOLE
 *      - AW_COM_MTD_DEV
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ�������ء�
 *   3. ����д����������Ҫ�ڶ�Ӧ�������ļ��д�д������
 *
 * - ʵ������:
 *   1. ���ڴ�ӡ���������Ϣ��
 *
 * \par Դ����
 * \snippet demo_spi_flash_fs.c src_spi_flash_fs
 *
 * \internal
 * \par History
 * - 1.00 17-09-05  deo, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_spi_flash_fs SPI Flash(�ļ�ϵͳ)
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

#define __DISK_NAME     DE_SPI_FLASH        /* flash���ƣ����������޸� */
#define __FS_NAME       "lffs"

#define BUF_SIZE        512                 /* ��д��������С */

aw_local uint8_t        __g_buf[BUF_SIZE];  /* ��д������ */

/**
 * \brief ʹ���ļ�ϵͳ��д SPI Flash ʾ��
 * \return ��
 */
void demo_spi_flash_fs_entry (void)
{
    aw_err_t    ret;
    int         fd, i;

    /* ��һ��ʹ�ý��и�ʽ�� */
    struct aw_fs_format_arg fmt = {"awdisk", 4 * 1024, 1};
    ret = aw_make_fs(__DISK_NAME, __FS_NAME, &fmt);
    if (ret != AW_OK) {
        aw_kprintf("SPI-Flash make LFFS failed %d.\r\n", ret);
        goto __exit;
    }
    aw_kprintf("SPI-Flash make LFFS done.\r\n");

    /* ���� ��/sflash0�� �� ��/sf�� */
    ret = aw_mount("/sf", __DISK_NAME, __FS_NAME, 0);
    if (ret != AW_OK) {
        aw_kprintf("SPI-Flash mount LFFS failed %d.\r\n", ret);
        goto __exit;
    }
    aw_kprintf("SPI-Flash mount LFFS done.\r\n");

    /* ��test.txt�ļ�������������򴴽� */
    fd = aw_open("/sf/test.txt", O_RDWR | O_CREAT | O_TRUNC, 0777);
    if (fd < 0) {
        aw_kprintf("SPI-Flash create file failed %d.\r\n", ret);
        goto __exit;
    }
    aw_kprintf("SPI-Flash create file done.\r\n");

    for (i = 0; i < BUF_SIZE; i++) {
        __g_buf[i] = (uint8_t)(i & 0xFF);
    }

    /* д������ */
    ret = aw_write(fd, __g_buf, BUF_SIZE);
    if (ret != BUF_SIZE) {
        aw_close(fd);
        aw_kprintf("SPI-Flash write file failed %d.\r\n", ret);
        goto __exit;
    }
    aw_kprintf("SPI-Flash write file done.\r\n");

    /* �ر��ļ� */
    aw_close(fd);
    aw_kprintf("SPI-Flash close file.\r\n");

    /* ���´��ļ� */
    fd = aw_open("/sf/test.txt", O_RDONLY, 0777);
    if (fd < 0) {
        aw_kprintf("SPI-Flash open file failed %d.\r\n", ret);
        goto __exit;
    }
    aw_kprintf("SPI-Flash open file done.\r\n");

    /* �������� */
    ret = aw_read(fd, __g_buf, BUF_SIZE);
    if (ret != BUF_SIZE) {
        aw_close(fd);
        aw_kprintf("SPI-Flash read file failed %d.\r\n", ret);
        goto __exit;
    }
    aw_kprintf("SPI-Flash read file done.\r\n");

    /* �ر��ļ� */
    aw_close(fd);
    aw_kprintf("SPI-Flash close file.\r\n");

    /* У���д�����Ƿ�һ�� */
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
