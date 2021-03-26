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
 * \brief slab�ڴ����������(�����ڴ���Ƭ)
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��AW_COM_CONSOLE�͵��Դ���ʹ�ܺꣻ
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������:
 *   1. ��ӡһ�鶯̬�ڴ�ĵ�ַ
 *
 * \par Դ����
 * \snippet demo_slab.c src_slab
 *
 * \internal
 * \par History
 * - 1.00 16-03-25  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_slab slab�ڴ������
 * \copydoc demo_slab.c
 */

/** [src_slab] */
#include "aworks.h"
#include "aw_slab.h"
#include "aw_vdebug.h"

#include <string.h>

#define TRUNK_SIZE (100 * 1024)
#define ALLOC_SIZE 1024


/**
 * \brief slab ʾ��
 * \return ��
 */
void demo_slab_entry (void)
{
    static char     slab_buf[TRUNK_SIZE] = {0};
    char           *p_buf = NULL;

    struct aw_slab  foo;

    /* ��ʼ��slab */
    aw_slab_init(&foo);

    /* Ϊslab�����ڴ�ռ� */
    aw_slab_easy_catenate(&foo, slab_buf, TRUNK_SIZE);
 
    /* ʹ��slab��̬�����ڴ� */
    p_buf = aw_slab_malloc(&foo, ALLOC_SIZE);

    /* ʹ�÷�����ڴ� */
    if (p_buf) {
        memset(p_buf, 0, ALLOC_SIZE);
        aw_kprintf("Got an addr 0x%x.\r\n", p_buf);

        /* �ͷ�slab������ڴ� */
        aw_slab_free(&foo, p_buf);

    } else {
        aw_kprintf("Had nothing to be alloced.\r\n");
    }
}

/** [src_slab] */

/* end of file */
