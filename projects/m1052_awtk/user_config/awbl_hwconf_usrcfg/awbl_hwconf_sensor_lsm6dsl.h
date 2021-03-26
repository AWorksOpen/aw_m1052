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

#ifndef __AWBL_HWCONF_SENSOR_LSM6DSL_H
#define __AWBL_HWCONF_SENSOR_LSM6DSL_H


#ifdef AW_DEV_SENSOR_LSM6DSL
#include "driver/sensor/awbl_lsm6dsl.h"

/** \brief ƽ̨��س�ʼ�� */
aw_local void __lsm6dsl_plfm_init (void)
{
    int lsm6dsl_gpios[] = {GPIO1_14, GPIO1_15};

    aw_gpio_pin_request("lsm6dsl_gpios",
                        lsm6dsl_gpios,
                        AW_NELEMENTS(lsm6dsl_gpios));
    aw_gpio_pin_cfg(GPIO1_14, AW_GPIO_INPUT);
    aw_gpio_pin_cfg(GPIO1_15, AW_GPIO_INPUT);
}

/** \brief LSM6DSL �豸��Ϣ */
aw_local aw_const struct awbl_lsm6dsl_devinfo __g_lsm6dsl_devinfo = {

        SENSOR_LSM6DSL_START_ID,    /*< \brief ͨ����ʼID           */
        GPIO1_14,                   /*< \brief �������ⲿ��������1 */
        GPIO1_15,                   /*< \brief �������ⲿ��������2  */
        0x6B,                       /*< \brief IIC�豸��ַ          */
        __lsm6dsl_plfm_init,        /*< \brief ע�������Ѿ�ռ��     */
        1,                          /*< \brief ������ٶȸ����ܴ�     */
        1,                          /*< \brief ���������Ǹ����ܴ�     */
        0                           /*< \brief ������ٶ�ƫ�Ʋ�������ѡ��     */
};

/** \brief LSM6DSL �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_lsm6dsl_dev __g_lsm6dsl_dev;

#define AWBL_HWCONF_LSM6DSL                       \
    {                                             \
        AWBL_LSM6DSL_NAME,                        \
        0,                                        \
        AWBL_BUSID_I2C,                           \
        IMX1050_LPI2C1_BUSID,                     \
        (struct awbl_dev *)&__g_lsm6dsl_dev,      \
        &__g_lsm6dsl_devinfo                      \
    },

#else
#define AWBL_HWCONF_LSM6DSL

#endif  /* AW_DEV_SENSOR_LSM6DSL */

#endif  /* __AWBL_HWCONF_SENSOR_LSM6DSL_H */

/* end of file */
