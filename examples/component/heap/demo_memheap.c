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
 * \brief �ѹ�������ʾ����
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ�� AW_COM_CONSOLE�͵��Դ��ڶ�Ӧ�ĺꣻ
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������:
 *   1. ��ӡһ��Ӷ��ڴ����������ڴ�ĵ�ַ
 *
 * \par Դ����
 * \snippet demo_memheap.c src_memheap
 *
 * \internal
 * \par History
 * - 1.00 16-03-25  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_memheap �ѹ�����
 * \copydoc demo_memheap.c
 */

/** [src_memheap] */
#include "aworks.h"
#include "aw_memheap.h"
#include "aw_vdebug.h"

#define  HEAP_SIZE 1024

static char         __g_heap_buf[HEAP_SIZE];    /* �ѹ�����ʹ�õ��ڴ� */
static aw_memheap_t __g_my_heap;                /* �ѹ��������� */

/**
 * \brief �ѹ���������
 *
 * \return ��
 */
void demo_memheap_entry (void)
{
    aw_err_t  ret = AW_OK;
    int       i   = 0;

    char     *ptr = NULL;

    /* �ѹ�������ʼ�� */
    ret = aw_memheap_init(&__g_my_heap,     // �ѹ�����
                          "my_heap",        // �ڴ�ѵ�����
                          __g_heap_buf,     // ���õĵ�ַ�ռ�
                          HEAP_SIZE);       // ��ַ�ռ��С
    if (ret != AW_OK) {
        AW_ERRF(("aw_memheap_init err: %d\n", ret));
        return;
    }

    /* �Ӷѹ������з����ڴ� */
    ptr = (char *)aw_memheap_alloc(&__g_my_heap, 32);
    if (NULL == ptr) {
        AW_ERRF(("aw_memheap_alloc err!\n"));
        return;
    }

    /* ���Է�����ڴ� */
    for (i = 0; i < 32; i++) {
        ptr[i] = (char)i;
    }

    /* �ͷŷ�����ڴ� */
    if (ptr) {
        aw_kprintf("Got an addr 0x%x.\r\n", ptr);
        aw_memheap_free(ptr);

    } else {
        aw_kprintf("Had nothing got.\r\n");
    } 
}

/** [src_memheap] */

/* end of file */
