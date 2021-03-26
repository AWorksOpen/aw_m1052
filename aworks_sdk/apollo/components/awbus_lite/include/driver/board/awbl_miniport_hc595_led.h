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
 * \brief MINIPORT HC595+LED 模块驱动
 *
 *
 * \par 使用示例
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 17-06-19  nwt, first implementation.
 * \endinternal
 */

#ifndef AWBL_MINIPORT_HC595_LED_H_
#define AWBL_MINIPORT_HC595_LED_H_

#include "driver/led/awbl_gpio_led.h"
#include "awbl_gpio_hc595.h"
#include "awbl_led.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MINIPORT_HC595_LED_NAME      "miniport_hc595_led"

struct awbl_miniport_hc595_led_param {

    struct awbl_led_servinfo   led_servinfo;   /**< \brief led 中间件服务信息*/

    uint8_t                    num_leds;       /**< \brief led 数量*/
    uint8_t                    active_low;     /**< \brief led 极性*/

    /**
     * \brief hc595设备配置参数
     */
    struct awbl_gpio_hc595_param   gpio_hc595_par;

};

struct awbl_miniport_hc595_led_dev {

    /**
     * \brief 继承自awbl_led_gpio_dev
     */
    struct awbl_led_gpio_dev gpio_led_dev;

    /**
     * \brief 关联hc595设备
     */
    struct awbl_gpio_hc595_dev    gpio_hc595_dev;

    /**
     * \brief hc595上一次引脚的状态
     */
    uint8_t hc595_laststatus;
};

#ifdef __cplusplus
}
#endif


#endif /* AWBL_MINIPORT_HC595_LED_H_ */
