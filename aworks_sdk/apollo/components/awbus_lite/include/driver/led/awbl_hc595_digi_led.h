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
 * \brief 基于hc595数码管的led驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 15-04-14  ops, first implementation.
 * \endinternal
 */

#ifndef AWBL_HC595_DIGI_LED_H_
#define AWBL_HC595_DIGI_LED_H_

#include "apollo.h"
#include "awbus_lite.h"

#include "awbl_led.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HC595_DIGI_LED_NAME      "hc595_digi_led"

struct awbl_led_seg8_tbl {

    /**
     * \brief LED编号, LED标准接口操作的ID号
     */
    uint16_t  led_id;

    /**
     * \brief LED所在的数码管编号
     */
    uint8_t   digi_id;

    /**
     * \brief LED所在的组编号 (一个数码管对应一个组)
     */
    uint8_t   grp_id;

    /**
     * \brief LED在该数码管内的编号
     */
    uint8_t   grp_led_id;

};


struct awbl_hc595_digi_led_param {

    /**
     * \brief led中间件服务信息
     */
    struct awbl_led_servinfo             led_servinfo;

    /**
     * \brief 解码表
     */
    aw_const struct awbl_led_seg8_tbl   *p_decode_tbl;

    /**
     * \brief LED显存
     */
    uint8_t                             *p_led_disp_buf;

    /**
     * \brief 驱动该LED设备的数码管设备号
     */
    uint16_t                             digitron_dev_id;

    /**
     * \brief 是否是低电平有效
     */
    aw_bool_t                               active_low;

};

struct awbl_hc595_digi_led_dev {

    struct awbl_hc595_digi_led_param *p_par;

    struct awbl_led_service           led_serv;

    aw_err_t (*pfn_hc595_led_set_by_digitron)(
                         struct awbl_hc595_digi_led_dev *p_this,
                         int                             id,
                         aw_bool_t                       on);

    aw_err_t (*pfn_hc595_led_toggle_by_digitron)(
                         struct awbl_hc595_digi_led_dev *p_this,
                         int                             id);

};

aw_local aw_inline aw_err_t awbl_hc595_digi_led_set (
    struct awbl_hc595_digi_led_dev *p_this,
    int                             id,
    aw_bool_t                       on)
{

    return p_this->pfn_hc595_led_set_by_digitron(p_this, id, on);
}

aw_local aw_inline aw_err_t awbl_hc595_digi_led_toggle (
    struct awbl_hc595_digi_led_dev *p_this,
    int                             id)
{

    return p_this->pfn_hc595_led_toggle_by_digitron(p_this, id);
}

void awbl_hc595_digi_led_dev_ctor (struct awbl_hc595_digi_led_dev    *p_this,
                                   struct awbl_hc595_digi_led_param  *p_par);


#ifdef __cplusplus
}
#endif

#endif /* AWBL_HC595_DIGI_LED_H_ */
