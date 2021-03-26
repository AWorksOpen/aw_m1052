/*******************************************************************************
*                                  AWorks
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
 * \brief no-cache�ڴ��������
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - ��Ӧƽ̨�Ĵ��ں�
 *      - AW_COM_CONSOLE
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. ���ڴ�ӡ������Ϣ��
 *
 *
 * \par Դ����
 * \snippet demo_cache.c src_cache
 */

/**
 * \addtogroup demo_if_cache cache����
 * \copydoc demo_cache.c
 */

/** [src_cache] */

#include "aworks.h"
#include "aw_cache.h"
#include "aw_mem.h"
#include "aw_vdebug.h"
#include <string.h>

/**
 * \brief cache demo
 * \return ��
 */
void demo_cache_entry (void)
{
    int   i         = 0;
    char *p_mem     = NULL;
    char *test_str  = "test cache";

    /* ����500�ֽڴ�С�� cache-safe �Ļ����� */
    p_mem = (char *)aw_cache_dma_malloc(500);
    if (p_mem == NULL) {
        aw_kprintf("p_mem is NULL");
        return ;
    }

    memset(p_mem, '\0', 500);
    for (i = 0; i < strlen(test_str) + 1; i++) {
        writeb(*(test_str + i), p_mem + i);
    }

    AW_INFOF(("p_mem addr : %x \n", p_mem));
    AW_INFOF(("p_mem data : %s \n", p_mem));

    /* �ͷ� cache-safe �Ļ����� */
    aw_cache_dma_free(p_mem);

    /* ����ָ�����볤�ȵ� cache-safe �Ļ����� */
    p_mem = aw_cache_dma_align(1000, 16);

    memset(p_mem, '\0', 1000);
    for (i = 0; i < strlen(test_str) + 1; i++) {
        writeb(*(test_str + i), p_mem + i);
    }

    AW_INFOF(("p_mem addr : %x \n", p_mem));
    AW_INFOF(("p_mem data : %s \n", p_mem));

    /* �ͷ� cache-safe �Ļ����� */
    aw_cache_dma_free(p_mem);

    uint8_t *buf = NULL;
    buf = aw_mem_alloc(128);

    for (i = 0; i < 128; i++) {
        buf[i] = (uint8_t)i;
    }

    /* ˢ��buf�ڴ��е�����  */
    aw_cache_flush(buf, sizeof(buf));

    memset(buf, 0, 128);
    aw_cache_flush(buf, 128);

    /* ʹbuf������Ч�����´��ڴ��ж�ȡ����  */
    aw_cache_invalidate(buf, 128);

    /* ����ʹ��buf */
    // todo

    return ;
}

/** [src_cache] */

/* end of file */
