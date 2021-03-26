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
 * \brief TinyHMI(v1.02)模块驱动
 *
 *
 * \par 使用示例
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 14-12-08  ops, first implementation.
 * \endinternal
 */

#ifndef AWBL_TINY_HMI_H_
#define AWBL_TINY_HMI_H_

#include "awbl_scanner.h"
#include "awbl_gpio_hc595.h"

#include "driver/digitron/awbl_hc595_digitron.h"
#include "driver/input/key/awbl_hc595_matrix_key.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TINY_HMI_BOARD_NAME      "tiny_hmi_board"

struct awbl_tiny_hmi_param {

    /**
     * \brief hc595数码管设备配置参数
     */
    struct awbl_hc595_digitron_param   hc_ddpar;


    struct awbl_hc595_matrix_key_param hc_keypar;

    /**
     * \brief 扫描器设备配置参数
     */
    struct awbl_scanner_param         scpar;

    /**
     * \brief hc595设备配置参数
     */
    struct awbl_gpio_hc595_param      hcpar;

};

struct awbl_tiny_hmi_dev {

    /**
     * \brief 继承自awbl_dev
     */
    struct awbl_dev                  dev;

    /**
     * \brief 关联hc595数码管设备
     */
    struct awbl_hc595_digitron_dev   hc595_digitron;

    /**
     * \brief 关联hc595矩阵按键设备
     */
    struct awbl_hc595_matrix_key_dev hc595_key;

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


#endif /* AWBL_TINY_HMI_H_ */
