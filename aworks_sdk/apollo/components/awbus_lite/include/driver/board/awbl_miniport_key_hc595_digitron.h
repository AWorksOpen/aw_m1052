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

#ifndef AWBL_MINIPORT_KEY_HC595_DIGITRON_H_
#define AWBL_MINIPORT_KEY_HC595_DIGITRON_H_

#include "awbl_scanner.h"
#include "awbl_gpio_hc595.h"
#include "driver/digitron/awbl_gpio_hc595_digitron.h"
#include "driver/input/key/awbl_matrix_keypad.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MINIPORT_KEY_HC595_DIGITRON_NAME      "miniport_key_hc595_digitron"

#define   __KEY_ROW_NUM     16
#define   __KEY_COL_NUM     16


struct awbl_miniport_key_hc595_digitron_param {

    /**
     * \brief hc595数码管设备配置参数(gpio管脚作为com引脚)
     */
    struct awbl_gpio_hc595_digitron_param    hc_ddpar;

    /**
     * \brief 扫描器设备配置参数
     */
    struct awbl_scanner_param           scpar;

    /**
     * \brief hc595设备配置参数
     */
    struct awbl_gpio_hc595_param        gpio_hc595_par;

};

struct awbl_miniport_key_hc595_digitron_dev {

    /**
     * \brief 继承自awbl_dev
     */
    struct awbl_dev                       dev;

    /**
     * \brief 关联hc595数码管设备(gpio管脚作为com引脚)
     */
    struct awbl_gpio_hc595_digitron_dev   hc595_digitron;

    /**
     * \brief 关联hc595设备
     */
    struct awbl_gpio_hc595_dev            gpio_hc595_dev;

    /**
     * \brief 关联扫描器设备
     */
    struct awbl_scanner_dev               scan_dev;

};


#ifdef __cplusplus
}
#endif


#endif /* AWBL_MINIPORT_KEY_HC595_DIGITRON_H_ */
