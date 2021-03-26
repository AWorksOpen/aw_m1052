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
 * \brief ����ʧ�Դ洢���ӿ����̣�ʹ��NVRAM �ӿڶ�д SPI Flash ��ʾ����
 *
 * - ��������:
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - FLASH �豸��
 *      - AW_COM_CONSOLE
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������:
 *   1. ���ڴ�ӡ���������Ϣ��
 *
 * \par Դ����
 * \snippet demo_nvram.c src_nvram
 *
 * \internal
 * \par History
 * - 1.00 17-09-05  deo, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_nvram NVRAM
 * \copydoc demo_nvram.c
 */

/** [src_nvram] */
#include "aworks.h"             /* ��ͷ�ļ����뱻���Ȱ��� */

/* �������õ������·��� */
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_nvram.h"
#include "string.h"

#define TEST_MEM_NAME       "user_data"     /* �洢�����ƣ����������޸� */

#define BUF_SIZE            255             /* ��д��������С */

static uint8_t __g_buf[BUF_SIZE];           /* ��д������ */

/**
 * \brief ʹ��NVRAM �ӿڶ�д SPI Flash ��ʾ����
 * \return ��
 */
void demo_nvram_entry (void)
{
    uint32_t i;

    for (i = 0; i < BUF_SIZE; i++) {
        __g_buf[i] = i & 0xFF;
    }

    /* ���� nvram �� flexspi_flash ���� */
    if (aw_nvram_set(TEST_MEM_NAME, 0, __g_buf, 0, BUF_SIZE) != AW_OK) {
        AW_INFOF(("SPIFLASH NVRAM: Write Failed!\r\n"));
        return ;
    }

    memset(__g_buf, 0, BUF_SIZE);

    /* ��ȡ nvram �� flexspi_flash ���� */
    if (aw_nvram_get(TEST_MEM_NAME, 0, __g_buf, 0, BUF_SIZE) != AW_OK) {
        AW_INFOF(("SPIFLASH NVRAM: Read Failed!\r\n"));
        return ;
    }

    for (i = 0; i < BUF_SIZE; i++) {
        if (__g_buf[i] != (i & 0xFF)) {
            AW_INFOF(("SPIFLASH NVRAM: Verify Failed!\r\n"));
            return;
        }
    }

    AW_INFOF(("SPIFLASH NVRAM: Verify Successful!\r\n"));
}

/** [src_nvram] */

/* end of file */
