/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief nandflashע��Ϊmtd�豸��д����
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
 *   1. ���ڴ�ӡ��д������Ϣ��
 *
 * \par Դ����
 * \snippet demo_nand_mtd.c src_demo_nand_mtd
 *
 * \internal
 * \par Modification History
 * - 1.00 18-06-26  lqy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_nand_mtd nandflash(mtd�豸)
 * \copydoc demo_nand_mtd.c
 */

/** [src_demo_nand_mtd] */

#include "aworks.h"
#include "aw_mem.h"
#include "aw_vdebug.h"
#include "aw_errno.h"
#include "io/sys/aw_stat.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "string.h"
#include "mtd/aw_mtd.h"
#include "aw_demo_config.h"
#include <stdlib.h>

static struct aw_mtd_info  *gp_mtd          = NULL;
static size_t               g_burn_offset   = 0;
static uint8_t             *gp_rw_buf, *gp_tmp_buf;

/* ������С */
#define BLOCK_SIZE          2048

/* �������� */
#define BLOCK_COUNT         100

#define DISK_NAME           DE_DISK_NAME

/**
 * \brief nand_mtd��д����
 * \return ��
 */
static void __nand_mtd_write_read (void)
{
    size_t                   real;
    struct aw_mtd_erase_info mtd_erase;
    int                      ret;
    int                      i;
    int                      offset = 0;
    uint32_t                 start_time;
    uint32_t                 end_time;
    float                    speed;


    AW_INFOF(("Erase mtd device...\n"));

    mtd_erase.mtd      = gp_mtd;
    mtd_erase.addr     = (g_burn_offset / gp_mtd->erase_size) * gp_mtd->erase_size;
    mtd_erase.len      = AW_DIV_ROUND_UP(BLOCK_SIZE*BLOCK_COUNT, gp_mtd->erase_size) * gp_mtd->erase_size;
    mtd_erase.callback = NULL;

    ret = aw_mtd_erase(gp_mtd, &mtd_erase);
    if (ret != AW_OK) {
        aw_kprintf("aw_mtd_erase error!\n");
    }

    /* set the data for write */
    for (i = 0; i< BLOCK_SIZE; i++) {
        gp_rw_buf[i] = (uint8_t)i;
    }

    /* д�� */
    AW_INFOF(("Write mtd device...\n"));

    start_time = aw_sys_tick_get();
    offset = 0;
    for (i = 0; i < BLOCK_COUNT; i++) {
        ret = aw_mtd_write(gp_mtd, g_burn_offset + offset, BLOCK_SIZE, &real, gp_rw_buf);
        if (ret != AW_OK) {
            aw_kprintf("aw_mtd_write error!\n");
        }

        offset += BLOCK_SIZE;
    }
    end_time = aw_sys_tick_get();
    speed    = BLOCK_SIZE * BLOCK_COUNT;
    speed   /= end_time - start_time;
    speed   *= 1000;
    speed   /= 1024*1024;
    aw_kprintf("test_fs_read_write write speed %.3f MB/s\n",speed);

    /* ���� */
    AW_INFOF(("Read mtd device...\n"));

    start_time = aw_sys_tick_get();

    offset = 0;
    for (i = 0; i < BLOCK_COUNT; i++) {
        ret = aw_mtd_read(gp_mtd, g_burn_offset + offset, BLOCK_SIZE, &real, gp_tmp_buf);
        if (ret != AW_OK) {
            aw_kprintf("aw_mtd_read error!\n");
        }

        offset += BLOCK_SIZE;
    }

    end_time = aw_sys_tick_get();

    speed  = BLOCK_SIZE * BLOCK_COUNT;
    speed /= end_time - start_time;
    speed *= 1000;
    speed /= 1024*1024;
    aw_kprintf("test_fs_read_write read speed %.3f MB/s\n",speed);

    /* �Ƚ� */
    AW_INFOF(("compare data...\n"));
    offset = 0;
    for (i = 0; i < BLOCK_COUNT; i++) {
        ret = aw_mtd_read(gp_mtd, g_burn_offset + offset, BLOCK_SIZE, &real, gp_tmp_buf);
        if (ret != AW_OK) {
            aw_kprintf("aw_mtd_read error!\n");
        }

        ret = memcmp(gp_rw_buf, gp_tmp_buf, BLOCK_SIZE);
        if (ret != AW_OK) {
            aw_kprintf("memcmp error!\n");
            return ;
        }
        offset += BLOCK_SIZE;
    }
    aw_kprintf("compare OK!\n");

}

/**
 * \brief nand mtd����
 * \return ��
 */
void demo_nand_mtd_entry (void)
{
    /*
     * note: mtd������(DISK_NAME)�� nandflash ��Ӳ������(awbl_hwconf_imxxxxxxx_nandflash.h)�����
     */
    gp_mtd = aw_mtd_dev_find(DISK_NAME);
    if (gp_mtd == NULL) {
        AW_INFOF(("Please input valid device name!\n"));
        return ;
    }
    gp_rw_buf  = aw_mem_alloc(BLOCK_SIZE);
    gp_tmp_buf = aw_mem_alloc(BLOCK_SIZE);

    if (gp_rw_buf == NULL || gp_tmp_buf == NULL) {
        AW_INFOF(("aw_mem_alloc fail!\n"));
        return ;
    }

    aw_kprintf("\n");

    /* mtd��д���� */
    __nand_mtd_write_read();

    aw_mem_free(gp_rw_buf);
    aw_mem_free(gp_tmp_buf);

    return ;
}

/** [src_demo_nand_mtd] */

/* end of file */
