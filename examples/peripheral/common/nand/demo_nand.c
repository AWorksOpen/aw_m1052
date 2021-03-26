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
 * \brief nand 裸写例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - NANDFLASH设备宏
 *      - AW_COM_CONSOLE
 *      - AW_COM_SHELL_SERIAL
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 将向 nand 指定第0块的1,2页写入数据，并且串口将打印出读写数据的操作信息。
 *
 * \par 源代码
 * \snippet demo_nand.c src_nand
 */

/**
 * \addtogroup demo_if_nand nandflash(裸读裸写)
 * \copydoc demo_nand.c
 */

/** [src_nand] */

#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "awbl_nand.h"
#include <string.h>
#include "aw_demo_config.h"

#define  NAND_FLASH_DEV_NAME    DE_NANDFLASH_DEV_NAME
#define  NAND_FLASH_DEV_ID      DE_NAND_FLASH_DEV_ID

/* 查找nand设备 */
static struct awbl_nand_chip *__nand_dev_find (const char *name, int unit)
{
    return (struct awbl_nand_chip *)AW_CONTAINER_OF(awbl_dev_find_by_name(name, unit),
            struct awbl_nand_chip,
            awdev);
}

/**
 * \brief nand 裸写例程
 * \return 无
 */
void demo_nand_entry (void)
{
    struct awbl_nand_chip *p_chip = NULL;

    int         dat;
    aw_err_t    ret;
    uint8_t     buf[2048];
    char        oob[28];
    int         i = 0;

    /* 通过nandflash设备驱动名获取nand设备 */
    p_chip = __nand_dev_find(NAND_FLASH_DEV_NAME, NAND_FLASH_DEV_ID);
    if (p_chip == NULL) {
        aw_kprintf("nand error!\r\n");
        return ;
    }

    /* 如果块i是坏块，继续循环 */
    while (awbl_nand_blk_is_bad(p_chip, i) == AW_TRUE) {
        i++;
    }

    /* 擦除该块 */
    ret = awbl_nand_blk_erase(p_chip, i);
    if (ret != AW_OK) {
        aw_kprintf("awbl_nand_blk_erase fail!%d\r\n",ret);
        return ;
    } else {
        aw_kprintf("awbl_nand_blk_erase successful!\r\n");
    }

    /* 获取该nand设备的页大小 */
    ret = awbl_nand_ioctl(p_chip, AWBL_NAND_PAGE_SIZE, &dat);
    if (ret != AW_OK) {
        aw_kprintf("get NAND_PAGE fail!\r\n");
        return ;
    } else {
        aw_kprintf("NAND_PAGE_SIZE:%d\r\n",dat);
    }

    /* 获取该nand设备的块大小 */
    ret = awbl_nand_ioctl(p_chip, AWBL_NAND_BLK_SIZE,  &dat);
    if (ret != AW_OK) {
        aw_kprintf("get NAND_BLK fail!\r\n");
        return ;
    } else {
        aw_kprintf("NAND_BLK_SIZE:%d\r\n",dat);
    }

    /* 获取该nand设备的oob大小 */
    ret = awbl_nand_ioctl(p_chip, AWBL_NAND_OOB_SIZE,  &dat);
    if (ret != AW_OK) {
        aw_kprintf("get NAND_OOB fail!\r\n");
        return ;
    } else {
        aw_kprintf("NAND_OOB_SIZE:%d\r\n",dat);
    }

    /* 不带ecc的读写操作 */
    strcpy((char *)buf, "test nandflash.");
    ret = awbl_nand_write(p_chip, 1, 0, buf, 100);  /* 写flash，不带ecc校验，写页1，偏移为0，写入长度100 */
    if (ret != AW_OK) {
        aw_kprintf("awbl_nand_write fail!\r\n");
    }
    memset(buf, 0, 2048);
    ret = awbl_nand_read(p_chip, 1, 0, buf, 100);   /* 读flash，不带ecc校验，读页1，偏移为0，读100字节 */
    if (ret != AW_OK) {
        aw_kprintf("awbl_nand_read fail!\r\n");
    }
    AW_INFOF(("buf : %s\n", buf));  /* 打印buf数据 */

    /* 带ecc的读写操作 */
    strcpy((char *)buf, "test nandflash wtih ecc.");
    ret = awbl_nand_page_write(p_chip, buf, 2);     /* 带ecc校验，写页2，buf的大小必须要大于或等于nandflash的页大小，该操作会写一整页 */
    if (ret != AW_OK) {
        aw_kprintf("awbl_nand_page_write fail!\r\n");
    }
    memset(buf, 0, 2048);
    ret = awbl_nand_page_read(p_chip, buf, 2);      /* 带ecc校验，读页2，buf的大小必须要大于或等于nandflash的页大小，该操作会读一整页 */
    if (ret != AW_OK) {
        aw_kprintf("awbl_nand_page_read fail!\r\n");
    }
    AW_INFOF(("buf : %s\n", buf));  /* 打印buf数据 */


    /* 对保留区进行读写操作 */
    strncpy(oob, "oob-data", 10);
    ret = awbl_nand_oob_write(p_chip, 2, 0, oob, 10);   /* 写数据到oob区 */
    if (ret != AW_OK) {
        aw_kprintf("awbl_nand_oob_write fail!\r\n");
        return ;
    }
    memset(oob, '\0', sizeof(oob)); /* 清空缓存区 */
    ret = awbl_nand_oob_read(p_chip, 2, 0, oob, 10);    /* 读oob区数据 */
    AW_INFOF(("oob : %s\n", oob));  /* 打印oob数据 */

    /* 擦除本次读写操作 */
    ret = awbl_nand_blk_erase(p_chip, i);
    if (ret != AW_OK) {
        aw_kprintf("awbl_nand_blk_erase fail!%d\r\n",ret);
    }

    return ;
}

/** [src_nand] */

/* end of file */
