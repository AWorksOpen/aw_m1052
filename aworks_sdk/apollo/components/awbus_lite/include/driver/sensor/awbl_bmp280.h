/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief BMP280 ��ѹ����������
 *

 *
 * \par 1.������Ϣ
 *
 *  - ������:   "bmp280_pressure"
 *  - ��������:   AWBL_BUSID_I2C
 *  - �豸��Ϣ:   struct awbl_bmp280_par
 *
 * \par 2.�����豸
 *
 * \par 3.�豸����/����
 *
 *  - \ref grp_awbl_bmp280_hwconf
 *
 * \par 4.�û��ӿ�
 *
 *  - \ref grp_aw_serv_bmp280
 *
 * \internal
 * \par modification history
 * - 1.00 18-10-16  dsg, first implementation.
 * \endinternal
 */

#ifndef __AWBL_BMP280_H
#define __AWBL_BMP280_H

/**
 * \addtogroup grp_awbl_if_bmp280
 * \copydoc awbl_bmp280.h
 * @{
 */

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_sensor.h"
#include "aw_sensor.h"
#include "aw_i2c.h"
#include "aw_common.h"

#define AWBL_BMP280_NAME "sensor-bmp280"

/**
 * \brief BMP280 �������豸�ṹ��
 */
typedef struct awbl_bmp280_param {
    uint8_t  start_id; /**< \brief ��������ʼID (ͨ��) */
    uint16_t addr;     /**< \brief �������ӻ���ַ */
} awbl_bmp280_param_t;

/**
 * \brief bmp280 �豸У׼ϵ���ṹ��
 */
typedef struct awbl_bmp280_cali {
    uint16_t   t1;   /**< \brief У׼ϵ��T1 */
    int16_t    t2;   /**< \brief У׼ϵ��T2 */
    int16_t    t3;   /**< \brief У׼ϵ��T3 */

    uint16_t   p1;   /**< \brief У׼ϵ��P1 */
    int16_t    p2;   /**< \brief У׼ϵ��P2 */
    int16_t    p3;   /**< \brief У׼ϵ��P3 */
    int16_t    p4;   /**< \brief У׼ϵ��P4 */
    int16_t    p5;   /**< \brief У׼ϵ��P5 */
    int16_t    p6;   /**< \brief У׼ϵ��P6 */
    int16_t    p7;   /**< \brief У׼ϵ��P7 */
    int16_t    p8;   /**< \brief У׼ϵ��P8 */
    int16_t    p9;   /**< \brief У׼ϵ��P9 */

    int32_t    up;   /**< \brief δУ׼��ѹֵ UP */
    int32_t    ut;   /**< \brief δУ׼�¶�ֵ UT */
} awbl_bmp280_cali_t;

/**
 * \brief BMP280 �������豸��Ϣ�ṹ��
 */
typedef struct awbl_bmp280_dev {
    /** \brief �̳��� AWBus �豸 */
    awbl_dev_t              dev;

    /** \brief sensor ����  */
    awbl_sensor_service_t   sensor_serv;

    /** \brief bmp280���豸�ṹ */
    aw_i2c_device_t         i2c_bmp280;

    /** \brief bmp280 �豸У׼���ݽṹ�� */
    awbl_bmp280_cali_t      bmp280_cali;

    /** \brief ���ݻ�����*/
    int32_t                 data_cache[2];
} awbl_bmp280_dev_t;

/**
 * \brief ע�� sensor-bmp280 ����
 */
void awbl_bmp280_drv_register (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AWBL_BMP280_H */
