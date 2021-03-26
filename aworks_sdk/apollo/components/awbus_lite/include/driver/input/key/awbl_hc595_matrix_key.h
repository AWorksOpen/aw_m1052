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
 * \brief 基于hc595驱动的矩阵按键类定义
 *
 * \internal
 * \par modification history
 * - 1.00 14-12-08  ops, first implementation.
 * \endinternal
 */
#ifndef AWBL_HC595_MATRIX_KEY_H_
#define AWBL_HC595_MATRIX_KEY_H_

#include "apollo.h"
#include "aw_hc595.h"

#ifdef __cplusplus
extern "C" {
#endif

struct awbl_hc595_matrix_key_param {

    /**
     * \brief 按键键码表
     */
    const uint16_t *key_code_tbl;

    /**
     * \brief 按键个数
     */
    uint8_t  key_nums;

    /**
     * \brief 按键极性
     */
    aw_bool_t   active_low;

    /**
     * \brief 是否与其他设备复用引脚
     */
    aw_bool_t   multi_io;

    /**
     * \brief Key Out引脚
     */
    uint16_t key_out_pin;

    /**
     * \brief hc595级联个数
     */
    uint8_t  hc595_nums;

    /**
     * \brief 位码数据缓冲区
     */
    uint8_t  *p_com_data;

    /**
     * \brief hc595排列表
     */
    const uint8_t *p_hc595_arrange;

};

struct awbl_hc595_matrix_key_dev {

    /**
     * \brief 关联抽象hc595设备
     */
    struct aw_hc595_dev                 *p_hc595_dev;

    /**
     * \brief hc595按键配置参数
     */
    struct awbl_hc595_matrix_key_param  *p_par;

    /**
     * \brief 检测计数
     */
    uint8_t  detect_cnt;

    /**
     * \brief 检测开始标记
     */
    uint8_t  detect_start;

    /**
     * \brief 当前检测结果
     */
    uint32_t detect_bits_state;

    /**
     * \brief 上一次检测的结果
     */
    uint32_t last_bits_state;

    /**
     * \brief 按键引脚初始化
     */
    void (*pfn_hc595_key_pin_gpio_init)(struct awbl_hc595_matrix_key_dev *p_this);

    /**
     * \brief 按键检测
     */
    void (*pfn_hc595_key_detect)(struct awbl_hc595_matrix_key_dev *p_this, 
                                 uint8_t                           idx);

};


aw_local aw_inline void awbl_hc595_matrix_key_detect(
    struct awbl_hc595_matrix_key_dev *p_this, uint8_t idx)
{
    p_this->pfn_hc595_key_detect(p_this, idx);
}

void awbl_hc595_matrix_key_dev_ctor(struct awbl_hc595_matrix_key_dev    *p_this,
                                    struct aw_hc595_dev                 *p_hc,
                                    struct awbl_hc595_matrix_key_param  *p_par);

#ifdef __cplusplus
}
#endif


#endif /* AWBL_HC595_MATRIX_KEY_H_ */
