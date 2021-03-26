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
 * \brief gpio�����ж�����
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_COM_CONSOLE
 *      - AW_DEV_GPIO_LED
 *      - ��Ӧƽ̨�Ĵ��ں�
 *   2. �������ŵĴ�������ʱ�����öŰ��߽�  TEST_GPIO_0 �� TEST_GPIO_1 ��Ӧ����������������
 *   3. ��������ʹ�õ�LED�Ƶĵط�
 *
 * - ʵ������
 *  1. �����жϺ�LED����˸����ƽ����ģʽLED���ܲ�����˸����
 *  2. ���ڴ�ӡ������Ϣ��
 *
 *
 * \par Դ����
 * \snippet demo_gpio_trigger.c src_gpio_trigger
 *
 * \internal
 * \par Modification history
 * - 1.00 18-06-27  lqy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_gpio_trigger
 * \copydoc demo_gpio_trigger.c
 */

/** [src_gpio_trigger] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_gpio.h"
#include "aw_led.h"
#include "aw_vdebug.h"
#include "aw_demo_config.h"
#include "aw_sem.h"

/* ƽ̨GPIO���ɸ��ݾ���ƽ̨�޸� */
#define TEST_GPIO_0     DM_GPIO_TRG_OUTPUT
#define TEST_GPIO_1     DM_GPIO_TRG_INTR

// ����������ź��� sem
AW_MUTEX_DECL_STATIC(mutex);

/**\brief ��¼�жϽ���Ĵ��� */
aw_local uint8_t volatile __g_test_count = 0;

/* gpio ���Իص����� */
void test_gpio_trig_isr (void* arg);

void demo_gpio_trigger_entry (void)
{
    // ��ʼ���ź���
    AW_MUTEX_INIT(mutex, AW_SEM_Q_PRIORITY);

    uint32_t i   = 0;
    uint32_t ret = 0;

    aw_kprintf("\nGPIO trigger demo testing...\r\n");

    /* �����ж����Ų������������ */
    int test_gpios[] = {TEST_GPIO_0, TEST_GPIO_1};
    if (aw_gpio_pin_request("test_gpios",
                             test_gpios,
                             AW_NELEMENTS(test_gpios)) == AW_OK) {
        aw_gpio_pin_cfg(TEST_GPIO_0,
                        AW_GPIO_PUSH_PULL | AW_GPIO_OUTPUT_INIT_HIGH);
        aw_gpio_pin_cfg(TEST_GPIO_1, AW_GPIO_INPUT);
    }

    /* �����жϻص����� */
    ret = aw_gpio_trigger_connect(TEST_GPIO_1, test_gpio_trig_isr, 0);
    if (ret != AW_OK) {
        aw_kprintf("gpio trigger connect failed!\n");
        return;
    }

//#define TRIGGER_FLAG    AW_GPIO_TRIGGER_LOW         /* ����Ϊ�͵�ƽ���� */
//#define TRIGGER_FLAG    AW_GPIO_TRIGGER_HIGH        /* ����Ϊ�ߵ�ƽ���� */
#define TRIGGER_FLAG    AW_GPIO_TRIGGER_BOTH_EDGES  /* ����Ϊ˫���ش��� */
//#define TRIGGER_FLAG    AW_GPIO_TRIGGER_FALL        /* ����Ϊ�½��ش��� */
//#define TRIGGER_FLAG    AW_GPIO_TRIGGER_RISE        /* ����Ϊ�����ش��� */

    ret = aw_gpio_trigger_cfg(TEST_GPIO_1, TRIGGER_FLAG);
    if (ret != AW_OK) {
        aw_kprintf("gpio trigger cfg failed!\n");
        return;
    }

    /* ���ô����ж� */
    ret = aw_gpio_trigger_on(TEST_GPIO_1);
    if (ret != AW_OK) {
        aw_kprintf("gpio trigger on failed!\n");
        return;
    }

    for (i = 0; i < 19; i++) {
#if TRIGGER_FLAG == AW_GPIO_TRIGGER_LOW || TRIGGER_FLAG == AW_GPIO_TRIGGER_HIGH
        /* ���ڻص������йرյĴ����ж� */
        ret = aw_gpio_trigger_on(TEST_GPIO_1);
        if (ret != AW_OK) {
            aw_kprintf("gpio trigger on failed!\n");
            return;
        }
#endif
        aw_task_delay(500);
        aw_gpio_toggle(TEST_GPIO_0);
    }

    AW_MUTEX_LOCK(mutex,AW_SEM_WAIT_FOREVER);

    /* ��ӡ�����жϴ��� */
    AW_INFOF(("entry gpio int times: %d\r\n", __g_test_count));

    AW_MUTEX_UNLOCK(mutex);

    /* �Ͽ��жϻص����� */
    aw_gpio_trigger_disconnect(TEST_GPIO_1, test_gpio_trig_isr, 0);
    /* �رմ����ж� */
    aw_gpio_trigger_off(TEST_GPIO_1);

    /* �ͷ����� */
    aw_gpio_pin_release(test_gpios, AW_NELEMENTS(test_gpios));
    
    aw_kprintf("GPIO trigger demo exit...\r\n");
}

/* gpio ���Իص����� */
void test_gpio_trig_isr (void* arg)
{
#if TRIGGER_FLAG == AW_GPIO_TRIGGER_LOW || TRIGGER_FLAG == AW_GPIO_TRIGGER_HIGH
    /* �رմ����жϣ������ƽ����ʱ��ͣ�ؽ��жϵ��³����޷��������� */
    aw_gpio_trigger_off(TEST_GPIO_1);
#endif
    aw_led_toggle(0);

    AW_MUTEX_LOCK(mutex,AW_SEM_WAIT_FOREVER);

    __g_test_count++;

    AW_MUTEX_UNLOCK(mutex);


}

/** [src_gpio_trigger] */

/* end of file */
