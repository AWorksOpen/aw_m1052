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
 * \brief �ڴ����ʾ����
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��AW_COM_CONSOLE�͵��Դ���ʹ�ܺꣻ
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������:
 *      ���ڴ�ӡ �ڴ�� ���������Ϣ��
 *
 * \par Դ����
 * \snippet demo_pool.c src_pool
 *
 * \internal
 * \par History
 * - 1.00 17-09-12  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_pool �ڴ��
 * \copydoc demo_pool.c
 */

/** [src_pool] */
#include "aworks.h"
#include "aw_pool.h"
#include "aw_vdebug.h"

/**
 * \brief �ڴ���������
 *
 */
void demo_pool_entry (void)
{
#define  TESN_POOL_CNT  10

    aw_pool_t       my_pool;                /* �ڴ�ض��� */
    aw_pool_id_t    my_pool_id;             /* �ڴ�ؾ�� */
    static int      bulk[TESN_POOL_CNT];    /* �ڴ�� */

    int             i;
    int            *p_item[TESN_POOL_CNT] = {0};

    /* �ڴ�س�ʼ�� */
    my_pool_id = aw_pool_init(&my_pool,
                              bulk,
                              sizeof(bulk),
                              sizeof(bulk[0]));
    if (NULL == my_pool_id) {
        AW_ERRF(("aw_pool_init err!\n"));
        return;
    }

    /* ���Դ��ڴ�ػ�ȡ�ڴ� */
    for (i = 0; i < TESN_POOL_CNT; i++) {

        /* ���ڴ�ط����ڴ� */
        p_item[i]  = (int *)aw_pool_item_get(my_pool_id);
        if (NULL == p_item[i]) {
            AW_ERRF(("pool have no mem!\n"));
            break;
        }

        /* ʹ���ڴ� */
        *p_item[i] = i;
    }

    /* �ͷŴ��ڴ�ط�����ڴ� */
    for (i = 0; i < TESN_POOL_CNT; i++) {
        if (p_item[i] != NULL) {
            aw_pool_item_return(my_pool_id, p_item[i]);
        }
    }

    AW_INFOF(("pool demo done.\n"));
}

/** [src_pool] */

/* end of file */
