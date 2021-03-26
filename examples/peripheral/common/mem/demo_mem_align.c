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
 * \brief ���ڴ��������
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_COM_CONSOLE
 *      - ��Ӧƽ̨�Ĵ��ں�
 *
 * - ʵ������
 *   1. ���ڴ�ӡ�ڴ����ɹ�ʱʹ�õ��ֽڶ����������Կ���ֻ�ж��볤����4x2^n�������ڴ�Ż�ɹ���
 *
 * \par Դ����
 * \snippet demo_mem_align.c src_mem_align
 *
 * \internal
 * \par Modification History
 * - 1.00 18-06-27  lqy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_mem_align ���ڴ����
 * \copydoc demo_mem_align.c
 */

/** [src_mem_align] */
#include "aworks.h"
#include "aw_mem.h"
#include "aw_cache.h"
#include "aw_vdebug.h"

#define     BUF_SIZE    1024
#define     ALIGN(i)    ((0) + (i))

void demo_mem_align_entry (void)
{
    uint8_t    *p_temp = NULL;
    uint32_t    i      = 0;

    aw_kprintf(" aw_cache_dam_align: \n");
    for (i = 0; i < 500; i++) {

       /* ALIGN(i)���볤����4x2^n�������ڴ�Ż�ɹ� */
        p_temp = (uint8_t *)aw_cache_dma_align(BUF_SIZE, ALIGN(i));
        if (NULL != p_temp) {
            aw_kprintf(" address: 0x%X ", p_temp);
            if (((uint32_t)p_temp % ALIGN(i)) != 0) {
                aw_kprintf(" misaligned %d \n", ALIGN(i));
            } else {
                aw_kprintf(" address: 0x%X align %d \n", p_temp, ALIGN(i));
            }
            aw_cache_dma_free(p_temp);
        }
    }

    aw_kprintf(" aw_mem_align: \n");
    for (i = 0; i < 500; i++) {

        /* ALIGN(i)���볤����4x2^n�������ڴ�Ż�ɹ� */
        p_temp = (uint8_t *)aw_mem_align(BUF_SIZE, ALIGN(i));
        if (NULL != p_temp) {
            aw_kprintf(" address: 0x%X ", p_temp);
            if (((uint32_t)p_temp % ALIGN(i)) != 0) {
                aw_kprintf(" misaligned %d \n", ALIGN(i));
            } else {
                aw_kprintf(" align %d \n", ALIGN(i));
            }
            aw_mem_free(p_temp);
        }
    }
}

/** [src_mem_align] */

/* end of file */
