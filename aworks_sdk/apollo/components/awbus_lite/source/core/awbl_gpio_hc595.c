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
 * \brief 基于GPIO驱动的74HC595通用设备类实现
 *
 * \internal
 * \par modification history:
 * - 1.00 14-12-13  ops, first implementation.
 * \endinternal
 */

#include "awbl_gpio_hc595.h"
#include "aw_gpio.h"
#include "aw_assert.h"

#define __THIS_TO_GPIO_HC595_DEV(p_this)   \
          AW_CONTAINER_OF(p_this, struct awbl_gpio_hc595_dev, hc595_dev);


aw_local void __gpio_hc595_lock_enable (struct aw_hc595_dev *p_this)
{
    struct awbl_gpio_hc595_dev *p_gh = __THIS_TO_GPIO_HC595_DEV(p_this);

    aw_gpio_set(p_gh->hc595_dev.hc595_pin.lock_pin, 0);
}

aw_local void __gpio_hc595_clock_enable (struct aw_hc595_dev *p_this)
{
    struct awbl_gpio_hc595_dev *p_gh = __THIS_TO_GPIO_HC595_DEV(p_this);

    aw_gpio_set(p_gh->hc595_dev.hc595_pin.clk_pin, 0);
}

aw_local void __gpio_hc595_shift (struct aw_hc595_dev *p_this, 
                                  uint8_t              bit_order,
                                  uint8_t              data)
{
    struct awbl_gpio_hc595_dev *p_gh = __THIS_TO_GPIO_HC595_DEV(p_this);

    uint8_t i = 0;

    for (i = 0; i < HC595_OUTPUT_PIN_NUMS; i++) {

        aw_gpio_set(p_gh->hc595_dev.hc595_pin.clk_pin, 0);

        if (bit_order == HC595_LSBFISRT) {

            aw_gpio_set(p_gh->hc595_dev.hc595_pin.data_pin, 
                        !!(data & (1 << i)));

        }else {

            aw_gpio_set(p_gh->hc595_dev.hc595_pin.data_pin, 
                        !!(data & (1 << (7 - i))));
        }

        aw_gpio_set(p_gh->hc595_dev.hc595_pin.clk_pin, 1);
    }
}

aw_local void __gpio_hc595_clock_disable (struct aw_hc595_dev *p_this)
{
    struct awbl_gpio_hc595_dev *p_gh = __THIS_TO_GPIO_HC595_DEV(p_this);

    aw_gpio_set(p_gh->hc595_dev.hc595_pin.clk_pin, 1);
}

aw_local void __gpio_hc595_lock_disable(struct aw_hc595_dev *p_this)
{
    struct awbl_gpio_hc595_dev *p_gh = __THIS_TO_GPIO_HC595_DEV(p_this);

    aw_gpio_set(p_gh->hc595_dev.hc595_pin.lock_pin, 1);
}

aw_local void __gpio_hc595_pin_init(struct aw_hc595_dev *p_this)
{
    struct awbl_gpio_hc595_dev *p_gh = __THIS_TO_GPIO_HC595_DEV(p_this);

    aw_gpio_pin_cfg(p_gh->hc595_dev.hc595_pin.data_pin, AW_GPIO_OUTPUT);
    aw_gpio_pin_cfg(p_gh->hc595_dev.hc595_pin.clk_pin , AW_GPIO_OUTPUT);
    aw_gpio_pin_cfg(p_gh->hc595_dev.hc595_pin.lock_pin, AW_GPIO_OUTPUT);
    aw_gpio_pin_cfg(p_gh->hc595_dev.hc595_pin.oe_pin  , AW_GPIO_OUTPUT);

    aw_gpio_set(p_gh->hc595_dev.hc595_pin.data_pin, 0);
    aw_gpio_set(p_gh->hc595_dev.hc595_pin.clk_pin , 0);
    aw_gpio_set(p_gh->hc595_dev.hc595_pin.lock_pin, 0);
    aw_gpio_set(p_gh->hc595_dev.hc595_pin.oe_pin  , 0);
}

aw_hc595_dev_t* awbl_gpio_hc595_dev_ctor (struct awbl_gpio_hc595_dev   *p_gh,
                                          struct awbl_gpio_hc595_param *p_par)
{

    aw_assert(p_gh);

    p_gh->hc595_dev.hc595_pin.data_pin = p_par->data_gpio_pin;
    p_gh->hc595_dev.hc595_pin.clk_pin  = p_par->clk_gpio_pin;
    p_gh->hc595_dev.hc595_pin.lock_pin = p_par->lock_gpio_pin;
    p_gh->hc595_dev.hc595_pin.oe_pin   = p_par->oe_gpio_pin;

    p_gh->hc595_dev.pfn_hc595_pin_init      = __gpio_hc595_pin_init;
    p_gh->hc595_dev.pfn_hc595_lock_enable   = __gpio_hc595_lock_enable;
    p_gh->hc595_dev.pfn_hc595_clock_enable  = __gpio_hc595_clock_enable;
    p_gh->hc595_dev.pfn_hc595_shift         = __gpio_hc595_shift;
    p_gh->hc595_dev.pfn_hc595_clock_disable = __gpio_hc595_clock_disable;
    p_gh->hc595_dev.pfn_hc595_lock_disable  = __gpio_hc595_lock_disable;

    return &p_gh->hc595_dev;
}

/* end of file */