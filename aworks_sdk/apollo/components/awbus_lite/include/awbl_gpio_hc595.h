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
 * \brief 基于GPIO驱动的74HC595通用设备类定义
 *
 *        该类用于描述基于GPIO驱动的74HC595设备, 继承自通用
 *        74HC595器件类awbl_hc595_dev.
 *
 * \par 使用示例
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 14-12-13  ops, first implementation.
 * \endinternal
 */

#ifndef AWBL_GPIO_HC595_H_
#define AWBL_GPIO_HC595_H_

#include "apollo.h"
#include "aw_types.h"
#include "aw_hc595.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief hc595配置参数
 */
struct awbl_gpio_hc595_param {

    uint16_t data_gpio_pin;
    uint16_t clk_gpio_pin;
    uint16_t lock_gpio_pin;
    uint16_t oe_gpio_pin;
};

/**
 * \brief 基于GPIO驱动的74HC595通用设备
 */
struct awbl_gpio_hc595_dev {

    /**
     * \brief 继承自抽象类awbl_hc595_dev
     */
    aw_hc595_dev_t               hc595_dev;

    struct awbl_gpio_hc595_param *p_par;
};

aw_hc595_dev_t* awbl_gpio_hc595_dev_ctor(struct awbl_gpio_hc595_dev   *p_gh,
                                         struct awbl_gpio_hc595_param *p_par);

#ifdef __cplusplus
}
#endif

#endif /* AWBL_HC595_H_ */
