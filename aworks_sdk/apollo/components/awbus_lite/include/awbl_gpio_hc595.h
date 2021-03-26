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
 * \brief ����GPIO������74HC595ͨ���豸�ඨ��
 *
 *        ����������������GPIO������74HC595�豸, �̳���ͨ��
 *        74HC595������awbl_hc595_dev.
 *
 * \par ʹ��ʾ��
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
 * \brief hc595���ò���
 */
struct awbl_gpio_hc595_param {

    uint16_t data_gpio_pin;
    uint16_t clk_gpio_pin;
    uint16_t lock_gpio_pin;
    uint16_t oe_gpio_pin;
};

/**
 * \brief ����GPIO������74HC595ͨ���豸
 */
struct awbl_gpio_hc595_dev {

    /**
     * \brief �̳��Գ�����awbl_hc595_dev
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
