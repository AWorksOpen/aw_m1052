/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_SENSOR_BME280_H
#define __AWBL_HWCONF_SENSOR_BME280_H

#ifdef AW_DEV_SENSOR_BME280

#include "driver/sensor/awbl_bme280.h"

/** \brief BME280 �豸��Ϣ */
aw_local aw_const struct awbl_bme280_devinfo __g_bme280_devinfo = {
        SENSOR_BME280_START_ID,                 /*< \brief ͨ����ʼID       */
        0x76,                                   /*< \brief IIC�豸��ַ      */
        AWBL_BME280_FILTER_COEFFICIENT_4,       /*< \brief IIR�˲�����      */
        AWBL_BME280_OVERSAMPLE_X_8,             /*< \brief �¶ȹ���������   */
        AWBL_BME280_OVERSAMPLE_X_8,             /*< \brief ѹ������������   */
        AWBL_BME280_OVERSAMPLE_X_8,             /*< \brief ʪ�ȹ���������   */
        AWBL_BME280_NORMAL_MODE                 /*< \brief ����Ϊ����ģʽ   */
};

/** \brief BME280 �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_bme280_dev __g_bme280_dev;

#define AWBL_HWCONF_BME280                        \
    {                                             \
        AWBL_BME280_NAME,                         \
        0,                                        \
        AWBL_BUSID_I2C,                           \
        IMX1050_LPI2C1_BUSID,                     \
        (struct awbl_dev *)&__g_bme280_dev,       \
        &__g_bme280_devinfo                       \
    },
#else

#define AWBL_HWCONF_BME280

#endif  /* AW_DEV_SENSOR_BME280 */

#endif  /* __AWBL_HWCONF_SENSOR_BME280_H */

/* end of file */

