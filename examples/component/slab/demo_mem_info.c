/*******************************************************************************
*                                 AWorks
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
 * \brief slab�ڴ����������(�۲��ڴ�ʹ�����)
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��AW_COM_CONSOLE�͵��Դ���ʹ�ܺꣻ
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������:
 *   1. ��ӡ����һ�� aw_slab_malloc() ֮���ڴ��ʹ�����.
 *
 * \par Դ����
 * \snippet demo_slab.c src_slab
 *
 * \internal
 * \par History
 * - 1.00 19-03-19  imp, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_slab
 * \copydoc demo_slab.c
 */

/** [src_slab] */
#include "aworks.h"
#include "aw_slab.h"
#include "aw_vdebug.h"

#define TRUNK_SIZE (30 * 1024)
#define ALLOC_SIZE 1024

static char  __g_trunk[TRUNK_SIZE];

/**
 * \brief slab ʾ��
 * \return ��
 */
void demo_mem_info_entry (void)
{
    uint32_t total;
    uint32_t used;
    uint32_t peak;
    char    *p_buf;

    struct aw_slab  foo;

    /* ��ʼ��slab */
    aw_slab_init(&foo);

    /* Ϊslab�����ڴ�ռ� */
    aw_slab_easy_catenate(&foo, __g_trunk, TRUNK_SIZE);
 
    /* ʹ��slab��̬�����ڴ� */
    p_buf = aw_slab_malloc(&foo, ALLOC_SIZE);

    /* ʹ�÷�����ڴ� */
    if(p_buf) {
        aw_slab_memory_info(&foo, &total, &used, &peak);
        aw_kprintf("Memory Info: Total %d, Used %d, Peak %d.\r\n", 
            total, used, peak);

        /* �ͷ�slab������ڴ� */
        aw_slab_free(&foo, p_buf);

    } else {
        aw_kprintf("Malloc failed.\r\n");
    }
}

/** [src_slab] */

/* end of file */
