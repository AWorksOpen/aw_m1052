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
    
/** \brief 平台相关初始化 */
aw_local void __hts221_plfm_init (void)
{
    int hts221_gpios[] = {GPIO1_27};  /*< \brief 触发引脚 */

    aw_gpio_pin_request("hts221_gpios",
                        hts221_gpios,
                        AW_NELEMENTS(hts221_gpios));
    aw_gpio_pin_cfg(GPIO1_27, AW_GPIO_INPUT);
}


/** \brief HTS221 设备信息 */
aw_local aw_const struct awbl_hts221_devinfo __g_hts221_devinfo = {

        SENSOR_HTS221_START_ID, /*< \brief 通道起始ID            */
        GPIO1_27,               /*< \brief 数据准备就绪触发引脚  */
        0x5f,                   /*< \brief IIC设备地址           */
        __hts221_plfm_init      /*< \brief 注册GPIO1_27 已经占用 */
};


/** \brief HTS221 设备实例内存静态分配 */
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
