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

#ifndef __AWBL_HWCONF_SENSOR_LIS3MDL_H
#define __AWBL_HWCONF_SENSOR_LIS3MDL_H


#ifdef AW_DEV_SENSOR_LIS3MDL
#include "driver/sensor/awbl_lis3mdl.h"

/** \brief ƽ̨��س�ʼ�� */
aw_local void __lis3mdl_plfm_init (void)
{
    int lis3mdl_gpios[] = {GPIO2_28, GPIO1_30};

    aw_gpio_pin_request("lis3mdl_gpios",
                        lis3mdl_gpios,
                        AW_NELEMENTS(lis3mdl_gpios));
    aw_gpio_pin_cfg(GPIO2_28, AW_GPIO_INPUT);
    aw_gpio_pin_cfg(GPIO1_30, AW_GPIO_INPUT);
}

/** \brief LIS3MDL �豸��Ϣ */
aw_local aw_const struct awbl_lis3mdl_devinfo __g_lis3mdl_devinfo = {

        SENSOR_LIS3MDL_START_ID, /*< \brief ͨ����ʼID           */
        GPIO2_28,                /*< \brief ����׼�������������� */
        GPIO1_30,                /*< \brief ��ֵ��������         */
        0x1C,                    /*< \brief IIC�豸��ַ          */
        __lis3mdl_plfm_init      /*< \brief ע�������Ѿ�ռ��     */

};

/** \brief LIS3MDL �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_lis3mdl_dev __g_lis3mdl_dev;

#define AWBL_HWCONF_LIS3MDL                       \
    {                                             \
        AWBL_LIS3MDL_NAME,                        \
        0,                                        \
        AWBL_BUSID_I2C,                           \
        IMX1050_LPI2C1_BUSID,                     \
        (struct awbl_dev *)&__g_lis3mdl_dev,      \
        &__g_lis3mdl_devinfo                      \
    },

#else
#define AWBL_HWCONF_LIS3MDL

#endif  /* AW_DEV_SENSOR_LIS3MDL */

#endif  /* __AWBL_HWCONF_SENSOR_LIS3MDL_H */

/* end of file */
