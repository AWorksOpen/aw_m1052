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
 * \brief sdram���ڴ��������
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_COM_CONSOLE
 *      - ��Ӧƽ̨�Ĵ��ں�
 *
 * - ʵ������
 *   1. ���ڴ�ӡ������Ϣ��
 *
 * - ��ע��
 *   1. �û����ڶ�Ӧƽ̨��sdram���ӽű��в鿴sdram�ĵ�ַ��Χ����������sdram���з�����1M���ڴ棬���в�����
 *
 * \par Դ����
 * \snippet demo_sdram.c src_sdram
 *
 * \internal
 * \par Modification History
 * - 1.00 18-06-27  lqy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_sdram SDRAM����
 * \copydoc demo_sdram.c
 */

/** [src_sdram] */

#include "aworks.h"                   /* ��ͷ�ļ����뱻���Ȱ��� */
#include "aw_vdebug.h"
#include "aw_shell.h"
#include "aw_mem.h"
#include "aw_psp_task.h"

/* ���� 1M �ֽڵ�SDRAM�ڴ� */
#define SDRAM_SIZE                  (1 * 1024 * 1024)

/* ���� 1M �ֽڵ�SDRAM�ڴ�,4�ֽڶ��� */
#define SDRAM_START_ADDR            aw_mem_align(SDRAM_SIZE, 4)

/* ��������sdram��ʼ��ַ */
static uint32_t *__g_sdram_start_addr;

static uint32_t test_mask = 0x0;

static void __test_sdram_write_init (void)
{
    uint32_t   *p     = (uint32_t *)SDRAM_START_ADDR;
    uint32_t    count = SDRAM_SIZE / sizeof(uint32_t);
    uint32_t    i;
    aw_tick_t   t1, t2;
    double      rate;
    uint32_t    mask;

    __g_sdram_start_addr = p;

    test_mask = ~test_mask;
    mask = test_mask;

    /* ��sdram�ڴ���д������ */
    t1 = aw_sys_tick_get();
    for (i = 0; i < count; i++) {
        *p = i ^ mask;
        p++;
    }
    t2 = aw_sys_tick_get();

    /* ����д�ٶ� */
    rate  = SDRAM_SIZE;
    rate /= (t2 - t1);
    rate *= 1000;
    rate /= (1024 * 1024);
    aw_kprintf("%s:%.2f MB/s\n", "test_sdram_write", rate);

    return ;
}

static int __test_sdram_read_verfy (void)
{
    uint32_t  *p = __g_sdram_start_addr;
    uint32_t   count = SDRAM_SIZE / sizeof(uint32_t);
    uint32_t   i;
    int        ret;
    aw_tick_t  t1, t2;
    double     rate;
    uint32_t   mask;
    uint32_t   temp;

    ret  = 0;
    mask = test_mask;

    /* ��sdram�ڴ��ж�ȡ���ݣ���У�� */
    t1 = aw_sys_tick_get();
    for (i = 0; i < count; i++) {
        temp = *p;
        p++;
        if ((i ^ mask) != temp) {
            ret++;
        }
    }
    t2 = aw_sys_tick_get();

    /* ����������ٶ� */
    rate  = SDRAM_SIZE;
    rate /= (t2 - t1);
    rate *= 1000;
    rate /= (1024 * 1024);
    aw_kprintf("%s:%.2f MB/s\n", "test_sdram_read", rate);

    return ret;
}

void demo_sdram_entry (void)
{
    aw_kprintf("demo_sdram test...\n");

    int r;
    int count = 5;

    /* ��sdram�Ķ��ڴ���д������ */
    __test_sdram_write_init();

    count = 5;
    while (count --) {
        aw_task_delay(1000);

        /* ��sdram�Ķ��ڴ��ж�ȡ���ݲ�У�� */
        r = __test_sdram_read_verfy();
        if (0 == r) {
            aw_kprintf("test sdram success\n");
        } else {
            aw_kprintf("test sdram failed:r= %d\n", r);
        }
    }

    /* �ͷŷ����sdram���ڴ� */
    aw_mem_free(__g_sdram_start_addr);
    return ;
}

/** [src_sdram] */

/* end of file */
