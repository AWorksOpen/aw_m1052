/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief BMA253 driver
 *
 * \internal
 * \par Modification History
 * - 1.00 19-11-19 cat, first implementation.
 * \endinternal
 */

#ifndef __AWBL_HWCONF_SENSOR_BMA253_H
#define __AWBL_HWCONF_SENSOR_BMA253_H


#ifdef AW_DEV_SENSOR_BMA253

#include "driver/sensor/awbl_bma253.h"

/**
 * \brief ƽ̨��س�ʼ��
 */
aw_local void __bma253_plfm_init (void)
{
    aw_local const int bma253_pins_tabe[] = {
        GPIO1_2,
        GPIO1_3
    };
    if (aw_gpio_pin_request("bma253_pins",
                            bma253_pins_tabe,
                            AW_NELEMENTS(bma253_pins_tabe)) == AW_OK) {
        aw_gpio_pin_cfg(GPIO1_2, AW_GPIO_INPUT);
        aw_gpio_pin_cfg(GPIO1_3, AW_GPIO_INPUT);
    }
}

/** \brief BMA253 �豸��Ϣ */
aw_local aw_const struct awbl_sensor_bma253_devinfo __g_bma253_devinfo = {

    SENSOR_BMA253_START_ID,    /**< \brief ͨ����ʼID   */
    0x18,                      /**< \brief I2C�豸��ַ     */
    GPIO1_2,                   /**< \brief �жϴ�������1 */
    GPIO1_3,                   /**< \brief �жϴ�������2 */
    __bma253_plfm_init         /**< \brief ƽ̨��ʼ������ */
};

/* BMA253 �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_sensor_bma253_dev __g_bma253_dev;

#define AWBL_HWCONF_BMA253                        \
    {                                             \
        AWBL_BMA253_NAME,                         \
        0,                                        \
        AWBL_BUSID_I2C,                           \
        IMX1050_LPI2C1_BUSID,                     \
        (struct awbl_dev *)&__g_bma253_dev,       \
        &__g_bma253_devinfo                       \
    },

#else
#define AWBL_HWCONF_BMA253

#endif  /* AW_DEV_SENSOR_BMA253 */

#endif  /* __AWBL_HWCONF_SENSOR_BMA253_H */

/* end of file */
