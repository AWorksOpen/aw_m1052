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
 * \brief �����ʱ������ʾ����
 *
 * ������ʹ��ϵͳ��ʱ���ṩ�����ʱ���ġ�ʱ�ӡ���
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��AW_COM_CONSOLE�͵��Դ���ʹ�ܺꣻ
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   3. ���������ط���LED�ȵ�ʹ��
 *
 * - ʵ������:
 *   1. LED0 �� 2s Ϊ������˸
 *
 * \par Դ����
 * \snippet demo_sftimer_lib.c src_sftimer_lib
 *
 * \internal
 * \par History
 * - 1.00 16-03-25  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_sftimer_lib �����ʱ��
 * \copydoc demo_sftimer_lib.c
 */

/** [src_sftimer_lib] */
#include "aworks.h"
#include "aw_sftimer_lib.h"
#include "aw_vdebug.h"
#include "aw_timer.h"
#include "aw_led.h"

#define BUCKET_SIZE   6     /* ��ʱ��ɢ���� */
#define HW_CLKRATE    10    /* ϵͳ��ʱ����ʱƵ�� 10ms */
#define SF_TIME_MS    1000  /* ��ʱ����ʱ���� */
#define LED_ID        0     /* LED0*/

/**
 * \brief ��ʱ����ʱ�ص�����
 */
static void __sftimer_handler(void *arg)
{
    aw_led_toggle(LED_ID);
    aw_sftimer_restart((struct aw_sftimer *)arg);
}

/* ϵͳ��ʱ������ */
static  aw_timer_t  my_timer;

static void __my_callback (void *p_arg)
{
    /* ִ�������ʱ����ʱ�ص���������Ҫ���ⲿ��ʱ���������� */
    aw_sftimer_group_tick((struct aw_sftimer_group *)p_arg);

    /* ������ʱ�� */
    aw_timer_start(&my_timer, aw_ms_to_ticks(HW_CLKRATE));
}

/**
 * \brief �����ʱ������ʾ�������
 *
 */
void demo_sftimer_lib_entry (void)
{
    aw_err_t  ret = AW_OK;

    static struct aw_sftimer        my_sftimer;
    static struct aw_sftimer_group  my_sftimer_group;
    static struct aw_sftimer_bucket my_bucket[BUCKET_SIZE];

    /* ��ʼ����ʱ���� */
    ret = aw_sftimer_group_init(&my_sftimer_group,  /* ��ʱ���� */
                                &my_bucket[0],      /* ɢ������ */
                                BUCKET_SIZE,        /* ɢ���� */
                                1000/HW_CLKRATE     /* �ⲿ��ʱƵ��(Hz) */
                                );
    if (ret != AW_OK) {
        AW_ERRF(("aw_sftimer_group_init err: %d\n"));
        return;
    }

    /* ��ʼ����ʱ�� */
    aw_sftimer_init(&my_sftimer,
                    &my_sftimer_group,
                    __sftimer_handler,
                    &my_sftimer);

    /* ���ö�ʱ���� */
    aw_sftimer_start_ms(&my_sftimer, SF_TIME_MS);

    /* ʹ�������ʱ���⣬��Ҫ�ⲿ�ṩ�������еķ������̣����䵱�����ʱ�����
     * ��ʱ�ӡ�
     *
     * �������ʹ��ϵͳ�����ʱ������ʱ�ص�
     */
    aw_timer_init(&my_timer, __my_callback, &my_sftimer_group);

    /* ������ʱ�� */
    aw_timer_start(&my_timer, aw_ms_to_ticks(HW_CLKRATE));
}
/** [src_sftimer_lib] */

/* end of file */
