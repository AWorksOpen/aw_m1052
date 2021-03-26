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
 * \brief 功率计前面板PM1000驱动
 *
 *
 * \par 使用示例
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 14-12-05  ops, first implementation.
 * \endinternal
 */
#ifndef AWBL_PANEL_PM1000_H_
#define AWBL_PANEL_PM1000_H_

#include "awbl_scanner.h"
#include "awbl_gpio_hc595.h"

#include "driver/board/awbl_panel_pm1000_def.h"
#include "driver/digitron/awbl_hc595_digitron.h"
#include "driver/input/key/awbl_hc595_matrix_key.h"
#include "driver/led/awbl_hc595_digi_led.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PANEL_BOARD_PM1000_NAME      "panel_board_pm1000"

struct awbl_panel_pm1000_param {

    /**
     * \brief hc595数码管设备配置参数
     */
    struct awbl_hc595_digitron_param   hc_ddpar;

    /**
     * \brief hc595矩阵按键设备配置参数
     */
    struct awbl_hc595_matrix_key_param hc_keypar;

    /**
     * \brief 基于hc595数码管的LED设备配置参数
     */
    struct awbl_hc595_digi_led_param   digi_ledpar;

    /**
     * \brief 扫描器设备配置参数
     */
    struct awbl_scanner_param          scpar;

    /**
     * \brief hc595设备配置参数
     */
    struct awbl_gpio_hc595_param       hcpar;
};



struct awbl_panel_pm1000_dev {

    /**
     * \brief 继承自awbl_dev
     */
    struct awbl_dev                dev;

    /**
     * \brief 关联hc595数码管设备
     */
    struct awbl_hc595_digitron_dev   hc595_digitron;

    /**
     * \brief 关联hc595矩阵按键设备
     */
    struct awbl_hc595_matrix_key_dev hc595_matrix_key;

    /**
     * \brief 关联基于hc595数码管的LED设备
     */
    struct awbl_hc595_digi_led_dev   digi_led;

    /**
     * \brief 关联hc595设备
     */
    struct awbl_gpio_hc595_dev       gpio_hc595_dev;

    /**
     * \brief 关联扫描器设备(用于扫描hc595数码管)
     */
    struct awbl_scanner_dev          scan_dev;

    /**
     * \brief Panel参数
     */
    struct awbl_panel_pm1000_param   *p_par;

};

#ifdef __cplusplus
}
#endif

#endif /* AWBL_PANEL_PM1000_H_ */
