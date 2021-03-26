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
 * \brief ��׼C�⺯��ʹ�����̣��ڴ棩
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��AW_COM_CONSOLE�͵��Դ��ڶ�Ӧ�ĺꣻ
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. ���ڴ�ӡ������Ϣ��
 *
 * \par Դ����
 * \snippet demo_libc_mem.c src_libc_mem
 *
 * \internal
 * \par Modification history
 * - 1.00 17-09-07  mkr, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_libc_mem �ڴ�(��׼C��)
 * \copydoc demo_libc_mem.c
 */

/** [src_libc_mem] */
#include "aworks.h"
#include "aw_vdebug.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * \brief libc �ڴ�ӿ�ʾ����ں���
 *
 * \return ��
 */
void demo_libc_mem_entry (void)
{
    uint8_t  i;
    uint8_t *p_dat = NULL;

    aw_kprintf("\r\nLibc memory interface testing...\r\n");

    p_dat = (uint8_t *)malloc(10 * sizeof(uint8_t));
    if (NULL == p_dat) {
        aw_kprintf("malloc allocate failed!\r\n");
    } else {
        aw_kprintf("malloc allocate success!\r\n");
    }

    for (i = 0; i < 9; i++) {
        p_dat[i] = '0' + i;
    }
    p_dat[i] = '\0';

    aw_kprintf("pdat = %s\r\n", p_dat);

    free(p_dat);
    p_dat = NULL;

    aw_kprintf("memory was freed\r\n");
}

/** [src_libc_mem] */

/* end of file */
