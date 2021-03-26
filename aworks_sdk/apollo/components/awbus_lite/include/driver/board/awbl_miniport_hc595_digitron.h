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
 * \brief MINIPORT HC595+LED ģ������
 *
 *
 * \par ʹ��ʾ��
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 17-06-19  nwt, first implementation.
 * \endinternal
 */

#ifndef AWBL_MINIPORT_HC595_DIGITRON_H_
#define AWBL_MINIPORT_HC595_DIGITRON_H_

#include "awbl_scanner.h"
#include "awbl_gpio_hc595.h"
#include "driver/board/awbl_matrix_key_lite.h"
#include "driver/digitron/awbl_gpio_hc595_digitron.h"
#include "driver/input/key/awbl_hc595_matrix_key.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MINIPORT_HC595_DIGITRON_NAME      "miniport_hc595_digitron"

struct awbl_miniport_hc595_digitron_param {

    /**
     * \brief hc595������豸���ò���(gpio�ܽ���Ϊcom����)
     */
    struct awbl_gpio_hc595_digitron_param    hc_ddpar;

    /**
     * \brief ɨ�����豸���ò���
     */
    struct awbl_scanner_param           scpar;

    /**
     * \brief hc595�豸���ò���
     */
    struct awbl_gpio_hc595_param        gpio_hc595_par;

};

struct awbl_miniport_hc595_digitron_dev {

    /**
     * \brief �̳���awbl_dev
     */
    struct awbl_dev                       dev;

    /**
     * \brief ����hc595������豸(gpio�ܽ���Ϊcom����)
     */
    struct awbl_gpio_hc595_digitron_dev   hc595_digitron;

    /**
     * \brief ����hc595�豸
     */
    struct awbl_gpio_hc595_dev           gpio_hc595_dev;

    /**
     * \brief ����ɨ�����豸
     */
    struct awbl_scanner_dev              scan_dev;

};

#ifdef __cplusplus
}
#endif


#endif /* AWBL_MINIPORT_HC595_DIGITRON_H_ */
