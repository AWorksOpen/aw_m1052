/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/
/**
 * \file
 * \brief ���벶���������
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - QTIMER_CAP��
 *   2. ���ڵ����������ã�������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ�������ء�
 *   3. �������ϵ� DE_GPIO_PWM �� DE_TIMER_CAPID_GPIO ���Ŷ̽�,
 *      ���г���ͨ�����ڵ������ֹ۲첶������
 *
 * \par Դ����
 * \snippet demo_canfd.c src_canfd
 *
 * \internal
 * \par Modification history
 * - 1.00 18-09-05  mex, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_timer_cap ���벶��
 * \copydoc demo_timer_cap.c
 */

/** [src_timer_cap] */
#include "aworks.h"
#include "aw_gpio.h"
#include "aw_cap.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_demo_config.h"
#include "aw_timer.h"
#include "aw_sem.h"

/* ����һ���������ź��� */
AW_SEMB_DECL_STATIC(cap_semb);

/* �������벶׽ʹ�õ�����ͨ�� */
#define __TEST_CAP_ID       DE_TIMER_CAPID

/* ���벶������ͨ����Ӧ������ */
#define __TEST_CAPID_GPIO   DE_TIMER_CAPID_GPIO

/* GPIO ģ�����PWM���� */
#define __TEST_GPIO_PWM     DE_GPIO_PWM

/* ����ȫ�ֵĲ����ź����� */
unsigned int g_period_ns;

/**
 * \brief ����PWM������
 */
static void __my_callback(void *arg, unsigned int count)
{
    static unsigned int num = 0;

    if (num == 0) {
        num = 1;
    } else {
        num = 0;
        aw_cap_disable(__TEST_CAP_ID);

        /* count �б��������β���ļ�����ֵ */
        aw_cap_count_to_time(__TEST_CAP_ID, 0, count, &g_period_ns);

        /* �ͷ��ź��� */
        AW_SEMB_GIVE(cap_semb);
    }
}

/**
 * \brief �����ʱ���ж���ں���
 */
aw_local void __timer_isr (void *p_arg)
{

    /* GPIO��תģ�����PWM */
    aw_gpio_toggle(__TEST_GPIO_PWM);

    /*  �ٴζ�ʱ2ms */
    aw_timer_start(p_arg, aw_ms_to_ticks(2));
}


/**
 * \brief ������
 */
void demo_timer_cap_entry (void)
{
    int ret;
    aw_timer_t timer;
    int cap_test_gpios[] = {
         __TEST_GPIO_PWM,
    };

    /* ��ʼ���ź��� */
    AW_SEMB_INIT(cap_semb,
                 AW_SEM_EMPTY,    /* ��ʼ��Ϊ��״̬ */
                 AW_SEM_Q_FIFO);  /* ��ʼ��ʱ��ѡ�����ã�AW_SEM_Q_FIFO���Ⱥ�˳���Ŷӣ�*/
                                  /* AW_SEM_Q_PRIORITY (�����ȼ��Ŷ�)   */

    /* ��ʼ��һ�������ʱ�� */
    aw_timer_init(&timer, __timer_isr, (void *)&timer);

    /* ��ʱ2ms�� ���������ʱ�� */
    aw_timer_start(&timer, aw_ms_to_ticks(2));

    /* ����������PWM���� */
    if (aw_gpio_pin_request("cap_test_gpios", cap_test_gpios, AW_NELEMENTS(cap_test_gpios)) == AW_OK) {
        aw_gpio_pin_cfg(__TEST_GPIO_PWM, AW_GPIO_OUTPUT_INIT_LOW | AW_GPIO_PULL_UP);
    }

    /* �������벶��Ϊ�����ش��� */
    ret = aw_cap_config(__TEST_CAP_ID, AW_CAP_TRIGGER_RISE, &__my_callback, 0);
    if (ret != AW_OK) {
        aw_kprintf("cap config failed...\r\n");
        aw_timer_stop(&timer);
        return ;
    }

    /* �������� */
    ret = aw_cap_enable(__TEST_CAP_ID);
    if (ret != AW_OK) {
        aw_kprintf("cap enable failed...\r\n");
        aw_timer_stop(&timer);
        return ;
    }

    while (1) {

        /* �ȴ��ź��� */
        AW_SEMB_TAKE(cap_semb, AW_SEM_WAIT_FOREVER);

        /* ��ӡ��� */
        aw_kprintf("The period of the signal is:%d ns\n", g_period_ns);

        /* ������������ */
        aw_cap_enable(__TEST_CAP_ID);
    }
}


/** [src_timer_cap] */

/* end of file */
