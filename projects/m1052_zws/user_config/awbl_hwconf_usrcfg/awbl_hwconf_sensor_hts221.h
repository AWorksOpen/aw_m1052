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

#ifndef __AWBL_HWCONF_SENSOR_HTS221_H
#define __AWBL_HWCONF_SENSOR_HTS221_H


#ifdef AW_DEV_SENSOR_HTS221

#include "driver/sensor/awbl_hts221.h"
    
/** \brief ƽ̨��س�ʼ�� */
aw_local void __hts221_plfm_init (void)
{
    int hts221_gpios[] = {GPIO1_27};  /*< \brief �������� */

    aw_gpio_pin_request("hts221_gpios",
                        hts221_gpios,
                        AW_NELEMENTS(hts221_gpios));
    aw_gpio_pin_cfg(GPIO1_27, AW_GPIO_INPUT);
}


/** \brief HTS221 �豸��Ϣ */
aw_local aw_const struct awbl_hts221_devinfo __g_hts221_devinfo = {

        SENSOR_HTS221_START_ID, /*< \brief ͨ����ʼID            */
        GPIO1_27,               /*< \brief ����׼��������������  */
        0x5f,                   /*< \brief IIC�豸��ַ           */
        __hts221_plfm_init      /*< \brief ע��GPIO1_27 �Ѿ�ռ�� */
};


/** \brief HTS221 �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_hts221_dev __g_hts221_dev;

#define AWBL_HWCONF_HTS221                        \
    {                                             \
        AWBL_HTS221_NAME,                         \
        0,                                        \
        AWBL_BUSID_I2C,                           \
        IMX1050_LPI2C1_BUSID,                     \
        (struct awbl_dev *)&__g_hts221_dev,       \
        &__g_hts221_devinfo                       \
    },

#else
#define AWBL_HWCONF_HTS221

#endif  /* AW_DEV_SENSOR_HTS221 */

#endif  /* __AWBL_HWCONF_SENSOR_HTS221_H */

/* end of file */
