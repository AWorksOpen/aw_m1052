/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief ����gpio��led����
 *
 * \internal
 * \par modification history:
 * - 1.00 14-11-22  ops, first implementation.
 * \endinternal
 */
#ifndef AWBL_GPIO_LED_H_
#define AWBL_GPIO_LED_H_

#include "apollo.h"
#include "awbus_lite.h"
#include "aw_compiler.h"

#include "awbl_led.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GPIO_LED_NAME    "gpio_led"

struct awbl_led_gpio_param {

    const uint16_t             *p_led_gpios; /**< \brief led ��ż���Ӧgpio����*/
    struct awbl_led_servinfo   led_servinfo; /**< \brief led �м��������Ϣ */
    uint8_t                    num_leds;     /**< \brief led ���� */
    uint8_t                    active_low;   /**< \brief led ���� */

    void (*pfn_plfm_init)(void);             /**< \brief led ƽ̨��ʼ������ */
};

struct awbl_led_gpio_dev {

    struct awbl_dev          dev;
    struct awbl_led_service  led_serv;

};

#ifdef __cplusplus
}
#endif


#endif /* AWBL_LED_GPIO_H_ */
