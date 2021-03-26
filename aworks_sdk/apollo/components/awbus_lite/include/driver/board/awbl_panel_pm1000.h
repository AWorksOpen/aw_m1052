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
 * \brief ���ʼ�ǰ���PM1000����
 *
 *
 * \par ʹ��ʾ��
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
     * \brief hc595������豸���ò���
     */
    struct awbl_hc595_digitron_param   hc_ddpar;

    /**
     * \brief hc595���󰴼��豸���ò���
     */
    struct awbl_hc595_matrix_key_param hc_keypar;

    /**
     * \brief ����hc595����ܵ�LED�豸���ò���
     */
    struct awbl_hc595_digi_led_param   digi_ledpar;

    /**
     * \brief ɨ�����豸���ò���
     */
    struct awbl_scanner_param          scpar;

    /**
     * \brief hc595�豸���ò���
     */
    struct awbl_gpio_hc595_param       hcpar;
};



struct awbl_panel_pm1000_dev {

    /**
     * \brief �̳���awbl_dev
     */
    struct awbl_dev                dev;

    /**
     * \brief ����hc595������豸
     */
    struct awbl_hc595_digitron_dev   hc595_digitron;

    /**
     * \brief ����hc595���󰴼��豸
     */
    struct awbl_hc595_matrix_key_dev hc595_matrix_key;

    /**
     * \brief ��������hc595����ܵ�LED�豸
     */
    struct awbl_hc595_digi_led_dev   digi_led;

    /**
     * \brief ����hc595�豸
     */
    struct awbl_gpio_hc595_dev       gpio_hc595_dev;

    /**
     * \brief ����ɨ�����豸(����ɨ��hc595�����)
     */
    struct awbl_scanner_dev          scan_dev;

    /**
     * \brief Panel����
     */
    struct awbl_panel_pm1000_param   *p_par;

};

#ifdef __cplusplus
}
#endif

#endif /* AWBL_PANEL_PM1000_H_ */
