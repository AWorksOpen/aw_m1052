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
 * \brief APL_D01(v1.00)模块驱动
 *
 *
 * \par 使用示例
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 14-12-19  ops, first implementation.
 * \endinternal
 */

#ifndef AWBL_APL_D01_H_
#define AWBL_APL_D01_H_

#include "awbl_scanner.h"
#include "awbl_gpio_hc595.h"

#include "driver/digitron/awbl_hc595_digitron.h"
#include "driver/input/key/awbl_hc595_matrix_key.h"

#ifdef __cplusplus
extern "C" {
#endif

#define APL_D01_BOARD_NAME      "apl_d01_board"

struct awbl_apl_d01_param {

    /**
     * \brief hc595数码管设备配置参数
     */
    struct awbl_hc595_digitron_param    hc_ddpar;

    /**
     * \brief 按键设备配置参数
     */
    struct awbl_hc595_matrix_key_param  ma_keypar;

    /**
     * \brief 扫描器设备配置参数
     */
    struct awbl_scanner_param           scpar;

    /**
     * \brief hc595设备配置参数
     */
    struct awbl_gpio_hc595_param        hcpar;

};

struct awbl_apl_d01_dev {

    /**
     * \brief 继承自awbl_dev
     */
    struct awbl_dev                  dev;

    /**
     * \brief 关联hc595数码管设备
     */
    struct awbl_hc595_digitron_dev   hc595_digitron;

    /**
     * \brief 关联与其他设备复用hc595引脚的按键设备
     */
    struct awbl_hc595_matrix_key_dev  matrix_key;

    /**
     * \brief 关联hc595设备
     */
    struct awbl_gpio_hc595_dev       gpio_hc595_dev;

    /**
     * \brief 关联扫描器设备
     */
    struct awbl_scanner_dev          scan_dev;
};

#ifdef __cplusplus
}
#endif


#endif /* AWBL_APL_D01_H_ */
