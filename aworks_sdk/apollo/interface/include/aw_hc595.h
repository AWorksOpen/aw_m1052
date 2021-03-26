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
 * \brief 通用74HC595器件类定义
 *
 *        该类用于描述一块74HC595芯片, 属于抽象类. 根据不同的驱动方式, 由子类实
 *        现对其的操作, 例如, 使用GPIO驱动74HC595, 使用SPI驱动74HC595.
 *
 *  74HC595引脚及其定义
 *
 *               -------------
 *               | O         |
 *          Q1 --|1        16|-- VCC
 *          Q2 --|2        15|-- Q0
 *          Q3 --|3        14|-- SDI    (DATA Pin)
 *          Q4 --|4   595  13|-- OE     (OE Pin)
 *          Q5 --|5        12|-- RCK    (LOCK Pin)
 *          Q6 --|6        11|-- SCK    (CLOCK Pin)
 *          Q7 --|7        10|-- SCLR
 *         GND --|8         9|-- Q7'
 *               |           |
 *               -------------
 *
 *       Q0 ~ Q7 :  并行数据输出.
 *        GND(8) :  地.
 *        Q7'(9) :  串行数据输入(输入到下一个级联的74HC595).
 *      SCLR(10) :  当SCLR是低电平时, 将移位寄存器中的内容清除.
 *       SCK(11) :  移位寄存器串行输入时钟信号, 移位发生在SCK的上升沿.
 *       RCK(12) :  输出寄存器锁存时钟信号.
 *        OE(13) :  输出使能, 低电平有效.
 *       SDI(14) :  串行数据输入.
 *           VCC :  2 ~ 5v.
 *
 * \par 使用示例
 * \code
 *
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.01 14-12-12  ops, redefine the 74HC595 device.
 * - 1.00 14-11-27  ops, first implementation.
 * \endinternal
 */

#ifndef AWBL_HC595_H_
#define AWBL_HC595_H_

#include "apollo.h"
#include "aw_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief 并行输出位数 */
#define  HC595_OUTPUT_PIN_NUMS   8

/** \brief 从低位开始移位 */
#define  HC595_LSBFISRT   0

/** \brief 从高位开始移位 */
#define  HC595_MSBFISRT   1

/**
 * \brief hc595操作引脚
 */
struct aw_hc595_param {

    uint16_t data_pin;  /**< \brief 数据引脚 */
    uint16_t clk_pin;   /**< \brief 时钟引脚 */
    uint16_t lock_pin;  /**< \brief 锁存引脚 */
    uint16_t oe_pin;    /**< \brief 输出使能引脚 */
};

/**
 * \brief 通用74HC595器件
 */
typedef struct aw_hc595_dev {

    struct aw_hc595_param  hc595_pin; /**< \brief hc595引脚配置 */

    /**
     * \brief hc595器件引脚初始化接口
     */
    void (*pfn_hc595_pin_init)(struct aw_hc595_dev *p_this);

    /**
     * \brief hc595器件锁存(LOCK)使能接口
     */
    void (*pfn_hc595_lock_enable) (struct aw_hc595_dev *p_this);

    /**
     * \brief hc595器件串行输入(CLK)使能接口
     */
    void (*pfn_hc595_clock_enable) (struct aw_hc595_dev *p_this);

    /**
     * \brief hc595器件移位操作接口
     */
    void (*pfn_hc595_shift) (struct aw_hc595_dev *p_this, 
                             uint8_t              bit_order,
                             uint8_t              data);

    /**
     * \brief hc595器件串行输入(CLK)禁能接口
     */
    void (*pfn_hc595_clock_disable) (struct aw_hc595_dev *p_this);

    /**
     * \brief hc595器件锁存(LOCK)禁能接口
     */
    void (*pfn_hc595_lock_disable)(struct aw_hc595_dev *p_this);

    /**
     * \brief hc595器件OE(Output Enable)使能接口
     */
    void (*pfn_hc595_oe_enable)(struct aw_hc595_dev *p_this);

    /**
     * \brief hc595器件OE(Output Enable)禁能接口
     */
    void (*pfn_hc595_oe_disable)(struct aw_hc595_dev *p_this);

}aw_hc595_dev_t;

/**
 * \brief hc595器件引脚初始化接口
 *
 * \param p_this   通用hc595对象.
 */
aw_local aw_inline void aw_hc595_pin_init(aw_hc595_dev_t *p_this)
{
    p_this->pfn_hc595_pin_init(p_this);
}

/**
 * \brief hc595器件锁存(LOCK)使能接口
 *
 * \param  p_this   通用hc595对象.
 */
aw_local aw_inline void aw_hc595_lock_enable(aw_hc595_dev_t *p_this)
{
    p_this->pfn_hc595_lock_enable(p_this);
}

/**
 * \brief hc595器件串行输入(CLK)使能接口
 *
 * \param  p_this   通用hc595对象.
 */
aw_local aw_inline void aw_hc595_clock_enable(aw_hc595_dev_t *p_this)
{
    p_this->pfn_hc595_clock_enable(p_this);
}

/**
 * \brief hc595器件移位操作接口
 *
 *        每执行一次移位操作接口, 将指定数据的某一位移入移位寄存器.
 *
 * \param  p_this     通用hc595对象.
 * \param  bit_order  移位方向.
 *                    MSBFISRT : 从最高有效位开始移位(最左边).
 *                    LSBFISRT : 从最低有效位开始移位(最右边).
 * \param  bit        待移位数据.
 */
aw_local aw_inline void aw_hc595_shift (aw_hc595_dev_t *p_this, 
                                        uint8_t         bit_order,
                                        uint8_t         data)
{
    p_this->pfn_hc595_shift(p_this, bit_order, data);
}

/**
 * \brief hc595器件串行输入(CLK)禁能接口
 *
 * \param  p_this   通用hc595对象.
 */
aw_local aw_inline void aw_hc595_clock_disable(aw_hc595_dev_t *p_this)
{
    p_this->pfn_hc595_clock_disable(p_this);
}

/**
 * \brief hc595器件锁存(LOCK)禁能接口
 *
 * \param  p_this   通用hc595对象.
 */
aw_local aw_inline void aw_hc595_lock_disable(aw_hc595_dev_t *p_this)
{
    p_this->pfn_hc595_lock_disable(p_this);
}

/**
 * \brief hc595器件OE(Output Enable)使能接口
 */
aw_local aw_inline void aw_hc595_oe_enable(struct aw_hc595_dev *p_this)
{
    p_this->pfn_hc595_oe_enable(p_this);
}

/**
 * \brief hc595器件OE(Output Enable)禁能接口
 */
aw_local aw_inline void aw_hc595_oe_disable(struct aw_hc595_dev *p_this)
{
    p_this->pfn_hc595_oe_disable(p_this);
}

#ifdef __cplusplus
}
#endif

#endif /* AWBL_HC595_H_ */
