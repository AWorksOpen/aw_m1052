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

#ifndef __AWBL_HWCONF_GPIO_KEY_H
#define __AWBL_HWCONF_GPIO_KEY_H

#ifdef AW_DEV_GPIO_KEY

/*******************************************************************************
 ����������Ϣ
*******************************************************************************/
#include "driver/input/key/awbl_gpio_key.h"
#include "aw_gpio.h"
#include "imx1050_pin.h"

/* �����ܽźͶ�Ӧ�İ���ֵ�����֧��32������ */
aw_const aw_local struct gpio_key_map __g_keymap[] =
{
    {GPIO1_11, KEY_1},
};

/* �����豸��Ϣ */
aw_local aw_const struct gpio_key_data __g_gpio_key_data = {
    NULL,                       /* KEY ��ʼ�� */
    __g_keymap,                 /* �����б� */
    AW_TRUE,                       /* �������£��Ƿ�Ϊ�͵�ƽ */
    AW_NELEMENTS(__g_keymap),   /* number of keys */
    10,                         /* ʹ���ж�ʱ��ɨ��ʱ��  ms */
    20,                         /* ����ɨ������  ms */
    0                           /* �Ƿ�ʹ���ж� */
};


/* �����豸ʵ���ڴ澲̬���� */
aw_local struct gpio_key_dev __g_gpio_key;

#define AWBL_HWCONF_GPIO_KEY   \
    {                          \
        GPIO_KEY_DEV_NAME,     \
        0,                     \
        AWBL_BUSID_PLB,        \
        0,                     \
        &(__g_gpio_key.dev),   \
        &(__g_gpio_key_data)   \
    },

#else
#define AWBL_HWCONF_GPIO_KEY

#endif  /* AW_DEV_GPIO_KEY */

#endif  /* __AWBL_HWCONF_GPIO_KEY_H */

/* end of file */
