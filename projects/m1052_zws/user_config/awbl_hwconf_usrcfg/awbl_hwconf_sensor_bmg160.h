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

#ifndef __AWBL_HWCONF_SENSOR_BMG160_H
#define __AWBL_HWCONF_SENSOR_BMG160_H

#ifdef AW_DEV_SENSOR_BMG160

#include "driver/sensor/awbl_bmg160.h"

/** \brief BMG160 �豸��Ϣ */
aw_local aw_const struct awbl_bmg160_devinfo __g_bmg160_devinfo = {
        SENSOR_BMG160_START_ID,                 /*< \brief ͨ����ʼID       */
        GPIO1_25,                               /*< \brief ����׼��������������INT1 */
        0x68,                                   /*< \brief IIC�豸��ַ      */
};

/** \brief BME280 �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_bmg160_dev __g_bmg160_dev;

#define AWBL_HWCONF_BMG160                        \
    {                                             \
        AWBL_BMG160_NAME,                         \
        0,                                        \
        AWBL_BUSID_I2C,                           \
        IMX1050_LPI2C1_BUSID,                     \
        (struct awbl_dev *)&__g_bmg160_dev,       \
        &__g_bmg160_devinfo                       \
    },
#else

#define AWBL_HWCONF_BMG160

#endif  /* AW_DEV_SENSOR_BMG160 */

#endif  /* __AWBL_HWCONF_SENSOR_BMG160_H */

/* end of file */

