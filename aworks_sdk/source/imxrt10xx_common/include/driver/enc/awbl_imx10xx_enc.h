
/******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      aworks.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx Quadrature Decoder driver
 *
 * \internal
 * \par Modification History
 * - 1.00 2018-03-20  mex, first implementation.
 * \endinternal
 */

#ifndef __AWBL_IMX10xx_ENC_H
#define __AWBL_IMX10xx_ENC_H

#include "aworks.h"
#include "aw_gpio.h"
#include "awbus_lite.h"
#include "aw_sem.h"
#include "aw_gpio.h"
#include "driver/enc/awbl_enc_service.h"


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/** \brief iMX RT10xx ENC ������ */
#define AWBL_IMX10XX_ENC_NAME            "imx10xx_enc"

typedef enum __enc_index_trigger_mode
{

    /** \brief INDEX signal's trigger is disabled. */
    enc_index_trigger_disable = 0,

    /** \brief Use positive going edge-to-trigger initialization of position counters. */
    enc_index_trigger_rising_edge,

    /** \brief Use negative going edge-to-trigger initialization of position counters. */
    enc_index_trigger_falling_edge,

} enc_index_trigger_mode_t;



/**
 * \brief iMX RT10xx ENC �豸��Ϣ
 */
typedef struct awbl_imx10xx_enc_devinfo {

    uint32_t            regbase;    /**< \brief �Ĵ�������ַ */
    uint8_t             int_num;    /**< \brief �жϺ� */

    int                 chan;        /** \brief ��ǰͨ���� */

    /** \brief index�������� */
    enc_index_trigger_mode_t trigger_mode;

    /** \brief  PHASE��PHASEB��INDEX�����ź��˲�����������,���÷�Χ0-7 */
    uint8_t             filter_count;

    /** \brief  �����ź��˲����������ڣ����÷�Χ0-255
     *   �˲�����ʱ���㷽����
     *   DELAY (IPBus clock cycles) = filter_sample_priod * (filter_count+3) + 2
     */
    uint16_t            filter_sample_priod;

    void (*pfn_plfm_init) (void);   /**< \brief ƽ̨��ʼ������ */
} awbl_imx10xx_enc_devinfo_t;



/**
 * \brief iMX RT10xx ENC �豸ʵ��
 */
typedef struct awbl_imx10xx_enc_dev {

    struct awbl_dev         super;       /**< \brief �̳��� AWBus �豸 */
    struct awbl_enc_service enc_serv;    /**< \brief �豸���� */

} awbl_imx10xx_enc_dev_t;

/**
 * \brief iMX RT10xx ENC ����ע��
 */
void awbl_imx10xx_enc_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX10xx_ENC_H */

/* end of file */
