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
 * \brief nand ��д����
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - NANDFLASH�豸��
 *      - AW_COM_CONSOLE
 *      - AW_COM_SHELL_SERIAL
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. ���� nand ָ����0���1,2ҳд�����ݣ����Ҵ��ڽ���ӡ����д���ݵĲ�����Ϣ��
 *
 * \par Դ����
 * \snippet demo_nand.c src_nand
 */

/**
 * \addtogroup demo_if_nand nandflash(�����д)
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

/* ����nand�豸 */
static struct awbl_nand_chip *__nand_dev_find (const char *name, int unit)
{
    return (struct awbl_nand_chip *)AW_CONTAINER_OF(awbl_dev_find_by_name(name, unit),
            struct awbl_nand_chip,
            awdev);
}

/**
 * \brief nand ��д����
 * \return ��
 */
void demo_nand_entry (void)
{
    struct awbl_nand_chip *p_chip = NULL;

    int         dat;
    aw_err_t    ret;
    uint8_t     buf[2048];
    char        oob[28];
    int         i = 0;

    /* ͨ��nandflash�豸��������ȡnand�豸 */
    p_chip = __nand_dev_find(NAND_FLASH_DEV_NAME, NAND_FLASH_DEV_ID);
    if (p_chip == NULL) {
        aw_kprintf("nand error!\r\n");
        return ;
    }

    /* �����i�ǻ��飬����ѭ�� */
    while (awbl_nand_blk_is_bad(p_chip, i) == AW_TRUE) {
        i++;
    }

    /* �����ÿ� */
    ret = awbl_nand_blk_erase(p_chip, i);
    if (ret != AW_OK) {
        aw_kprintf("awbl_nand_blk_erase fail!%d\r\n",ret);
        return ;
    } else {
        aw_kprintf("awbl_nand_blk_erase successful!\r\n");
    }

    /* ��ȡ��nand�豸��ҳ��С */
    ret = awbl_nand_ioctl(p_chip, AWBL_NAND_PAGE_SIZE, &dat);
    if (ret != AW_OK) {
        aw_kprintf("get NAND_PAGE fail!\r\n");
        return ;
    } else {
        aw_kprintf("NAND_PAGE_SIZE:%d\r\n",dat);
    }

    /* ��ȡ��nand�豸�Ŀ��С */
    ret = awbl_nand_ioctl(p_chip, AWBL_NAND_BLK_SIZE,  &dat);
    if (ret != AW_OK) {
        aw_kprintf("get NAND_BLK fail!\r\n");
        return ;
    } else {
        aw_kprintf("NAND_BLK_SIZE:%d\r\n",dat);
    }

    /* ��ȡ��nand�豸��oob��С */
    ret = awbl_nand_ioctl(p_chip, AWBL_NAND_OOB_SIZE,  &dat);
    if (ret != AW_OK) {
        aw_kprintf("get NAND_OOB fail!\r\n");
        return ;
    } else {
        aw_kprintf("NAND_OOB_SIZE:%d\r\n",dat);
    }

    /* ����ecc�Ķ�д���� */
    strcpy((char *)buf, "test nandflash.");
    ret = awbl_nand_write(p_chip, 1, 0, buf, 100);  /* дflash������eccУ�飬дҳ1��ƫ��Ϊ0��д�볤��100 */
    if (ret != AW_OK) {
        aw_kprintf("awbl_nand_write fail!\r\n");
    }
    memset(buf, 0, 2048);
    ret = awbl_nand_read(p_chip, 1, 0, buf, 100);   /* ��flash������eccУ�飬��ҳ1��ƫ��Ϊ0����100�ֽ� */
    if (ret != AW_OK) {
        aw_kprintf("awbl_nand_read fail!\r\n");
    }
    AW_INFOF(("buf : %s\n", buf));  /* ��ӡbuf���� */

    /* ��ecc�Ķ�д���� */
    strcpy((char *)buf, "test nandflash wtih ecc.");
    ret = awbl_nand_page_write(p_chip, buf, 2);     /* ��eccУ�飬дҳ2��buf�Ĵ�С����Ҫ���ڻ����nandflash��ҳ��С���ò�����дһ��ҳ */
    if (ret != AW_OK) {
        aw_kprintf("awbl_nand_page_write fail!\r\n");
    }
    memset(buf, 0, 2048);
    ret = awbl_nand_page_read(p_chip, buf, 2);      /* ��eccУ�飬��ҳ2��buf�Ĵ�С����Ҫ���ڻ����nandflash��ҳ��С���ò������һ��ҳ */
    if (ret != AW_OK) {
        aw_kprintf("awbl_nand_page_read fail!\r\n");
    }
    AW_INFOF(("buf : %s\n", buf));  /* ��ӡbuf���� */


    /* �Ա��������ж�д���� */
    strncpy(oob, "oob-data", 10);
    ret = awbl_nand_oob_write(p_chip, 2, 0, oob, 10);   /* д���ݵ�oob�� */
    if (ret != AW_OK) {
        aw_kprintf("awbl_nand_oob_write fail!\r\n");
        return ;
    }
    memset(oob, '\0', sizeof(oob)); /* ��ջ����� */
    ret = awbl_nand_oob_read(p_chip, 2, 0, oob, 10);    /* ��oob������ */
    AW_INFOF(("oob : %s\n", oob));  /* ��ӡoob���� */

    /* �������ζ�д���� */
    ret = awbl_nand_blk_erase(p_chip, i);
    if (ret != AW_OK) {
        aw_kprintf("awbl_nand_blk_erase fail!%d\r\n",ret);
    }

    return ;
}

/** [src_nand] */

/* end of file */
