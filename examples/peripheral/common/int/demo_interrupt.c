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
 * \brief interrupt����
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_COM_CONSOLE
 *      - ��Ӧƽ̨�Ĵ��ں�
 *   2. �������ŵĴ�������ʱ�����öŰ��߽�  TEST_IO_INTR �� TEST_IO_OUTPUT ��Ӧ����������������
 *
 * - ʵ������
 *  1. ���ڴ�ӡ������Ϣ
 *
 *
 * \par Դ����
 * \snippet demo_interrupt.c src_interrupt
 *
 * \internal
 * \par Modification history
 * - 1.00 18-06-27  lqy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_interrupt �ж�
 * \copydoc demo_interrupt.c
 */

/** [src_interrupt] */
#include "aworks.h"
#include "aw_gpio.h"
#include "aw_sem.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_demo_config.h"
#include "aw_int.h"

/* ƽ̨GPIO���ɸ��ݾ���ƽ̨�޸� */
#define TEST_IO_INTR    DM_GPIO_TRG_INTR
#define TEST_IO_OUTPUT  DM_GPIO_TRG_OUTPUT

/**\brief ��¼�Ƿ�����ж� */
AW_SEMB_DECL_STATIC(__gpio_intr_semb);


/* gpio ���Իص����� */
static void __test_gpio_trig_isr (void* arg)
{
    AW_SEMB_GIVE(__gpio_intr_semb);
}


void demo_interrupt_entry (void)
{
    aw_err_t    err;
    int         i;

    aw_kprintf("\ninterrupt demo testing...\r\n");

    /* �ź�����ʼ�� */
    AW_SEMB_INIT(__gpio_intr_semb, AW_SEM_EMPTY, AW_SEM_Q_FIFO);

    /* �����ж����Ų��������� */
    int test_gpios[] = {TEST_IO_INTR, TEST_IO_OUTPUT};
    if (aw_gpio_pin_request("test_gpios",
                             test_gpios,
                             AW_NELEMENTS(test_gpios)) == AW_OK) {

        aw_gpio_pin_cfg(TEST_IO_OUTPUT,
                        AW_GPIO_PUSH_PULL | AW_GPIO_OUTPUT_INIT_HIGH);
        aw_gpio_pin_cfg(TEST_IO_INTR, AW_GPIO_INPUT);
    }

    /* �����жϻص����� */
    err = aw_gpio_trigger_connect(TEST_IO_INTR,
                                  __test_gpio_trig_isr,
                                  (void *)AW_GPIO_TRIGGER_FALL);
    if (err != AW_OK) {
        aw_kprintf("gpio trigger connect failed!\n");
        return;
    }

    /* ����Ϊ�½��ش��� */
    err = aw_gpio_trigger_cfg(TEST_IO_INTR, AW_GPIO_TRIGGER_FALL);
    if (err != AW_OK) {
        aw_kprintf("gpio trigger cfg failed!\n");
        return;
    }

    /* �������ŵĴ��� */
    err = aw_gpio_trigger_on(TEST_IO_INTR);
    if (err != AW_OK) {
        aw_kprintf("gpio trigger on failed!\n");
        return;
    }

    for (i = 0; i < 50; i++) {

        /* ��������ܽ�Ϊ�͵�ƽ */
        aw_gpio_set(TEST_IO_OUTPUT, 0);

        /* �ȴ��жϴ��� */
        err = AW_SEMB_TAKE(__gpio_intr_semb, 1000);
        if (err == AW_OK) {
            aw_kprintf("enter gpio interrupt!\n");
        }

        /* ��������ܽ�Ϊ�ߵ�ƽ */
        aw_gpio_set(TEST_IO_OUTPUT, 1);

        aw_mdelay(100);
    }

    /* �Ͽ��ж����ӻص����� */
    aw_gpio_trigger_disconnect(TEST_IO_INTR,
                               __test_gpio_trig_isr,
                               (void *)AW_GPIO_TRIGGER_FALL);

    /* �ر����ŵĴ��� */
    aw_gpio_trigger_off(TEST_IO_INTR);

    /* �ͷ����� */
    aw_gpio_pin_release(test_gpios, AW_NELEMENTS(test_gpios));
    
    /* ��ֹ�ź��� */
    AW_SEMB_TERMINATE(__gpio_intr_semb);

    aw_kprintf("interrupt demo exit...\r\n");
}


/** [src_interrupt] */

/* end of file */
