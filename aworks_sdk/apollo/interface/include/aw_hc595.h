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
 * \brief ͨ��74HC595�����ඨ��
 *
 *        ������������һ��74HC595оƬ, ���ڳ�����. ���ݲ�ͬ��������ʽ, ������ʵ
 *        �ֶ���Ĳ���, ����, ʹ��GPIO����74HC595, ʹ��SPI����74HC595.
 *
 *  74HC595���ż��䶨��
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
 *       Q0 ~ Q7 :  �����������.
 *        GND(8) :  ��.
 *        Q7'(9) :  ������������(���뵽��һ��������74HC595).
 *      SCLR(10) :  ��SCLR�ǵ͵�ƽʱ, ����λ�Ĵ����е��������.
 *       SCK(11) :  ��λ�Ĵ�����������ʱ���ź�, ��λ������SCK��������.
 *       RCK(12) :  ����Ĵ�������ʱ���ź�.
 *        OE(13) :  ���ʹ��, �͵�ƽ��Ч.
 *       SDI(14) :  ������������.
 *           VCC :  2 ~ 5v.
 *
 * \par ʹ��ʾ��
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

/** \brief �������λ�� */
#define  HC595_OUTPUT_PIN_NUMS   8

/** \brief �ӵ�λ��ʼ��λ */
#define  HC595_LSBFISRT   0

/** \brief �Ӹ�λ��ʼ��λ */
#define  HC595_MSBFISRT   1

/**
 * \brief hc595��������
 */
struct aw_hc595_param {

    uint16_t data_pin;  /**< \brief �������� */
    uint16_t clk_pin;   /**< \brief ʱ������ */
    uint16_t lock_pin;  /**< \brief �������� */
    uint16_t oe_pin;    /**< \brief ���ʹ������ */
};

/**
 * \brief ͨ��74HC595����
 */
typedef struct aw_hc595_dev {

    struct aw_hc595_param  hc595_pin; /**< \brief hc595�������� */

    /**
     * \brief hc595�������ų�ʼ���ӿ�
     */
    void (*pfn_hc595_pin_init)(struct aw_hc595_dev *p_this);

    /**
     * \brief hc595��������(LOCK)ʹ�ܽӿ�
     */
    void (*pfn_hc595_lock_enable) (struct aw_hc595_dev *p_this);

    /**
     * \brief hc595������������(CLK)ʹ�ܽӿ�
     */
    void (*pfn_hc595_clock_enable) (struct aw_hc595_dev *p_this);

    /**
     * \brief hc595������λ�����ӿ�
     */
    void (*pfn_hc595_shift) (struct aw_hc595_dev *p_this, 
                             uint8_t              bit_order,
                             uint8_t              data);

    /**
     * \brief hc595������������(CLK)���ܽӿ�
     */
    void (*pfn_hc595_clock_disable) (struct aw_hc595_dev *p_this);

    /**
     * \brief hc595��������(LOCK)���ܽӿ�
     */
    void (*pfn_hc595_lock_disable)(struct aw_hc595_dev *p_this);

    /**
     * \brief hc595����OE(Output Enable)ʹ�ܽӿ�
     */
    void (*pfn_hc595_oe_enable)(struct aw_hc595_dev *p_this);

    /**
     * \brief hc595����OE(Output Enable)���ܽӿ�
     */
    void (*pfn_hc595_oe_disable)(struct aw_hc595_dev *p_this);

}aw_hc595_dev_t;

/**
 * \brief hc595�������ų�ʼ���ӿ�
 *
 * \param p_this   ͨ��hc595����.
 */
aw_local aw_inline void aw_hc595_pin_init(aw_hc595_dev_t *p_this)
{
    p_this->pfn_hc595_pin_init(p_this);
}

/**
 * \brief hc595��������(LOCK)ʹ�ܽӿ�
 *
 * \param  p_this   ͨ��hc595����.
 */
aw_local aw_inline void aw_hc595_lock_enable(aw_hc595_dev_t *p_this)
{
    p_this->pfn_hc595_lock_enable(p_this);
}

/**
 * \brief hc595������������(CLK)ʹ�ܽӿ�
 *
 * \param  p_this   ͨ��hc595����.
 */
aw_local aw_inline void aw_hc595_clock_enable(aw_hc595_dev_t *p_this)
{
    p_this->pfn_hc595_clock_enable(p_this);
}

/**
 * \brief hc595������λ�����ӿ�
 *
 *        ÿִ��һ����λ�����ӿ�, ��ָ�����ݵ�ĳһλ������λ�Ĵ���.
 *
 * \param  p_this     ͨ��hc595����.
 * \param  bit_order  ��λ����.
 *                    MSBFISRT : �������Чλ��ʼ��λ(�����).
 *                    LSBFISRT : �������Чλ��ʼ��λ(���ұ�).
 * \param  bit        ����λ����.
 */
aw_local aw_inline void aw_hc595_shift (aw_hc595_dev_t *p_this, 
                                        uint8_t         bit_order,
                                        uint8_t         data)
{
    p_this->pfn_hc595_shift(p_this, bit_order, data);
}

/**
 * \brief hc595������������(CLK)���ܽӿ�
 *
 * \param  p_this   ͨ��hc595����.
 */
aw_local aw_inline void aw_hc595_clock_disable(aw_hc595_dev_t *p_this)
{
    p_this->pfn_hc595_clock_disable(p_this);
}

/**
 * \brief hc595��������(LOCK)���ܽӿ�
 *
 * \param  p_this   ͨ��hc595����.
 */
aw_local aw_inline void aw_hc595_lock_disable(aw_hc595_dev_t *p_this)
{
    p_this->pfn_hc595_lock_disable(p_this);
}

/**
 * \brief hc595����OE(Output Enable)ʹ�ܽӿ�
 */
aw_local aw_inline void aw_hc595_oe_enable(struct aw_hc595_dev *p_this)
{
    p_this->pfn_hc595_oe_enable(p_this);
}

/**
 * \brief hc595����OE(Output Enable)���ܽӿ�
 */
aw_local aw_inline void aw_hc595_oe_disable(struct aw_hc595_dev *p_this)
{
    p_this->pfn_hc595_oe_disable(p_this);
}

#ifdef __cplusplus
}
#endif

#endif /* AWBL_HC595_H_ */
