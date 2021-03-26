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
 * \brief 基于HC595驱动的LED类实现
 *
 * \internal
 * \par modification history:
 * - 1.01 15-04-13  ops, add LED set implementation by using digitron if.
 * - 1.00 14-12-17  ops, first implementation.
 * \endinternal
 */
#ifndef AWBL_HC595_LED_H_
#define AWBL_HC595_LED_H_

#include "apollo.h"
#include "awbus_lite.h"

#include "aw_hc595.h"
#include "awbl_led.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HC595_LED_NAME      "hc595_led"

struct awbl_hc595_led_tbl {

    /**
     * \brief LED编号, LED标准接口操作的ID号
     */
    uint16_t  led_id;

    /**
     * \brief LED所在组的组编号
     */
    uint8_t   grp_id;

    /**
     * \brief LED在该组内的编号
     */
    uint8_t   grp_led_id;
};

struct awbl_hc595_led_param {

     /**
      * \brief led中间件服务信息
      */
    struct awbl_led_servinfo  led_servinfo;

    uint8_t                   hc595_com_nums;

    /**
     * \brief LED组排列的个数
     */
    uint8_t                   led_rank_nums;

    /**
     * \brief 每个排列中, LED组的个数
     */
    uint8_t                   leds_grp_per_rank_nums;

    /**
     * \brief LED是否是低电平有效
     */
    uint8_t                   led_active_low;

    /**
     * \brief 位选是否低电平有效
     */
    uint8_t                   com_active_low;

    /**
     * \brief LED数据缓冲区
     */
    uint8_t                   *p_led_data;

    /**
     * \brief 位码数据缓冲区
     */
    uint8_t                   *p_com_data;

    /**
     * \brief hc595 LED Code
     */
    aw_const struct awbl_hc595_led_tbl  *p_led_tbl;


    /**
     * \brief LED显存
     */
    uint8_t                   *p_led_disp_buf;

    /**
     * \brief hc595排列, 1: 段码, 0: 位码
     */
    aw_const uint8_t          *p_hc595_arrange;
};

/**
 * \brief 基于HC595驱动的LED类
 */
struct awbl_hc595_led_dev {

    struct awbl_hc595_led_param  *p_par;

    struct aw_hc595_dev               *p_hc595_dev;

    struct awbl_led_service           led_serv;

    aw_err_t (*pfn_hc595_led_set)(struct awbl_hc595_led_dev *p_this, 
                                  int                        id,
                                  aw_bool_t                  on);

    aw_err_t (*pfn_hc595_led_toggle) (struct awbl_hc595_led_dev *p_this, int id);

    void (*pfn_hc595_led_data_send) (struct awbl_hc595_led_dev *p_this,
                                     uint8_t                    scan_index,
                                     uint8_t                    led_rank_idx);

};

/**
 * \brief LED Set (Common)
 */
aw_local aw_inline aw_err_t awbl_hc595_led_set (
    struct awbl_hc595_led_dev *p_this, int id, aw_bool_t on)
{
    return p_this->pfn_hc595_led_set (p_this, id, on);
}

aw_local aw_inline void awbl_hc595_led_data_send (
    struct awbl_hc595_led_dev *p_this,
    uint8_t                    scan_index,
    uint8_t                    led_rank_idx)
{
    p_this->pfn_hc595_led_data_send(p_this, scan_index, led_rank_idx);
}

void awbl_hc595_led_dev_ctor(struct awbl_hc595_led_dev    *p_this,
                             struct aw_hc595_dev          *p_hc,
                             struct awbl_hc595_led_param  *p_par);

#ifdef __cplusplus
}
#endif


#endif /* AWBL_HC595_LED_H_ */
