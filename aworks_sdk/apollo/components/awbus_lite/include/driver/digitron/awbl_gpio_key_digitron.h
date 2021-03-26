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
 * \brief 基于GPIO驱动的数码管类定义
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

#ifndef AWBL_GPIO_DIGITRON_H_
#define AWBL_GPIO_DIGITRON_H_

#include "awbl_digitron_if.h"
#include "awbl_gpio.h"


#ifdef __cplusplus
extern "C" {
#endif

struct awbl_gpio_key_digitron_param {

    /**
     * \brief 通用数码管配置参数
     */
    struct awbl_digitron_disp_param  ddparam;

    /** \brief 段码 GPIO驱动 引脚 */
    const uint16_t                   *p_seg_pins;  
    /** \brief 位码 GPIO驱动 引脚 */    
    const uint16_t                   *p_com_pins;   


    /**
     * \brief 控制数码管段码的gpio个数
     */
    uint8_t                          gpio_seg_nums;

    /**
     * \brief 控制数码管位码的gpio个数
     */
    uint8_t                          gpio_com_nums;


    /**
     * \brief 一次扫描数码管的个数
     */
    uint8_t                          scan_num_digitron;


    /**
     * \brief 扫描数据缓存
     */
    void                            *p_scan_data;


};

struct awbl_gpio_key_digitron_dev {

    /**
     * \brief 继承自通用数码管awbl_digitron_dev
     */
    struct awbl_digitron_dev          digitron_dev;


    /**
     * \brief gpio数码管配置参数
     */
    struct awbl_gpio_key_digitron_param  *p_par;

    /**
     * \brief 抽象数据发送接口
     */
    void (*pfn_data_send)(struct awbl_gpio_key_digitron_dev *p_this, uint8_t idx);

};

aw_local aw_inline void awbl_gpio_key_digitron_data_send(
    struct awbl_gpio_key_digitron_dev *p_this, uint8_t idx)
{
    p_this->pfn_data_send(p_this, idx);
}

void awbl_gpio_key_digitron_dev_ctor(
    struct awbl_gpio_key_digitron_dev    *p_this,
    struct awbl_gpio_key_digitron_param  *p_par,
    uint8_t                               disp_dev_id);

#ifdef __cplusplus
}
#endif


#endif /* AWBL_GPIO_DIGITRON_H_ */
