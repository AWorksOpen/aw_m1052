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
 * \brief GPIO����
 *
 * - ������
 *   1. ����������aw_prj_params.hͷ�ļ��н���
 *      - AW_DEV_GPIO_LED
 *
 * - ʵ������
 *   1. ������LED��1s��������˸5�Σ�
 *   2. ������LED��0.2s��������˸4s��Ȼ��ֹͣ��˸��
 *
 * - ��ע�� ��������ڳ�ʼ��ʱ������鿴��demo��ʹ��GPIO�Ƿ�������Ÿ��á�
 *
 * \par Դ����
 * \snippet demo_gpio.c src_gpio
 *
 * \internal
 * \par Modification History
 * - 1.00 18-02-26  hsg, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_gpio GPIO����
 * \copydoc demo_gpio.c
 */

/** [src_gpio] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_gpio.h"
#include "aw_led.h"
#include "aw_vdebug.h"
#include "aw_demo_config.h"


#define GPIO_LED     DM_GPIO_LED

/**
 * \brief GPIO demo ���
 * \return ��
 */
void demo_gpio_entry (void)
{
    int i = 0;

    aw_kprintf("\nGPIO demo testing...\r\n");

    int gpio_led_test[] = {GPIO_LED};
    if (aw_gpio_pin_request("gpio_led_test",
                             gpio_led_test,
                             AW_NELEMENTS(gpio_led_test)) == AW_OK) {

        /* GPIO �������� */
        aw_gpio_pin_cfg(GPIO_LED, AW_GPIO_OUTPUT | AW_GPIO_PULL_UP);
    }

    /* LED��1s��������˸5�� */
    for (i = 0; i < 5; i++) {
        aw_gpio_set(GPIO_LED, 0);

        aw_mdelay(500);

        aw_gpio_set(GPIO_LED, 1);

        aw_mdelay(500);
    }

    /* LED��0.2s�����ڳ�����˸ */
    for (i = 0; i < 40; i++) {
        aw_gpio_toggle(GPIO_LED);
        aw_mdelay(100);
    }

    aw_gpio_pin_release(gpio_led_test, AW_NELEMENTS(gpio_led_test));
    
    aw_kprintf("\nGPIO demo exit...\r\n");
}

/** [src_gpio] */

/* end of file */
