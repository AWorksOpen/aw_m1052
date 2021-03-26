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
 ******************************************************************************/

/**
 * \file
 * \brief �ӳ���ҵ��ʾ����
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��AW_COM_CONSOLE��
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *     ���ڴ�ӡ������Ϣ��
 *
 * \par Դ����
 * \snippet demo_delayed_work.c src_delayed_work
 *
 * \internal
 * \par Modification History
 * - 1.00 17-09-06  sup, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_delayed_work �ӳ���ҵ
 * \copydoc demo_delayed_work.c
 */

/** [src_delayed_work] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_delayed_work.h"

 static void work_callback (void *p_arg)
 {
     static uint8_t i = 0;

     i++;
     AW_INFOF(("Process delay work:%d\n", i));

     if (i < 5) {
         AW_INFOF(("Delayed work restart:%d\n", i));
         /* �ٴ�����work, ��ʱʱ���Ϊ1s */
         aw_delayed_work_start((struct aw_delayed_work *)p_arg, 1000);

     } else {
         AW_INFOF(("Delayed work stop\n"));
         /* ֹͣdelayed work */
         aw_delayed_work_stop((struct aw_delayed_work *)p_arg);
     }
 }

/**
 * \brief �ӳ���ҵ��ʾ�������
 * \return ��
 */
void demo_delayed_work_entry (void)
{
    /* �����ӳ���ҵ���� */
    static struct aw_delayed_work work;

    /* ��ʼ��delayed work */
    aw_delayed_work_init(&work, work_callback, &work);

    /* ����delayed work */
    AW_INFOF(("Start delayed work,process work after 3s\n"));
    aw_delayed_work_start(&work, 3000); /* 3s������work */
}

/** [src_delayed_work] */

/* end of file */
