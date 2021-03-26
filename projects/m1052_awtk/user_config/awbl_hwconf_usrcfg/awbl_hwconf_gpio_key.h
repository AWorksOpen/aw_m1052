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
 按键配置信息
*******************************************************************************/
#include "driver/input/key/awbl_gpio_key.h"
#include "aw_gpio.h"
#include "imx1050_pin.h"

/* 按键管脚和对应的按键值，最多支持32个按键 */
aw_const aw_local struct gpio_key_map __g_keymap[] =
{
    {GPIO1_11, KEY_1},
};

/* 按键设备信息 */
aw_local aw_const struct gpio_key_data __g_gpio_key_data = {
    NULL,                       /* KEY 初始化 */
    __g_keymap,                 /* 按键列表 */
    AW_TRUE,                       /* 按键按下，是否为低电平 */
    AW_NELEMENTS(__g_keymap),   /* number of keys */
    10,                         /* 使用中断时，扫描时间  ms */
    20,                         /* 按键扫描周期  ms */
    0                           /* 是否使用中断 */
};


/* 按键设备实例内存静态分配 */
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
